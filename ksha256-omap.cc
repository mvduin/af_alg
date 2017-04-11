#include "hasher.h"

int main( int argc, char *argv[] )
{
	sockaddr_alg const sa { AF_ALG, "hash", 0, 0, "omap-sha256" };
	hasher( argc, argv, sa, 32 );
}
