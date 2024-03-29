# Detection of registered domain names by reg-dom libs

The reg-dom libs are available in PHP, Perl and C.

They include recent representations of the effective TLD list available at
https://publicsuffix.org/list/effective_tld_names.dat
and help to convert an arbitrary domain name to the registered domain name.
Upon changes of `effective_tld_names.dat` this git repository is automatically updated once a day.
The library supports Unicode (IDN notation) for domain name input and output. ACE notation is not supported currently.

### Sample Use
Signing domains in DKIM signatures can be used on the level of registered domains
to rate senders who use e.g. a.spamdomain.tld, b.spamdomain.tld, ... under
the most common identifier - the registered domain - finally.
Project page: https://www.agitos.de/registered-domain-libs/
Updated library downloads (besides Github): https://services.agitos.de/regdom-lib-downloads/

### Pseudo code
registeredDomain = getRegisteredDomain(ingoingDomain);

### Return values
1) NULL if ingoingDomain is a TLD
2) the registered domain name if TLD is known
3) just <domain>.<tld> if <tld> is unknown
   This case was added to support new TLDs in outdated reg-dom libs
   by a certain likelihood. This fallback method is implemented in the
   last conversion step and can be simply commented out.

---

If you like to regenerate the effective TLD tree structure by yourself
you can use the script `generateEffectiveTLDs.php` with the following parameters:

```
php generateEffectiveTLDs.php php  > PHP/effectiveTLDs.inc.php
php generateEffectiveTLDs.php perl > Perl/effectiveTLDs.pm
php generateEffectiveTLDs.php c    > C/tld-canon.h
```

### License
```
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.
# The ASF licenses this file to you under the Apache License, Version 2.0
# (the "License"); you may not use this file except in compliance with
# the License.  You may obtain a copy of the License at:
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# </@LICENSE>
```

Florian Sager, 2009-02-05, sager@agitos.de, http://www.agitos.de
