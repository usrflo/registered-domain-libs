/*
 * Calculate the effective registered domain of a fully qualified domain name.
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to you under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Florian Sager, 03.01.2009, sager@agitos.de, http://www.agitos.de
 * Ward van Wanrooij, 04.04.2010, ward@ward.nu
 * Ed Walker, 03.10.2012
 *
 */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "regdom.h"

/* data types */

#define ALL    '*'
#define THIS   '!'

#define CHILDREN_BITS (sizeof(unsigned int)*CHAR_BIT - CHAR_BIT)
#define CHILDREN_MAX  ((1ul << CHILDREN_BITS) - 1)

struct tldnode
{
    char *dom;
    unsigned int num_children : CHILDREN_BITS;
    char attr; // ALL, THIS, or zero
    struct tldnode **subnodes;
};
typedef struct tldnode tldnode;

/* static data */

#include "tld-canon.h"

// helper function to parse node in tldString
static int
readTldString(tldnode *node, const char *s, int len, int pos)
{
    int start = pos;
    int state = 0;

    memset(node, 0, sizeof(tldnode));
    do
    {
        char c = s[pos];

        switch (state)
        {
        case 0: // general read
            if (c == ',' || c == ')' || c == '(')
            {
                // add last domain
                int lenc = node->attr ? pos - start - 1 : pos - start;
                node->dom = malloc(lenc + 1);
                memcpy(node->dom, s + start, lenc);
                node->dom[lenc] = 0;

                if (c == '(')
                {
                    // read number of children
                    start = pos;
                    state = 1;
                }
                else if (c == ')' || c == ',')
                    // return to parent domains
                    return pos;
            }
            else if (c == ALL || c == THIS)
                node->attr = c;
            break;

        case 1: // reading number of elements (<number>:
            if (c == ':')
            {
                char *endptr;
                errno = 0;
                unsigned long n = strtoul(s + start + 1, &endptr, 10);
                if (endptr != s + pos || errno || n > CHILDREN_MAX)
                    abort();

                node->num_children = n;

                // allocate space for children
                node->subnodes = malloc(n * sizeof(tldnode *));
                for (unsigned long i = 0; i < n; i++)
                {
                    node->subnodes[i] = malloc(sizeof(tldnode));
                    pos = readTldString(node->subnodes[i], s, len, pos + 1);
                }

                return pos + 1;
            }
            break;
        }
        pos++;
    }
    while (pos < len);

    return pos;
}

// Read TLD string into fast-lookup data structure
void *
loadTldTree(void)
{
    tldnode *root = malloc(sizeof(tldnode));

    readTldString(root, tldString, sizeof tldString - 1, 0);

    return root;
}

static void
printTldTreeI(tldnode *node, const char *spacer)
{
    if (node->attr)
        printf("%s%s: %c\n", spacer, node->dom, node->attr);
    else
        printf("%s%s:\n", spacer, node->dom);

    if (node->num_children > 0)
    {
        size_t n = strlen(spacer);
        char nspacer[n+2+1];
        memcpy(nspacer, spacer, n);
        nspacer[n]   = ' ';
        nspacer[n+1] = ' ';
        nspacer[n+2] = '\0';

        for (unsigned int i = 0; i < node->num_children; i++)
            printTldTreeI(node->subnodes[i], nspacer);
    }
}

void
printTldTree(void *node, const char *spacer)
{
    if (!spacer)
        spacer = "";
    printTldTreeI((tldnode *) node, spacer);
}

static void
freeTldTreeI(tldnode *node)
{
    for (unsigned int i = 0; i < node->num_children; i++)
        freeTldTreeI(node->subnodes[i]);
    free(node->subnodes);
    free(node->dom);
    free(node);
}

void
freeTldTree(void *root)
{
    freeTldTreeI((tldnode *) root);
}

// linear search for domain (and * if available)
static tldnode *
findTldNode(tldnode *parent, const char *seg_start, const char *seg_end)
{
    tldnode *allNode = 0;

    for (unsigned int i = 0; i < parent->num_children; i++)
    {
        if (!allNode && parent->subnodes[i]->attr == ALL)
            allNode = parent->subnodes[i];
        else
        {
            size_t m = seg_end - seg_start;
            size_t n = strlen(parent->subnodes[i]->dom);
            if (m == n && !memcmp(parent->subnodes[i]->dom, seg_start, n))
                return parent->subnodes[i];
        }
    }
    return allNode;
}

static char *
getRegisteredDomainDropI(const char *hostname, tldnode *tree,
                         int drop_unknown)
{
    // Eliminate some special (always-fail) cases first.
    if (hostname[0] == '.' || hostname[0] == '\0')
        return 0;

    // The registered domain will always be a suffix of the input hostname.
    // Start at the end of the name and work backward.
    const char *head = hostname;
    const char *seg_end = hostname + strlen(hostname);
    const char *seg_start;

    if (seg_end[-1] == '.')
        seg_end--;
    seg_start = seg_end;

    for (;;) {
        while (seg_start > head && *seg_start != '.')
            seg_start--;
        if (*seg_start == '.')
            seg_start++;

        // [seg_start, seg_end) is one label.
        tldnode *subtree = findTldNode(tree, seg_start, seg_end);
        if (!subtree
            || (subtree->num_children == 1
                && subtree->subnodes[0]->attr == THIS))
            // Match found.
            break;

        if (seg_start == head)
            // No match, i.e. the input name is too short to be a
            // registered domain.
            return 0;

        // Advance to the next label.
        tree = subtree;

        if (seg_start[-1] != '.')
            abort();
        seg_end = seg_start - 1;
        seg_start = seg_end - 1;
    }

    // Ensure the stripped domain contains at least two labels.
    if (!strchr(seg_start, '.'))
    {
        if (seg_start == head || drop_unknown)
            return 0;

        seg_start -= 2;
        while (seg_start > head && *seg_start != '.')
            seg_start--;
        if (*seg_start == '.')
            seg_start++;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
    return (char *)seg_start;
#pragma GCC diagnostic pop
}

char *
getRegisteredDomainDrop(const char *hostname, void *tree, int drop_unknown)
{
    return getRegisteredDomainDropI(hostname, (tldnode *) tree, drop_unknown);
}

char *
getRegisteredDomain(const char *hostname, void *tree)
{
    return getRegisteredDomainDropI(hostname, (tldnode *) tree, 0);
}
