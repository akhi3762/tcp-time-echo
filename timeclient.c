#include	"unp.h"
#include 	<string.h>

int main(int argc, char** argv) {
	int sockfd, n, pfd[2];
	char recvline[MAXLINE + 1], pbuf[MAXLINE],pfdbuf[MAXLINE];
	sockfd = atoi(argv[1]);
	pfd[1] = atoi(argv[2]);
	//printf("pfd value: %d\n", pfd[1]);
	//printf("Time client ,sockfd:%d",sockfd);
	printf("Time client started:\n");	
	strcpy(pfdbuf, "Time client: started");
	write(pfd[1], pfdbuf, strlen(pfdbuf)+1);
	while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
		//recvline[n] = 0; /* null terminate */
		if (fputs(recvline, stdout) == EOF) {
			strcpy(pfdbuf, "Time client: Write error");
			write(pfd[1], pfdbuf, strlen(pfdbuf)+1);
			exit(1);
		}
		else {
		strcpy(pfdbuf, "Time client: Time successfully written to client");
			write(pfd[1], pfdbuf, strlen(pfdbuf)+1);
		}
	}
	if (n == 0) {
		strcpy(pfdbuf, "Time Client: server terminated prematurely");
		write(pfd[1], pfdbuf, strlen(pfdbuf)+1);
		exit(1);
	}
	if (n < 0) {
		strcpy(pfdbuf, "Time client: Read error");
		write(pfd[1], pfdbuf, strlen(pfdbuf)+1);
		exit(1);
	}
	exit(0);
}
