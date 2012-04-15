#! /usr/bin/perl -w

# Test client to develop the json interface from the PIC
#
# L. Hollevoet

use strict;

use IO::Socket;
use Net::hostent;  
use Data::Dumper;

#my $server_port = 80;
#my $host = "api.pachube.com";
my $server_port = 8080;
my $host = "localhost";

my $remote = IO::Socket::INET->new( Proto     => 'tcp',
				     PeerAddr => $host,
				     PeerPort => $server_port,
				     );


unless ($remote) {die "cannot connect to server"}
print "Connected to $host\n";

$remote->autoflush(1);
print $remote json(71) . "}";

while (<$remote>) {
	my $line = $_;
	print $line;
	if ($line =~ /OK/){
		last;
	}
}
close $remote;

sub json {
	my $humi = shift();

	return <<END;
PUT /v2/feeds/55686 HTTP/1.1\r\nUser-Agent: lieven_at_likatronix.be/0.1\r
Host: api.pachube.com\r
Accept: */*\r
X-PachubeApiKey: OivrtCBI0vaCBGTN46ktyluuoqeSAKxzZXlZUEdzdGlRYz0g\r
Content-Length: 92\r
Content-Type: application/x-www-form-urlencoded\r

{
  \"version\":\"1.0.0\",
  \"datastreams\":[
    {\"id\":\"humidity\", \"current_value\":\"$humi\"}
  ]
END
}