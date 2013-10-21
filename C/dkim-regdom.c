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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "dkim-regdom.h"

/* data types */

struct tldnode_el
{
    char *dom;
    const char *attr;

    int num_children;
    struct tldnode_el **subnodes;
};

typedef struct tldnode_el tldnode;

struct dlist_el
{
    const char *val;

    struct dlist_el *next;
};

typedef struct dlist_el dlist;

/* static data */

#include "tld-canon.h"

static const char ALL[] = "*";
static const char THIS[] = "!";

// helper function to parse node in tldString
static int
readTldString(tldnode * node, const char *s, int len, int pos)
{
    int start = pos;
    int state = 0;

    memset(node, 0, sizeof(tldnode));
    do
    {
        char c = *(s + pos);

        switch (state)
        {
        case 0:                // general read

            if (c == ',' || c == ')' || c == '(')
            {
                // add last domain
                int lenc = node->attr == THIS ? pos - start - 1 : pos - start;
                node->dom = (char *) malloc(lenc + 1);
                memcpy(node->dom, s + start, lenc);
                (node->dom)[lenc] = 0;

                if (c == '(')
                {
                    // read number of children
                    start = pos;
                    state = 1;
                }
                else if (c == ')' || c == ',')
                {
                    // return to parent domains
                    return pos;
                }

            }
            else if (c == '!')
            {
                node->attr = THIS;
            }

            break;
        case 1:                // reading number of elements (<number>:

            if (c == ':')
            {
                char *buf = (char *) malloc((pos - start - 1) + 1);
                memcpy(buf, s + start + 1, pos - start - 1);
                buf[pos - start - 1] = 0;
                node->num_children = atoi(buf);
                free(buf);

                // allocate space for children
                node->subnodes =
                    malloc(node->num_children * sizeof(tldnode *));

                int i;
                for (i = 0; i < node->num_children; i++)
                {
                    node->subnodes[i] = (tldnode *) malloc(sizeof(tldnode));
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
    tldnode *root = (tldnode *) malloc(sizeof(tldnode));

    readTldString(root, tldString, sizeof tldString - 1, 0);

    return root;
}

static void
printTldTreeI(tldnode * node, const char *spacer)
{
    if (node->num_children != 0)
    {
        // has children
        printf("%s%s:\n", spacer, node->dom);

        int i;
        for (i = 0; i < node->num_children; i++)
        {
            char dest[100];
            sprintf(dest, "  %s", spacer);

            printTldTreeI(node->subnodes[i], dest);
        }
    }
    else
    {
        // no children
        printf("%s%s: %s\n", spacer, node->dom, node->attr);
    }
}

void
printTldTree(void *node, const char *spacer)
{
    printTldTreeI((tldnode *) node, spacer);
}

static void
freeTldTreeI(tldnode * node)
{
    if (node->num_children != 0)
    {
        int i;
        for (i = 0; i < node->num_children; i++)
        {
            freeTldTree(node->subnodes[i]);
        }
    }
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
findTldNode(tldnode * parent, const char *subdom)
{
    tldnode *allNode = NULL;

    int i;
    for (i = 0; i < parent->num_children; i++)
    {
        if (strcmp(subdom, parent->subnodes[i]->dom) == 0)
        {
            return parent->subnodes[i];
        }
        if (allNode == NULL && strcmp(ALL, parent->subnodes[i]->dom) == 0)
        {
            allNode = parent->subnodes[i];
        }
    }
    return allNode;
}

// concatenate a domain with its parent domain
static char *
concatDomLabel(const char *dl, const char *du)
{

    char *s;

    if (dl == NULL)
    {
        s = (char *) malloc(strlen(du) + 1);
        strcpy(s, du);
    }
    else
    {
        s = (char *) malloc(strlen(dl) + 1 + strlen(du) + 1);
        strcpy(s, dl);
        strcat(s, ".");
        strcat(s, du);
    }
    return s;
}

// recursive helper method
static char *
findRegisteredDomain(tldnode * subtree, dlist * dom)
{
    tldnode *subNode = findTldNode(subtree, dom->val);
    if (subNode == NULL
        || (subNode->num_children == 1 && subNode->subnodes[0]->attr == THIS))
    {
        char *domain = (char *) malloc(strlen(dom->val) + 1);
        strcpy(domain, dom->val);
        return domain;
    }
    else if (dom->next == NULL)
    {
        return NULL;
    }

    char *fRegDom = findRegisteredDomain(subNode, dom->next);
    char *concDomain = NULL;
    if (fRegDom != NULL)
    {
        concDomain = concatDomLabel(fRegDom, dom->val);
        free(fRegDom);
    }

    return concDomain;
}

static void
freeDomLabels(dlist * head, char *sDcopy)
{

    dlist *cur;

    // free list of separated domain parts
    while (head)
    {
        cur = head;
        head = cur->next;
        free(cur);
    }

    free(sDcopy);
}

static char *
getRegisteredDomainDropI(const char *hostname, tldnode * tree,
                         int drop_unknown)
{

    dlist *cur, *head = NULL;
    char *saveptr = NULL;
    char *result = NULL;

    // split domain by . separator
    char *sDcopy = (char *) malloc(strlen(hostname) + 1);
    strcpy(sDcopy, hostname);
    char *token = strtok_r(sDcopy, ".", &saveptr);
    while (token != NULL)
    {
        cur = (dlist *) malloc(sizeof(dlist));
        cur->val = token;
        cur->next = head;
        head = cur;
        token = strtok_r(NULL, ".", &saveptr);
    }

    if (head != NULL)
        result = findRegisteredDomain(tree, head);

    if (result == NULL)
    {
        freeDomLabels(head, sDcopy);
        return NULL;
    }

    // assure there is at least 1 TLD in the stripped domain
    if (strchr(result, '.') == NULL)
    {
        free(result);
        if (head->next == NULL)
        {
            freeDomLabels(head, sDcopy);
            return NULL;
        }
        else if (drop_unknown)
            return NULL;
        else
        {
            char *minDomain = concatDomLabel(head->next->val, head->val);
            freeDomLabels(head, sDcopy);
            return minDomain;
        }
    }

    freeDomLabels(head, sDcopy);
    return result;
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
