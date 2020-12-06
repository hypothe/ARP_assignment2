/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/wait.h>

#define MAX_HEIGHT 200
#define MIN_HEIGHT 0
#define STEP 5
#define MAX_STEP MAX_HEIGHT/STEP
#define BUFSIZE 20


int spawn(char *ex_name, int *fd_in, int *fd_out, char new_shell);

typedef struct message{
	int height;
	char status;
} msg_t;

void error(char *msg, int ret)
{
    perror(msg);
    exit(ret); // error propagation
}

int main(int argc, char *argv[])
{
	char STATUS = 'B';
	int sockfd, newsockfd, portno, clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int ret;
	unsigned long sec = 1, usec = 0;
	char in=0;
	struct timeval tv;
	msg_t msg = {0,0};
	fd_set fd_in;
	int s2h[2], h2s[2]; // pipes for server - hoist communication
	pid_t hoist;
	char send=0;
	int r_height=0;
	char err_msg[256];

	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
		error("ERROR opening socket", sockfd);
		
	if((ret = pipe(s2h))<0){
		error("ERROR opening Server - Hoist pipe", ret);
	}	
	if((ret = pipe(h2s))<0){
		error("ERROR opening Hoist - Server pipe", ret);
	}
	/* s2h is the input pipe of the hoist, h2s is its output pipe */
	/* Spawn without opening a new shell, since hoist doesn't output anything */
	if((hoist = spawn("./hoist", s2h, h2s, 0))<0) error("ERROR hoist generation", hoist);

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if ((ret = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) < 0){ 
		error("ERROR on binding", ret);
		close(sockfd);
	}
	listen(sockfd,5);
	clilen = sizeof(cli_addr);

	if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0) 
		error("ERROR on accept", newsockfd);

	while(in!='E'){
		// --- preconfig ---
		FD_ZERO(&fd_in);
		FD_SET(newsockfd, &fd_in);
		tv.tv_sec = sec; tv.tv_usec = usec;
		in = 0;	// reset each time, in order to avoid problems if we somehow read 0Byte from the readable socket
		
		if((ret=select(newsockfd+1, &fd_in, NULL, NULL, &tv)) < 0){	error("Select on socket", ret); }
		/* Updates status only if a new one is submitted and it is compatible with current status, otherwise stop the hoist */
		if (FD_ISSET(newsockfd, &fd_in)){
			if ((ret = read(newsockfd,&in,1)) < 0) error("ERROR reading from socket", ret);
			// printf("Received request: ");	fflush(stdout);// won't print nicely if we read 0 bit
			if ((in == 'U' && STATUS != 'T') || (in == 'D' && STATUS != 'B') || (in == 'E') || (in == 'S'))
				STATUS = in;
		}
		
		switch (STATUS){
			case 'U':	 // go UP
				send = '+';
				break;
			case 'D': 	// go DOWN
				send = '-';
				break;
			case 'S': 	// STOP
				send = 0;
				break;
			case 'T':
				send = 0;
				break;
			case 'B':
				send = 0;
				break;
			case 'E':	// END
				send = 'E';
				break;
		}
		if ((ret = write(s2h[1],&send,1))<0) 					error("ERROR writing on Server - Hoist pipe", ret);
		
		if ((ret = read(h2s[0],&r_height,sizeof(r_height)))<0) error("ERROR reading from Hoist - Server pipe", ret);
		
		if (r_height>=MAX_HEIGHT && STATUS != 'E') STATUS = 'T';	// set TOP or BOTTOM state
		if (r_height<=MIN_HEIGHT && STATUS != 'E') STATUS = 'B';	// the condition allows to take the more strict height limit if
																	// the Server and Hoist one are different (this is not the case)
		if (ret == 0) STATUS = 'E';	//If the pipe results readable but nothing is read it's possible the hoist crashed								
		msg.height = r_height; msg.status = STATUS;
		
		if ((ret = write(newsockfd,&msg,sizeof(msg)))<0) 		error("ERROR writing on socket", ret);
		select(1, NULL, NULL, NULL, &tv); // simply used as a timer
		//usleep(1000000*(tv.tv_sec)+(tv.tv_usec));			// tv will yield the amount of time passed before reading
	}														// with this usleep we approximate a period of 1s for the serve
	
    waitpid(hoist, &ret, 0);
	if (!WIFEXITED(ret)){
		sprintf(err_msg, "Hoist terminated with an error %d %d\n", WIFSIGNALED(ret), WTERMSIG(ret));
		perror(err_msg);
	}
	else
		printf("Hoist exited with value %d\n", WEXITSTATUS(ret)); fflush(stdout);
	
	close(s2h[1]);
	close(h2s[0]);
	close(newsockfd);
	return 0; 
}

int spawn(char* ex_name, int *fd_in, int *fd_out, char new_shell) {
	int ret;
	pid_t child_pid = fork();
	char *args[6];
	int i = 0;
	if (child_pid != 0)
	{	
		/* Father closes fd_in[0], since it will write on it */
		/* Father closes fd_out[1], since it will read from it */
		if (fd_in!=NULL) 	close(fd_in[0]);
		if (fd_out!=NULL)	close(fd_out[1]);
		return child_pid;
	}
	else 
	{
		/* Child closes fd_in[1], since it will read from it */
		/* Father closes fd_out[0], since it will write on it */
		if (fd_in!=NULL)	close(fd_in[1]);
		if (fd_out!=NULL)	close(fd_out[0]);
		char tmp_in[5]="", tmp_out[5]="";
		sprintf(tmp_in, "%d", fd_in[0]);
		sprintf(tmp_out, "%d", fd_out[1]);
		if (new_shell){ 
			args[i++] = "/usr/bin/konsole";
			args[i++] = "-e";
		}
		args[i++] = ex_name;
		args[i++] = tmp_in;
		args[i++] = tmp_out;
		args[i++] = (char*)NULL;
		
		ret = execvp(args[0], args);
		perror("exec failed");
		exit(ret);
	}
}