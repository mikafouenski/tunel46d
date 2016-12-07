#ifndef __EXTREMITE_H__
#define __EXTREMITE_H__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>

#define BUFFSIZE 4096

void ext_out(char* port, int tun);
void ext_in(char* hote, char* port, int tun);

#endif
