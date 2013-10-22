#! /bin/sh

# Test data taken from
# https://mxr.mozilla.org/mozilla-central/source/netwerk/test/unit/data/test_psl.txt?raw=1
# Therefore: Any copyright is dedicated to the Public Domain.
# http://creativecommons.org/publicdomain/zero/1.0/

TESTDATA="
# Null input.
: error
# Mixed case (not handled by library, so case-folded here)
com: error
example.com: example.com
www.example.com: example.com
# Leading dot.
.com: error
.example: error
.example.com: error
.example.example: error
# Unlisted TLD.
example: error
example.example: example.example
b.example.example: example.example
a.b.example.example: example.example
# Listed, but non-Internet, TLD.
#local: error
#example.local: error
#b.example.local: error
#a.b.example.local: error
# TLD with only 1 rule.
biz: error
domain.biz: domain.biz
b.domain.biz: domain.biz
a.b.domain.biz: domain.biz
# TLD with some 2-level rules.
com: error
example.com: example.com
b.example.com: example.com
a.b.example.com: example.com
uk.com: error
example.uk.com: example.uk.com
b.example.uk.com: example.uk.com
a.b.example.uk.com: example.uk.com
test.ac: test.ac
# TLD with only 1 (wildcard) rule.
cy: error
c.cy: error
b.c.cy: b.c.cy
a.b.c.cy: b.c.cy
# More complex TLD.
jp: error
test.jp: test.jp
www.test.jp: test.jp
ac.jp: error
test.ac.jp: test.ac.jp
www.test.ac.jp: test.ac.jp
kyoto.jp: error
test.kyoto.jp: test.kyoto.jp
ide.kyoto.jp: error
b.ide.kyoto.jp: b.ide.kyoto.jp
a.b.ide.kyoto.jp: b.ide.kyoto.jp
c.kobe.jp: error
b.c.kobe.jp: b.c.kobe.jp
a.b.c.kobe.jp: b.c.kobe.jp
city.kobe.jp: city.kobe.jp
www.city.kobe.jp: city.kobe.jp
# TLD with a wildcard rule and exceptions.
ck: error
test.ck: error
b.test.ck: b.test.ck
a.b.test.ck: b.test.ck
www.ck: www.ck
www.www.ck: www.ck
# US K12.
us: error
test.us: test.us
www.test.us: test.us
ak.us: error
test.ak.us: test.ak.us
www.test.ak.us: test.ak.us
k12.ak.us: error
test.k12.ak.us: test.k12.ak.us
www.test.k12.ak.us: test.k12.ak.us
"

if [ x"$1" = x"--grind" ]; then
    CMD="valgrind ./test-regdom"
else
    CMD="./test-regdom"
fi

expected=$(mktemp expected.XXXXXX)
output=$(mktemp output.XXXXXX)
trap "rm -f '$expected' '$output'" 0

printf '%s' "$TESTDATA" | sed -e '/^#/d; s/ *$//; /^$/d' > "$expected"
input=$(printf '%s' "$TESTDATA" | sed -e '/^#/d; s/ *$//; /^$/d; s/:.*$//')

# Note: the null-string input at the very top of TESTDATA is forced into
# the command line below manually.
LD_LIBRARY_PATH=. $CMD '' $input > "$output"
if cmp -s "$expected" "$output"; then
    exit 0
else
    diff -u "$expected" "$output"
    exit 1
fi
