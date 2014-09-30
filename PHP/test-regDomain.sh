FQDNS="registered.com sub.registered.com parliament.uk sub.registered.valid.uk registered.somedom.kyoto.jp invalid-fqdn org academy.museum sub.academy.museum subsub.sub.academy.museum sub.nic.pa registered.sb sub.registered.sb subsub.registered.something.zw subsub.registered.9.bg registered.co.bi sub.registered.bi subsub.registered.ee s.tld -test.tld g%d.t/l"

echo '### INPUT ###';
echo
echo $FQDNS | sed 's/\ /\'$'\n/g';
echo
echo '### OUTPUT ###';
php test-regDomain.php $FQDNS
echo
