/* $begin mio.c */
#include "mio.h"




/*********************************************************************
 * The Mio package - ming I/O functions
 **********************************************************************/
/*
 * mio_readn - read n bytes (unbuffered)
 */
/* $begin mio_readn */
ssize_t mio_readn(int fd, void *usrbuf, size_t n) 
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = usrbuf;

    while (nleft > 0) {
	if ((nread = read(fd, bufp, nleft)) < 0) {
	    if (errno == EINTR) /* interrupted by sig handler return */
			nread = 0;      /* and call read() again */
		else if (errno == ECONNRESET) {
			fprintf(stderr, "ECONNRESET handled\n");
			return 0;
		} else
			return -1;      /* errno set by read() */ 
	} 
	else if (nread == 0)
	    break;              /* EOF */
	nleft -= nread;
	bufp += nread;
    }
    return (n - nleft);         /* return >= 0 */
}
/* $end mio_readn */

/*
 * mio_writen -  write n bytes (unbuffered)
 */
/* $begin mio_writen */
ssize_t mio_writen(int fd, void *usrbuf, size_t n) 
{
    size_t nleft = n;
    ssize_t nwritten;
    char *bufp = usrbuf;

    while (nleft > 0) {
	if ((nwritten = write(fd, bufp, nleft)) <= 0) {
	    if (errno == EINTR)  /* interrupted by sig handler return */
			nwritten = 0;    /* and call write() again */
		else if (errno == EPIPE) {
			fprintf(stderr, "EPIPE handled\n");
			return n;
		} else
			return -1;       /* errorno set by write() */
	}
	nleft -= nwritten;
	bufp += nwritten;
    }
    return n;
}
/* $end mio_writen */


/* 
 * mio_read - This is a wrapper for the Unix read() function that
 *    transfers min(n, mio_cnt) bytes from an internal buffer to a user
 *    buffer, where n is the number of bytes requested by the user and
 *    mio_cnt is the number of unread bytes in the internal buffer. On
 *    entry, mio_read() refills the internal buffer via a call to
 *    read() if the internal buffer is empty.
 */
/* $begin mio_read */
static ssize_t mio_read(mio_t *rp, char *usrbuf, size_t n)
{
    int cnt;

    while (rp->mio_cnt <= 0) {  /* refill if buf is empty */
	rp->mio_cnt = read(rp->mio_fd, rp->mio_buf, 
			   sizeof(rp->mio_buf));
	if (rp->mio_cnt < 0) {
		if (errno == ECONNRESET) {
			fprintf(stderr, "ECONNRESET handled\n");
			return 0;
		} else if (errno != EINTR) /* interrupted by sig handler return */
			return -1;
	} else if (rp->mio_cnt == 0)  /* EOF */
	    return 0;
	else 
	    rp->mio_bufptr = rp->mio_buf; /* reset buffer ptr */
    }

    /* Copy min(n, rp->mio_cnt) bytes from internal buf to user buf */
    cnt = n;          
    if (rp->mio_cnt < n)   
		cnt = rp->mio_cnt;
    memcpy(usrbuf, rp->mio_bufptr, cnt);
    rp->mio_bufptr += cnt;
    rp->mio_cnt -= cnt;
    return cnt;
}
/* $end mio_read */

/*
 * mio_readinitb - Associate a descriptor with a read buffer and reset buffer
 */
/* $begin mio_readinitb */
void mio_readinitb(mio_t *rp, int fd) 
{
    rp->mio_fd = fd;  
    rp->mio_cnt = 0;  
    rp->mio_bufptr = rp->mio_buf;
}
/* $end mio_readinitb */

/*
 * mio_readnb - Robustly read n bytes (buffered)
 */
/* $begin mio_readnb */
ssize_t mio_readnb(mio_t *rp, void *usrbuf, size_t n) 
{
    size_t nleft = n;
    ssize_t nread;
    char *bufp = usrbuf;
    
    while (nleft > 0) {
		if ((nread = mio_read(rp, bufp, nleft)) < 0) {
	    	if (errno == EINTR) /* interrupted by sig handler return */
				nread = 0;      /* call read() again */
	    	else
				return -1;      /* errno set by read() */ 
		} else if (nread == 0)
	    	break;              /* EOF */
		nleft -= nread;
		bufp += nread;
    }
    return (n - nleft);         /* return >= 0 */
}
/* $end mio_readnb */

/* 
 * mio_readlineb - mingly read a text line (buffered)
 */
/* $begin mio_readlineb */
ssize_t mio_readlineb(mio_t *rp, void *usrbuf, size_t maxlen) 
{
    int n, rc;
    char c, *bufp = usrbuf;

    for (n = 1; n < maxlen; n++) { 
	if ((rc = mio_read(rp, &c, 1)) == 1) {
	    *bufp++ = c;
	    if (c == '\n')
		break;
	} else if (rc == 0) {
	    if (n == 1)
			return 0; /* EOF, no data read */
	    else
			break;    /* EOF, some data was read */
	} else
	    return -1;	  /* error */
    }
    *bufp = 0;
    return n;
}
/* $end mio_readlineb */

/**********************************
 * Wrappers for ming I/O routines
 **********************************/
ssize_t Mio_readn(int fd, void *ptr, size_t nbytes) 
{
    ssize_t n;
  
    if ((n = mio_readn(fd, ptr, nbytes)) < 0)
	unix_error("Mio_readn error");
    return n;
}

void Mio_writen(int fd, void *usrbuf, size_t n) 
{
    if (mio_writen(fd, usrbuf, n) != n)
	unix_error("Mio_writen error");
}

void Mio_readinitb(mio_t *rp, int fd)
{
    mio_readinitb(rp, fd);
} 

ssize_t Mio_readnb(mio_t *rp, void *usrbuf, size_t n) 
{
    ssize_t rc;

    if ((rc = mio_readnb(rp, usrbuf, n)) < 0)
	unix_error("Mio_readnb error");
    return rc;
}

ssize_t Mio_readlineb(mio_t *rp, void *usrbuf, size_t maxlen) 
{
    ssize_t rc;

    if ((rc = mio_readlineb(rp, usrbuf, maxlen)) < 0) {
    	printf("Fd = %d\n", rp->mio_fd);
		unix_error("Mio_readlineb error");
	}
    return rc;
} 