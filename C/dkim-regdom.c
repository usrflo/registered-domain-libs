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

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "dkim-regdom.h"

static char* ALL = "*";
static char* THIS = "!";

// char* tldString = "root(3:ac(5:com,edu,gov,net,ad(3:nom,co!,*)),de,com)";

// helper function to parse node in tldString
int readTldString(tldnode* node, char* s, int len, int pos) {

	int start = pos;
	int state = 0;

	memset(node, 0, sizeof(tldnode));
	do {
		char c = *(s+pos);

		switch(state) {
			case 0: // general read

				if (c==',' || c==')' || c=='(') {
					// add last domain
					int lenc = node->attr == THIS ? pos - start - 1:pos - start;
					node->dom = (char*) malloc(lenc + 1);
					memcpy(node->dom, s+start, lenc);
					(node->dom)[lenc] = 0;

					if (c=='(') {
						// read number of children
						start = pos;
						state = 1;
					} else if (c==')' || c==',') {
						// return to parent domains
						return pos;
					}

				} else if (c=='!') {
					node->attr=THIS;
				}

				break;
			case 1: // reading number of elements (<number>:

				if (c==':') {
					char* buf = (char*) malloc((pos - start - 1) + 1);
					memcpy(buf, s+start+1, pos - start - 1);
					buf[pos - start - 1] = 0;				
					node->num_children = atoi(buf);
					free(buf);

					// allocate space for children
					node->subnodes = malloc(node->num_children * sizeof(tldnode*));

					int i;
					for (i=0; i<node->num_children; i++) {
						node->subnodes[i] = (tldnode*)malloc(sizeof(tldnode));
						pos = readTldString(node->subnodes[i], s, len, pos + 1);
					}

					return pos + 1;
				}

				break;
		}

		pos++;
	} while (pos < len);

	return pos;
}

// reads TLDs once at daemon startup
tldnode* readTldTree(char* tlds) {
	tldnode* root = (tldnode *)malloc(sizeof(tldnode));

	readTldString(root, tlds, strlen(tlds), 0);

	return root;
}

#ifdef DEBUG

void printTldTree(tldnode* node, const char * spacer) {
	if (node->num_children != 0) {
		// has children
		printf("%s%s:\n", spacer, node->dom);

		int i;
		for(i = 0; i < node->num_children; i++) {
			char dest[100];
			sprintf(dest, "  %s", spacer);

			printTldTree(node->subnodes[i], dest);
		}
	} else {
		// no children
		printf("%s%s: %s\n", spacer, node->dom, node->attr);
	}
}

#endif /* DEBUG */

void freeTldTree(tldnode* node) {

	if (node->num_children != 0) {
		int i;
		for(i = 0; i < node->num_children; i++) {
			freeTldTree(node->subnodes[i]);
		}
	}
	free(node->dom);
	free(node);
}

// linear search for domain (and * if available)
tldnode* findTldNode(tldnode* parent, char* subdom) {

	tldnode* allNode = NULL;

	int i;
	for (i=0; i<parent->num_children; i++) {
		if (strcmp(subdom, parent->subnodes[i]->dom) == 0) {
			return parent->subnodes[i];
		}
		if (allNode==NULL && strcmp(ALL, parent->subnodes[i]->dom) == 0) {
			allNode = parent->subnodes[i];
		}
	}
	return allNode;
}

// concatenate a domain with its parent domain
char* concatDomLabel(char* dl, char* du) {

	char* s;

	if (dl == NULL) {
		s = (char*) malloc(strlen(du)+1);
		strcpy(s, du);
	} else {
		s = (char*) malloc(strlen(dl)+1+strlen(du)+1);
		strcpy(s, dl);
		strcat(s, ".");
		strcat(s, du);
	}
	return s;
}

// recursive helper method
char* findRegisteredDomain(tldnode* subtree, dlist* dom) {

	tldnode* subNode = findTldNode(subtree, dom->val);
	if (subNode==NULL || (subNode->num_children==1 && subNode->subnodes[0]->attr == THIS)) {
		char* domain = (char*) malloc(strlen(dom->val)+1);
		strcpy(domain, dom->val);
		return domain;
	} else if (dom->next==NULL) {
		return NULL;
	}

	char* fRegDom = findRegisteredDomain(subNode, dom->next);
	char* concDomain = NULL;
	if (fRegDom!=NULL) {
		concDomain = concatDomLabel(fRegDom, dom->val);
		free(fRegDom);
	}

	return concDomain;
}

void freeDomLabels(dlist* head, char* sDcopy) {

	dlist* cur;

	// free list of separated domain parts
	while (head) {
		cur = head;
		head = cur->next;
		free(cur);
	}

	free(sDcopy);
}

char* getRegisteredDomain(char* signingDomain, tldnode* tree) {

	dlist *cur, *head = NULL;
	char *saveptr;

	// split domain by . separator
	char* sDcopy = (char*) malloc(strlen(signingDomain)+1);
	strcpy(sDcopy, signingDomain);
	char* token = strtok_r(sDcopy, ".", &saveptr);
	while (token != NULL) {
		cur = (dlist*) malloc(sizeof(dlist));
		cur->val = token;
		cur->next = head;
		head = cur;
		token = strtok_r(NULL, ".", &saveptr);
	}

	char* result = findRegisteredDomain(tree, head);

	if (result==NULL) {
		freeDomLabels(head, sDcopy);
		return NULL;
	}

	// assure there is at least 1 TLD in the stripped signing domain
	if (strchr(result, '.')==NULL) {
		free(result);
		if (head->next == NULL) {
			freeDomLabels(head, sDcopy);
			return NULL;
		} else {
			char* minDomain = concatDomLabel(head->next->val, head->val);
			freeDomLabels(head, sDcopy);
			return minDomain;
		}
	}

	freeDomLabels(head, sDcopy);
	return result;
}
