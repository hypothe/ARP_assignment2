/* A simple server in the internet domain using TCP
   The port number is passed as an argument */

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/wait.h>
#include "hoistlib.h"

int main(int argc, char *argv[])
{
	const char *NAME = "SERVER";
	char STATUS = BOTTOM;
	int sockfd, newsockfd, portno, clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int ret;
	unsigned long sec = 1, usec = 0;
	char in=0;
	struct timeval tv;
	msg_t msg;
	int msg_size;
	fd_set fd_in;
	int s2h[2], h2s[2]; // pipes for server - hoist communication
	pid_t hoist;
	char send=0;
	int r_height=0;
	char err_msg[256];

	FILE *log;
	char fd_log_str[20];
	int  fd_log;
	char *log_file = "./server_log.txt";

	if((log = fopen(log_file, "w"))==(FILE*)NULL){	// See NOTE_2
		perror("Log file open from");
		exit(1);
	}
	fd_log = fileno(log);

	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	fprintf(log, "%s: starting\n", NAME); fflush(log);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("ERROR opening socket", sockfd);
	fprintf(log, "%s: socket opened\n", NAME); fflush(log);

	if((ret = pipe(s2h))<0){
		error("ERROR opening Server - Hoist pipe", ret);
	}
	if((ret = pipe(h2s))<0){
		error("ERROR opening Hoist - Server pipe", ret);
	}
	fprintf(log, "%s: pipes opened\n", NAME); fflush(log);
	/* s2h is the input pipe of the hoist, h2s is its output pipe */
	/* Spawn without opening a new shell, since hoist doesn't output anything */
	if((hoist = spawn("./hoist", fd_log, s2h, h2s, 0))<0) error("ERROR hoist generation", hoist);
	close(s2h[0]);
	close(h2s[1]);
	fprintf(log, "%s: hoist spawned\n", NAME); fflush(log);

	msg_init(&msg); msg_size = msg_getsize(); // initialize message pointed to by msg

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
	fprintf(log, "%s: listening to socket\n", NAME); fflush(log);

	if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0)
		error("ERROR on accept", newsockfd);
	fprintf(log, "%s: communication started\n", NAME); fflush(log);

	while(in!=EXIT){
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
			if ((in == UP && STATUS != TOP) || (in == DOWN && STATUS != BOTTOM) || (in == EXIT) || (in == STOP))
				STATUS = in;
		}

		switch (STATUS){
			case UP:	 // go UP
				send = '+';
				break;
			case DOWN: 	// go DOWN
				send = '-';
				break;
			case STOP: 	// STOP
				send = 0;
				break;
			case TOP:
				send = 0;
				break;
			case BOTTOM:
				send = 0;
				break;
			case EXIT:	// END
				send = EXIT;
				break;
		}
		if ((ret = write(s2h[1],&send,1))<0) 					error("ERROR writing on Server - Hoist pipe", ret);

		if ((ret = read(h2s[0],&r_height,sizeof(r_height)))<0) 	error("ERROR reading from Hoist - Server pipe", ret);

		if (r_height>=MAX_HEIGHT && STATUS != EXIT) STATUS = TOP;	// set TOP or BOTTOM state
		if (r_height<=MIN_HEIGHT && STATUS != EXIT) STATUS = BOTTOM;	// the condition allows to take the more strict height limit if
																	// the Server and Hoist one are different (this is not the case)
		if (ret == 0) STATUS = EXIT;	//If the pipe results readable but nothing is read it's possible the hoist crashed
		//msg.height = r_height; msg.status = STATUS;
		msg_setheight(msg, r_height); msg_setstatus(msg, STATUS);
		if ((ret = write(newsockfd,msg,msg_size))<0) 			error("ERROR writing on socket", ret);
		select(1, NULL, NULL, NULL, &tv); // simply used as a timer
		//usleep(1000000*(tv.tv_sec)+(tv.tv_usec));			// tv will yield the amount of time passed before reading
	}														// with this usleep we approximate a period of 1s for the serve
	fprintf(log, "%s: communication terminated\n", NAME); fflush(log);

    waitpid(hoist, &ret, 0);
	if (!WIFEXITED(ret)){
		sprintf(err_msg, "Hoist terminated with an error %d %d\n", WIFSIGNALED(ret), WTERMSIG(ret));
		perror(err_msg);
	}
	else
		printf("Hoist exited with value %d\n", WEXITSTATUS(ret)); fflush(stdout);

	fprintf(log, "%s: exiting\n", NAME); fflush(log);
	msg_free(msg);
	close(s2h[1]);
	close(h2s[0]);
	close(fd_log);
	close(newsockfd);
	return 0;
}
