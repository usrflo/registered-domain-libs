FQDNS="registered.com sub.registered.com parliament.uk sub.registered.valid.uk registered.somedom.kyoto.jp invalid-fqdn org academy.museum sub.academy.museum subsub.sub.academy.museum sub.nic.pa registered.sb sub.registered.sb subsub.registered.something.zw subsub.registered.9.bg registered.co.bi sub.registered.bi subsub.registered.ee"

echo '### INPUT ###';
echo $FQDNS | sed 's/ /\n/g';

echo '\n### OUTPUT ###';
perl test-regDomain.pl $FQDNS
