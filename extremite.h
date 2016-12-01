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

int ext_out(char* port);
int ext_in();

#endif
