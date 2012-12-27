#!/usr/bin/perl -w
#
# Convert tld-canon.h to punycoded format.
# usage: punycode.pl < tld-canon-utf8.h > tld-canon-punycode.h
#
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
#
# Ward van Wanrooij, 04.04.2010, ward@ward.nu
#

use IDNA::Punycode;
use strict;

binmode STDIN, ":utf8";
while (<STDIN>) {
	if (m/(char\* tldString = ")(.+)(".*)/) {
		my ($begin, $s, $end) = ($1, $2, $3);
		print $begin;
		while ($s =~ m/(.*?)([\(\):,]+)/g) {
			print encode_punycode($1), $2;
		}
		print $end;
	} else {
		print $_;
	}
}
