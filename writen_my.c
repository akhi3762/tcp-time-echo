/* include writen */
#include	"unp.h"

ssize_t						/* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n)
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

void
Writen_my(int fd, void *ptr, size_t nbytes, int pfd)
{
	//int pfd[2];
	//pfd[1] = atoi(argv[2]);
	if (writen(fd, ptr, nbytes) != nbytes) {
		strcpy(pfdbuf, "Writen error");
			write(pfd[1], pfdbuf, strlen(pfdbuf)+1);
			exit(1);
	}
		//err_sys("writen error");
}
