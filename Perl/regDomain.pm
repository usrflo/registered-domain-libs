# <@LICENSE>
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
#
# Florian Sager, 2009-01-11, sager@agitos.de, http://www.agitos.de
#

package regDomain;

=item getRegisteredDomain($signingdomain)

Remove subdomains from a signing domain to get the registered domain.

dkim-reputation.org blocks signing domains on the level of registered domains
to rate senders who use e.g. a.spamdomain.tld, b.spamdomain.tld, ... under
the most common identifier - the registered domain - finally.

=cut

sub getRegisteredDomain {

	my ($signingDomain, $treeNode_ref) = @_;

	my @signingDomainParts = split( /\./, $signingDomain );

	my $result = findRegisteredDomain( $treeNode_ref, \@signingDomainParts );

	if ( ! defined($result) ) {

		# this is an invalid domain name
		return undef;
	}

	# assure there is at least 1 TLD in the stripped signing domain
	if ( index( $result, '.' ) == -1 ) {
		my $cnt = @signingDomainParts;
		return undef if ($cnt == 1);
		return $signingDomainParts[ $cnt - 2 ] . '.' . $signingDomainParts[ $cnt - 1 ];
	}
	else {
		return $result;
	}
}

# recursive helper method
sub findRegisteredDomain {

	my $treeNode_ref = $_[0];			# &$treeNode
	my @remainingSigningDomainParts = @{ $_[1] };	# copy to new variable

	my $sub = delete($remainingSigningDomainParts[$#remainingSigningDomainParts]);

	if ( !$sub ) {
		$sub = undef;
	}

	my $result = undef;

	if ( exists $treeNode_ref->{'!'} ) {
		return '#';
	}
	elsif ( exists $treeNode_ref->{$sub} ) {
		$result = findRegisteredDomain( $treeNode_ref->{$sub}, \@remainingSigningDomainParts );
	}
	elsif ( exists $treeNode_ref->{'*'} ) {
		$result = findRegisteredDomain( $treeNode_ref->{'*'}, \@remainingSigningDomainParts );
	}
	else {
		return $sub;
	}

	if ( $result eq '#' ) {
		return $sub;
	}
	elsif ( defined($result) && length($result) > 0 ) {
		return $result . '.' . $sub;
	}
	return undef;
}

return 1;
