/*
 * cache.h - Cache functionalities for proxy server of CMU 15-213 proxy lab
 * Author: Ming Fang
 * Email:  mingf@andrew.cmu.edu
 */

#ifndef CACHE_H
#define CACHE_H

#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400




/* Cache node strcut */
typedef struct cnode {
	char *host;
	char *path;	
	char *payload;
	struct cnode *prev;
	struct cnode *next;
	size_t size;
	int port;
} cnode_t;

extern cnode_t *tail;
extern cnode_t *head;
extern int cache_count;
extern volatile size_t cache_load;
extern volatile int readcnt;            // Initially 0
extern sem_t mutex, w;         // Both initially 1;

/* Cache function prototypes */
int cmp(cnode_t *this, char *host, int port, char *path);
void cache_init();
void delete(cnode_t *node);
void enqueue(cnode_t *node);
void dequeue();
cnode_t * new(char *host, int port, char *path, char *payload, size_t size);
cnode_t * match(char *host, int port, char *path);
int cache_check();
void Cache_check();


#endif

 