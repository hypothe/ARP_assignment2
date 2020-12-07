#include "hoistlib.h"

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
