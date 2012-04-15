#! /usr/bin/perl -w

# Test server to develop the json interface from the PIC
#
# L. Hollevoet

use strict;

use IO::Socket;
use Net::hostent;  
use Data::Dumper;

my $server_port = 8080;

my $server = IO::Socket::INET->new( Proto     => 'tcp',
				     LocalPort => $server_port,
				     Listen    => SOMAXCONN,
				     Reuse     => 1);
my $client;
my $framecount=0;

# Try starting the server
die "can't setup server" unless $server;
print "[Server $0 accepting clients on port $server_port...]\n";

# Main program loop, only one client at a time
while ($client = $server->accept()) {
    # Autoflush to prevent buffering
    $client->autoflush(1);

    print "[< Accepted connection from client...]\n";

    while ( <$client>) {
        my $frame = $_;
		print $frame;
		if ($frame =~ /^PUT/){
			print $client "<html></html>\n";
		}
		
    }
    
    # Once here, the client disconnected
    print "[> Remote peer disconnected ]\n";
    
    # Close and cleanup
    close $client;    
}
