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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "regdom.h"

int
main(int argc, char **argv)
{
    if (argc==1)
    {
        fprintf(stderr, "%s [--drop-unknown] "
                "<(fully-qualified-domain-name )+>\n",
                argv[0]);
        fprintf(stderr, "%s --dump\n",
                argv[0]);
        return 2;
    }

    void *tree = loadTldTree();
    int error = 0;

    if (!strcmp(argv[1], "--dump"))
        printTldTree(tree, "");
    else
    {
        int drop_unknown = 0;
        int i = 1;
        char *result;

        if (!strcmp(argv[1], "--drop-unknown"))
        {
            i++;
            drop_unknown = 1;
        }

        // strip subdomains from every FQDN on the command line
        for (; i < argc; i++)
        {
            // we do it this way so that this test program will call
            // all of the library interfaces
            if (drop_unknown)
                result = getRegisteredDomainDrop(argv[i], tree, 1);
            else
                result = getRegisteredDomain(argv[i], tree);

            if (!result)
            {
                printf("%s: error\n", argv[i]);
                error = 1;
            }
            else
                printf("%s: %s\n", argv[i], result);
        }
    }

    // This call is not strictly necessary, but, again, we want to make
    // sure to call all the library interfaces.  Also, it facilitates
    // running this program under valgrind to make sure the library does
    // not leak memory when used correctly.
    freeTldTree(tree);
    return error;
}
