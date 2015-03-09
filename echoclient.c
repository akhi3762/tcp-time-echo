#include	"unp.h"
void Writen_my(int fd, void *ptr, size_t nbytes, int pfd);
ssize_t	writen1(int fd, const void *vptr, size_t n);

int main(int argc, char** argv) {
	char	sendline[MAXLINE], recvline[MAXLINE], pfdbuf[MAXLINE];
	int sockfd, pfd[2], maxfdp1, n, stdineof;
	fd_set rset;
	sockfd = atoi(argv[1]);
	pfd[1] = atoi(argv[2]);
	//stdineof = 0;	
	FD_ZERO(&rset);
	printf("Starting echo client. Enter an input...\n");
	strcpy(pfdbuf, "Echo client: started");
	write(pfd[1], pfdbuf, strlen(pfdbuf)+1);
	while (1) {
		FD_SET(fileno(stdin), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(stdin), sockfd) + 1;
		Select(maxfdp1, &rset, NULL, NULL, NULL);
		if(FD_ISSET(sockfd, &rset)) { /*socket is readable */
			if( Readline(sockfd, recvline, MAXLINE) == 0) {	
				strcpy(pfdbuf, "Echo Client: Server terminated prematurely.");
				write(pfd[1], pfdbuf, strlen(pfdbuf)+1);			
				exit(1);
			}
			
			printf("Echo from server:");
			//printf("read from server socket: %s\n", recvline);
			 if (fputs(recvline, stdout) > 0) {
				strcpy(pfdbuf, "Message echoed from server sucessfully");
				write(pfd[1], pfdbuf, strlen(pfdbuf)+1);
			}
			else  {	
			strcpy(pfdbuf, "Echo message from server failed to display");
			write(pfd[1], pfdbuf, strlen(pfdbuf)+1);
			}		
			//Write(fileno(stdout), recvline, n);
		}
		if(FD_ISSET(fileno(stdin), &rset)) { /*Input is readable */		
			if( (n = Read(fileno(stdin), sendline, MAXLINE)) == 0)
				return;
			Writen(sockfd, sendline, n);
   		}
	}
	printf("\n");
}

ssize_t						/* Write "n" bytes to a descriptor. */
writen1(int fd, const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;		/* and call write() again */
			else
				return(-1);			/* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}
/* end writen */

void Writen_my(int fd, void *ptr, size_t nbytes, int pfd)
{
	char pfdbuf[MAXLINE];
	//int pfd[2];cho 
	//pfd[1] = atoi(argv[2]);
	if (writen1(fd, ptr, nbytes) != nbytes) {
		strcpy(pfdbuf, "Echo client: Writen error");
			write(pfd, pfdbuf, strlen(pfdbuf)+1);
			exit(1);
	}
	return;
}
