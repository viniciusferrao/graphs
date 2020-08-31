/*
 *
 * Types
 *
 */

#ifndef __TYPES_H__
#define __TYPES_H__

typedef enum datastructure {
	matrix,
	list
} datastructure;

/*
 * Tipo definido para implementação da lista encadeada para representação da
 * lista de adjacências.
 */
typedef struct graphAresta {
	unsigned vertice;
	struct graphAresta *next;
} typeAresta;

typedef struct node {
	unsigned destination;
	double weight;
	struct node *next;
} typeNode;

typedef struct _queue {
	unsigned data;
	struct _queue *link;
} queue;

typedef struct _stack {
	unsigned data;
	struct _stack *link;
} stack;

/* Priority Queue types */
/*typedef struct _heap {
	int nodeID;
	int distance;
} *heap;

typedef struct _priorityQueue {
    int capacity;
    int size;
    int *positions;
    heap *nodes;
} *priorityQueue;*/

#endif
