#include "hoistlib.h"

struct message{
	int height;
	char status;
};

// ------------- Utility functions ------------------ 

void error(char *msg, int ret) {
    perror(msg);
    exit(ret); // error propagation
}

int spawn(char* ex_name, int fd_log, int *fd_in, int *fd_out, char new_shell) {
	int ret;
	pid_t child_pid = fork();
	char *args[7];
	int i = 0;
	if (child_pid != 0)
	{
		/* Father closes fd_in[0], since it will write on it */
		/* Father closes fd_out[1], since it will read from it */
		/*if (fd_in!=NULL) 	close(fd_in[0]);
		if (fd_out!=NULL)	close(fd_out[1]);*/
		return child_pid;
	}
	else
	{
		/* Child closes fd_in[1], since it will read from it */
		/* Father closes fd_out[0], since it will write on it */
		char tmp_log[5] = "", tmp_in[5]="", tmp_out[5]="";
		sprintf(tmp_log, "%d", fd_log);
		if (fd_in!=NULL)	{close(fd_in[1]); sprintf(tmp_in, "%d", fd_in[0]);}
		if (fd_out!=NULL)	{close(fd_out[0]); sprintf(tmp_out, "%d", fd_out[1]);}

		if (new_shell){
			args[i++] = "/usr/bin/konsole";
			args[i++] = "-e";
		}
		args[i++] = ex_name;
		args[i++] = tmp_log;
		args[i++] = tmp_in;
		args[i++] = tmp_out;
		args[i++] = (char*)NULL;

		ret = execvp(args[0], args);
		perror("exec failed");
		exit(ret);
	}
}

/* --------------Message manipulation functions ---------------------------------- */
void height_tryup  	(int *height){
	*height = *height>=MAX_HEIGHT? MAX_HEIGHT:*height + STEP;
}
void height_trydown	(int *height){
	*height = *height<=MIN_HEIGHT? MIN_HEIGHT:*height - STEP;
}

void msg_init		(msg_t *pp_msg){
	(*pp_msg) = (msg_t)malloc(sizeof(struct message));
	(*pp_msg)->height = 0;
	(*pp_msg)->status = BOTTOM;
}

void msg_free(msg_t p_msg){
	/*	
	Function to free the memory allocated for a message of type 
	struct message
	
	Used to deallocate memory, not necessary but cleaner

	Arguments
	p_msg (msg_t)	:	pointer to the message to free up
	*/
	free(p_msg);
}

void msg_setheight	(msg_t p_msg, int height){
	p_msg->height = height;
}
void msg_setstatus	(msg_t p_msg, char status){
	p_msg->status = status;
}
int  msg_getheight	(msg_t p_msg){
	return p_msg->height;
}
char msg_getstatus	(msg_t p_msg){
	return p_msg->status;
}
int  msg_getsize	(){
	return sizeof(struct message);
}