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

typedef struct message* msg_t;
enum status {STOP = 'S', UP = 'U', DOWN = 'D', EXIT = 'E', TOP = 'T', BOTTOM = 'B'};

void error(char *msg, int ret);

int spawn(char *ex_name, int fd_log, int *fd_in, int *fd_out, char new_shell);

void height_tryup  	(int *height);
void height_trydown	(int *height);

void msg_setheight	(msg_t p_msg, int height);
void msg_setstatus	(msg_t p_msg, char status);
int  msg_getheight	(msg_t p_msg);
char msg_getstatus	(msg_t p_msg);
int  msg_getsize	();
void msg_init		(msg_t *pp_msg);
void msg_free		(msg_t p_msg);
#endif
