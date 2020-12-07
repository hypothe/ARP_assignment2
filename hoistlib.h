#ifndef _HOISTLIB_H_
#define _HOISTLIB_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>

#define MAX_HEIGHT 200
#define MIN_HEIGHT 0
#define STEP 5
#define MAX_STEP MAX_HEIGHT/STEP

typedef struct message{
	int height;
	char status;
} msg_t;

void error(char *msg, int ret);

int spawn(char *ex_name, int fd_log, int *fd_in, int *fd_out, char new_shell);



#endif
