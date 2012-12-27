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
 *
 */

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "dkim-regdom.h"
#include "tld-canon.h"

int main(int argc, char* argv[]) {

	if (argc==1) {
		printf("%s <(fully-qualified-domain-name )+>\n", argv[0]);
		return 0;
	}

	// read TLDs only once at daemon startup
	tldnode* tree = readTldTree(tldString);

	#ifdef DEBUG
	// this is for debug output only
	printTldTree(tree, "");
	printf("---\n\n");
	#endif /* DEBUG */

	// strip subdomains from every signing domain
	// char dom[] = "sub2.sub.registered.nom.ad";
	int i;
	for (i=1; i<argc; i++) {
		char* result = getRegisteredDomain((char*) argv[i], tree);

		if (result==NULL) {
			printf("error: %s\n", argv[i]);
		} else {
			printf("%s\n", result);
		}
		fflush(stdout);
	}

	// free only once before daemon exit
	freeTldTree(tree);

	return 0;
}
