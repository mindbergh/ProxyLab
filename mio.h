/* $begin mio.h */
#ifndef MIO_H
#define MIO_H

#include "csapp.h"


/* Persistent state for the robust I/O (Mio) package */
/* $begin mio_t */
#define MIO_BUFSIZE 8192
typedef struct {
    int mio_fd;                /* descriptor for this internal buf */
    int mio_cnt;               /* unread bytes in internal buf */
    char *mio_bufptr;          /* next unread byte in internal buf */
    char mio_buf[MIO_BUFSIZE]; /* internal buffer */
} mio_t;
/* $end mio_t */


/* Mio (Ming I/O) package */
ssize_t mio_readn(int fd, void *usrbuf, size_t n);
ssize_t mio_writen(int fd, void *usrbuf, size_t n);
void mio_readinitb(mio_t *rp, int fd); 
ssize_t	mio_readnb(mio_t *rp, void *usrbuf, size_t n);
ssize_t	mio_readlineb(mio_t *rp, void *usrbuf, size_t maxlen);

/* Wrappers for Mio package */
ssize_t Mio_readn(int fd, void *usrbuf, size_t n);
void Mio_writen(int fd, void *usrbuf, size_t n);
void Mio_readinitb(mio_t *rp, int fd); 
ssize_t Mio_readnb(mio_t *rp, void *usrbuf, size_t n);
ssize_t Mio_readlineb(mio_t *rp, void *usrbuf, size_t maxlen);

#endif /* MIO_H */
/* $end mio.h */