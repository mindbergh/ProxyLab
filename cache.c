/*
 * cache.c - Cache functionalities for proxy server of CMU 15-213 proxy lab
 * Author: Ming Fang
 * Email:  mingf@andrew.cmu.edu
 */

 #include "cache.h"



/*
 * cmp - Compare two given cache node
 * return: 0 if not identical
 *         1 if identical
 */
/* $begin cmp */
int cmp(cnode_t *this, cnode_t *that) {
	if (strcasecmp(this->host, that->host) 
		return 0;
	if (this->port != that->port)
		return 0;
	if (strcasecmp(this->path, that->path))
		return 0;
	return 1;
}
/* $end cmp */


void delete(cnode_t *node) {

}

void enqueue(cnode_t *node, cnode_t * tail) {

}

cnode_t * dequeue(cnode_t * head) {
	
}

cnode_t * new(char *host, int port, char *path, char *payload) {
	cnode_t * res = Malloc(sizeof(cnode_t));
	strcpy(res->host, host);
	strcpy(res->path, path);
	res->port = port;
	res->payload = payload;
	return res;
}