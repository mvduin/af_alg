#pragma once
#include <linux/if_alg.h>
#include <sys/socket.h>

[[noreturn]]
void hasher( int argc, char *argv[], sockaddr_alg const &sa, unsigned hlen );
