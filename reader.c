#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>

typedef struct message{
	int height;
	char status;
} msg_t;

void error(char *msg, int ret)
{
    perror(msg);
    exit(ret); // error propagation
}

int main(int argc, char * argv[])
{
  int ret;
  int sockfd = atoi(argv[1]);
  char buffer[20]; //will be used for printing hoist status
  msg_t msg = {0,0};
  while (1)
  {
    bzero(buffer,20);
    if ((ret = read(sockfd, &msg, sizeof(msg))) < 0)
      error("ERROR reading from socket", ret);
    printf("height: %d\tstatus: %c\n", msg.height, msg.status);
    if (msg.status == 'E')
      break;
	}
  exit (0);
}
