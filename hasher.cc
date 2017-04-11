#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <linux/if_alg.h>
#include <linux/socket.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include "hasher.h"

using u8 = uint8_t;

[[ noreturn ]]
inline void die( char const *fmt, ... )
{
	va_list ap;
	char *s;
	va_start( ap, fmt );
	if( vasprintf( &s, fmt, ap ) < 0 ) {
		fprintf( stderr, "out of memory\n" );
	} else {
		fprintf( stderr, "%s\n", s );
		free( s );
	}
	va_end( ap );
	exit( EXIT_FAILURE );
}

[[noreturn]]
void hasher( int argc, char *argv[], sockaddr_alg const &sa, unsigned hlen )
{
	if( argc > 1 )
		die( "no arguments supported (currently)" );

	int cfgfd = socket( AF_ALG, SOCK_SEQPACKET, 0 );
	if( cfgfd < 0 )
		die( "socket: %m" );
	if( bind( cfgfd, (sockaddr const *) &sa, sizeof sa ) < 0 )
		die( "bind: %m" );
	int fd = accept( cfgfd, NULL, 0 );
	if( fd < 0 )
		die( "accept: %m" );
	close(cfgfd);


	size_t const chunksize = 1 << 20;
	size_t total = 0;

	while(true) {
		ssize_t n = splice( 0, NULL, fd, NULL, chunksize, SPLICE_F_MORE );
		if( n < 0 ) {
			if( errno == EINVAL || errno == ENOSYS )
				goto try_sendfile;
			die( "splice: %m" );
		}
		if( n == 0 )
			goto done;
		total += n;
	}

try_sendfile:
	while(true) {
		ssize_t n = sendfile( fd, 0, NULL, chunksize );
		if( n < 0 ) {
			if( errno == EINVAL || errno == ENOSYS )
				goto fallback;
			die( "sendfile: %m" );
		}
		if( n == 0 )
			goto done;
		total += n;
	}

fallback:
	die( "I'm too lazy" );

done:
	u8 hash[ hlen ];
	ssize_t n = read( fd, hash, sizeof hash );
	if( n < 0 )
		die( "read: %m" );
	if( (size_t) n != sizeof hash )
		die( "read: expected %u bytes, got %u", sizeof hash, n );
	close(fd);
	for (u8 x : hash)
		printf("%02x", x);
	printf("\n");
	exit(0);
}
