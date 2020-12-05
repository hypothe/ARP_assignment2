#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>

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
  while (1)
  {
    bzero(buffer,20);
    if ((ret = read(sockfd, buffer, 20)) < 0)
      error("ERROR reading from socket", ret);
    printf("%s\n",buffer);
    if (strcmp(buffer, "END communication") == 0)
      break;
	}
  exit (0);
}
