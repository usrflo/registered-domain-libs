/*
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
 *
 * Florian Sager, 03.01.2009, sager@agitos.de, http://www.agitos.de
 * Ward van Wanrooij, 04.04.2010, ward@ward.nu
 *
 */

#ifndef _DKIM_REGDOM_H_
#define _DKIM_REGDOM_H_

/* DATA TYPES */
struct tldnode_el {
	char* dom;
	char* attr;

	int num_children;
	struct tldnode_el** subnodes;
};

typedef struct tldnode_el tldnode;

struct dlist_el {
	char* val;

	struct dlist_el* next;
};

typedef struct dlist_el dlist;

/* PROTOTYPES */
extern int readTldString(tldnode*,char*,int,int);
extern tldnode* findTldNode(tldnode*,char*);
extern char* concatDomLabel(char*,char*);
extern char* findRegisteredDomain(tldnode*,dlist*);
extern void freeDomLabels(dlist*,char*);
extern tldnode* readTldTree(char*);
extern char* getRegisteredDomain(char*,tldnode*);
extern void freeTldTree(tldnode*);

#ifdef DEBUG
extern void printTldTree(tldnode*, const char *);
#endif /* DEBUG */

#endif /*_DKIM_REGDOM_H_*/
