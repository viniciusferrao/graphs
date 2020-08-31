/*
 *
 * Header
 *
 */

#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

/* Funções para manipulação de bits */

inline void setbit(uint64_t *, unsigned char);

inline void clearbit(uint64_t *, unsigned char);

inline char getbit(uint64_t, char);

char *uint64_t2bin(uint64_t, char *, int);



/* Funções de manipulação de matriz */

uint64_t **createMatrix(unsigned, unsigned);  

void freeMatrix(uint64_t **, unsigned, unsigned);

void populateMatrix(uint64_t **, unsigned, unsigned);

bool getValueFromMatrix(uint64_t **, unsigned, unsigned);



/* Funções de manipulação do arquivo de entrada */

char validateGraphLineInput(char *, unsigned);

char validateWeightedGraphLineInput(char *, unsigned);

int getVertexFromFile(FILE *);

int loadAdjacencyMatrixFromFile(FILE *, uint64_t **);


/* Funções de criação e manipulação de matrizes com peso */

double **createWeightedMatrix(unsigned, unsigned);

int loadWeightedAdjacencyMatrixFromFile(FILE *, double **);



/* Funções de criação e manipulação de listas */

typeAresta *createList(unsigned);

void setAdjacency(typeAresta *, unsigned);

char duplicatesInList(typeAresta *, unsigned, unsigned);

int loadAdjacencyListFromFile(FILE *file, typeAresta *);



/* Calculo de graus */

unsigned *calculateGraphDegreeFromMatrix (uint64_t **, unsigned);

unsigned *calculateGraphDegreeFromList(typeAresta *, unsigned);



/* Funções de manipulação de filas */

queue *addItemInQueue(queue *, int);

queue *removeItemFromQueue(queue *);

void clearQueue(queue *);

int queueIsEmpty(queue *);

void printQueue(queue *);



/* Funções de manipulação de pilhas */

stack *stackPush(stack *, unsigned);

stack *stackPop(stack *);

stack *clearStack(stack *);

int stackIsEmpty(stack *);

void printStack(stack *);



/* Funções de busca */

void bfsFromMatrix(uint64_t **, unsigned, unsigned);

void dfsFromMatrix(uint64_t **, unsigned, unsigned);

void bfsFromList(typeAresta *, unsigned, unsigned);

void dfsFromList(typeAresta *, unsigned, unsigned);



/* Funções de busca em grafos com pesos */

unsigned dijkstra(FILE *, unsigned);



/* Funções de detecção de componentes conexos */

void connectedComponentsFromMatrix(uint64_t **, unsigned);

void connectedComponentsFromList(typeAresta *, unsigned);



/* Funções de saída */

void generateOutputFile(FILE *, unsigned, unsigned, unsigned *);

void printAdjacencyMatrix(unsigned, uint64_t **);

void printAdjacencyList(unsigned, typeAresta *);

#endif
