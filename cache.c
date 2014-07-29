/*
 * cache.c - Cache functionalities for proxy server of CMU 15-213 proxy lab
 * Author: Ming Fang
 * Email:  mingf@andrew.cmu.edu
 */


/*    prev <- Node -> next
 *    head = least recently used
 *    tail = most recently used
 *    prev one is less recent one
 *    next one is more recent one
 */


 #include "cache.h"


cnode_t *tail;
cnode_t *head;
volatile size_t cache_load;
volatile int readcnt;            // Initially 0
sem_t mutex, w;         // Both initially 1;

/*
 * cmp - Compare the given cache node
 * return: 0 if not identical
 *         1 if identical
 */
/* $begin cmp */
int cmp(cnode_t *this, char *host, int port, char *path) {
	if (strcasecmp(this->host, host))
		return 0;
	if (this->port != port)
		return 0;
	if (strcasecmp(this->path, path))
		return 0;
	return 1;
}
/* $end cmp */


/*
 * cache_init - Initial head and tail pointer
 */
/* $begin cache_init */
void cache_init() {
	tail = NULL;
	head = NULL;
	cache_load = 0;
	readcnt = 0;
	Sem_init(&mutex, 0 , 1);
	Sem_init(&w, 0, 1);
}
/* $end cache_init */

/*
 * delete - Delete the given cache node from the list
 */
/* $begin delete */
void delete(cnode_t *node) {
	if (head == tail) {
		head = NULL;
		tail = NULL;
	} else if (node->prev == NULL) {
		head = node->next;
		(node->next)->prev = NULL;
	} else if (node->next == NULL) {
		tail = node->prev;
		(node->prev)->next = NULL;
	} else {
		(node->prev)->next = node->next;
		(node->next)->prev = node->prev;
	}
	cache_load -= node->size;
}
/* $end delete */

/*
 * enqueue - Enqueue the given cache node
 */
/* $begin enqueue */
void enqueue(cnode_t *node) {
	if (head == NULL && tail == NULL) {
		head = node;
		tail = node;
		node->next = NULL;
		node->prev = NULL;
	} else {
		tail->next = node;
		node->prev = tail;
		node->next = NULL;
		tail = node;
	}
	cache_load += node->size;
}
/* $end enqueue */

/*
 * dequeue - Dequeue the given cache node
 */
/* $begin dequeue */
void dequeue() {
	cnode_t * res;
	if (head == NULL)
		return;
	else if (head == tail) {
		res = head;
		head = NULL;
		tail = NULL;
	} else {
		res = head;
		(head->next)->prev = NULL;
		head = head->next;
	}
	cache_load -= res->size;
	Free(res->host);
	Free(res->path);
	Free(res->payload);
	Free(res);
}
/* $end dequeue */

/*
 * new - New node constructor
 * return: the ptr to the node constructed
 */
/* $begin new */
cnode_t * new(char *host, int port, char *path, char *payload, size_t size) {
	cnode_t * res = Malloc(sizeof(cnode_t));
	res->host = Malloc(strlen(host));
	strcpy(res->host, host);
	res->path = Malloc(strlen(path));
	strcpy(res->path, path);
	res->port = port;
	res->payload = Malloc(strlen(payload));
	strcpy(res->payload, payload);
	res->size = size;
	return res;
}
/* $end new */

/*
 * match - Try to match a node in the cache
 * return: the node matched
 *         NULL on no matching
 */
/* $begin match */
cnode_t * match(char *host, int port, char *path) {
	cnode_t * res = tail;
	for (; res != NULL; res = res->prev) {
		if (cmp(res, host, port, path)) {
			return res;
		}
	}
	return NULL;
}
/* $end match */