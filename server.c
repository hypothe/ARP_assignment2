/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#define BUFSIZE 20

void error(char *msg, int ret)
{
    perror(msg);
    exit(ret); // error propagation
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen;
     //char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int ret;
	 char in=0;
	 char buffer[BUFSIZE];
	 
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
        error("ERROR opening socket", sockfd);
		
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if ((ret = bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr))) < 0) 
              error("ERROR on binding", ret);
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     
     if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0) 
          error("ERROR on accept", newsockfd);
     bzero(buffer,BUFSIZE);
	 while(in!='E'){
		if ((ret = read(newsockfd,&in,1)) < 0) error("ERROR reading from socket", ret);
		switch (in){
			case 'U':	 // go UP
				sprintf(buffer, "Sending UP");
				break;
			case 'S': 	// STOP
				sprintf(buffer, "Stopping");
				break;
			case 'D': 	// go DOWN
				sprintf(buffer, "Sending DOWN");
				break;
			case 'E':	// END
				sprintf(buffer, "END communication");
				break;
			// default 
		}
		if ((ret = write(newsockfd,buffer,BUFSIZE))<0) error("ERROR writing on socket", ret); 
	 }
     return 0; 
}
