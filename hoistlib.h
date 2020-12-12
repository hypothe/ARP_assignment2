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
#define STEP 5 // the hoist speed is 5 cm/s

// Server - Reader message structure
typedef struct message* msg_t;

// Hoist statuses
enum status {STOP = 'S', UP = 'U', DOWN = 'D', EXIT = 'E', TOP = 'T', BOTTOM = 'B'};

// ------------- Utility functions ------------------

//Throw error.
//arguments:
//msg - error message
//ret - exit status
void error(char *msg, int ret);

//Spawn a child process.
//arguments:
//ex_name - path to the process executable
//fd_log - log file descriptor
//fd_in - input file descriptor (might be socket or pipe)
//fd_out - output file descriptor (might be socket or pipe)
//new_shell - whether the child process should run in a new shell
int spawn(char *ex_name, int fd_log, int *fd_in, int *fd_out, char new_shell);

// -------------- Height manipulation functions -------------------------------
//arguments:
//*height - pointer to the hoist height variable

//If height >= maximum height, set it to maximum height, otherwise increase by STEP.
void height_tryup  	(int *height);
//If height <= minimum height, set it to minimum height, otherwise decrease by STEP.
void height_trydown	(int *height);

// -------------- Message manipulation functions -------------------------------
//arguments:
//p_msg - pointer to the message structure
//height - hoist height variable
//status - status to be assigned

//set the message height field
void msg_setheight	(msg_t p_msg, int height);
//set the message status field
void msg_setstatus	(msg_t p_msg, char status);
//get the message height field
int  msg_getheight	(msg_t p_msg);
//get the message status field
char msg_getstatus	(msg_t p_msg);
//get the message size
int  msg_getsize	();
//initialize the message structure (height to 0, status to BOTTOM)
void msg_init		(msg_t *pp_msg);
//free the memory occupated by the message
void msg_free		(msg_t p_msg);
#endif
