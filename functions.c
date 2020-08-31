/*
 * Arquivo principal de funções para biblioteca de grafos.
 *
 * TODO: 
 * 0. Free na porra toda!
 * 1. Passagem por referência nas funções de Stack e Queue.
 * 2. Reavaliar getbit();
 * 3. Mapeamento de memória em createMatrix();
 * 4. Acabar com a necessidade do tipo bool e da stdbool.h
 * 5. Underflow nos níveis da BFS
 * 6. Colocar as macros de _DEBUG_ na DFS
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
/* Remover depois */
#include <time.h>
#include <limits.h>

#include "const.h"
#include "errors.h"
#include "types.h"
#include "functions.h"

/* 
 * Funções de gerenciamento de bits
 */
extern inline void setbit(uint64_t *inteiro, unsigned char pos) {
	*(uint64_t*)inteiro |= (uint64_t)1 << pos;
}

extern inline void clearbit(uint64_t *inteiro, unsigned char pos) {
	*(uint64_t*)inteiro &= ~((uint64_t)1 << pos);
}

extern inline char getbit(uint64_t inteiro, char pos) {
	return (char) ((inteiro >> pos) & 1);
}

/*
 * Função para imprimir na tela um uint64_t de forma binária, utilizada com
 * finalizade de debugging durante o desenvolvimento do programa.
 */
char *uint64_t2bin(uint64_t a, char *buffer, int buf_size) {
	int i;
	buffer += (buf_size - 1);

	for (i = BITS - 1 ; i >= 0 ; i--) {
		*buffer-- = (a & 1) + '0';
		a >>= 1;
	}
	
	return buffer;
}

/*
 * Função para alocar uma matriz de forma entendível humanamente; basta solici-
 * tar o tamanho necessário que o padding e a divisão de bits é feita automáti-
 * camente.
 *
 * Exemplo: createMatrix(72,72);
 * Será criada uma Matrix de [0-71][0-1].
 */
uint64_t **createMatrix(unsigned size_x, unsigned size_y) {
	unsigned short bits = BITS;
	uint64_t **matrix;
	unsigned i;
	
	/* Ainda podemos otimizar mais um pouco essa alocação de memória */
	//size_y--;

#ifdef _DEBUG_
	printf("Solicitado x*y = %i,%i\n", size_x, size_y);
	printf("Alocado na memória: %i*%i\n", size_x, size_y / bits);
#endif

	matrix = (uint64_t**) malloc(size_x * sizeof(uint64_t*));
		for (i = 0; i < size_x; i++)
			/* Usando calloc para já inicializar as posições da matriz */
			matrix[i] = (uint64_t*) calloc((size_y / bits), sizeof(uint64_t));
	
	return matrix;
}

/*	
 * Função para liberar a memória da matriz alocada. A implementação está bem ruim
 * visto que o ideal seria um tipo de função que não necessitasse do tamanho da
 * matriz para fazer a liberação de memória.
 *
 * Detalhe que int y não é utilizado hora alguma.
 */
void freeMatrix(uint64_t **matrix, unsigned x, unsigned y) {
	unsigned i;
	
	for (i = 0 ; i < x ; i++) {
		free(matrix[i]);
	}
	free(matrix);

}

/*
 * Função para se popular uma matriz utilizando uma forma humana de numeração.
 * Para que a funções funcione corretamente as dimensões são reduzidas de 1
 * com a finalidade de corrigir a questão do offset em 0.
 */ 
void populateMatrix(uint64_t **matrix, unsigned x, unsigned y) {
	unsigned short bits = BITS;
	x--;
	y--;
	
#ifdef _DEBUG_
	printf("Posição x,y = %i,%i\n", x + 1, y + 1);
	printf("Valor passado para setbit(); = %i,%i,%i\n", x, y / bits, y % bits);
#endif
	
	setbit(&matrix[x][y / bits], y % bits);
}

/*
 * Função para pegar o valor da posição i, j, de uma matriz utilizando a forma 
 * humana de representação.
 */
bool getValueFromMatrix(uint64_t **matrix, unsigned x, unsigned y) {
	unsigned short bits = BITS;
	x--;
	y--;
	
#ifdef _DEBUG_
	printf("Posição x,y = %i,%i\n", x + 1, y + 1);
	printf("Valor passado para getbit(); = %i,%i,%i\n", x, y / bits, y % bits);
#endif
	
	return getbit(matrix[x][y / bits], y % bits);
}

/*
 * Esta função confere se uma linha contem informação válida para alimentar um
 * dado grafo. Ela procura por caracteres diferentes de digitos e espaços para
 * invalidar a linha caso seja encontrado algum caracter não válido. 
 *
 * Não há tratamento de erro caso existam informações desnecessárias por linha
 *
 * Entrada: string, tamanho da string.
 * Retorno: 0
 */
char validateGraphLineInput(char *line, unsigned lineLength) {
	unsigned i;

#ifdef _DEBUG_	
	printf("Validando a linha: %s", line);
	if (lineLength == 1)
			printf("Linha vazia encontrada\n");
#endif
	
	for (i = 0 ; i < lineLength - 1 ; i++) {
		if (!isdigit(line[i]) && (line[i] != ' ')) {
			fprintf(stderr, "Caractere inválido %c na posição %d\n", 
			        line[i], i);
			return INVALID_GRAPH_FILE;
		}
	}
	
	return OK;
}

/*
 * Esta função confere se uma linha contem informação válida para alimentar um
 * dado grafo com pesos. Ela procura por caracteres diferentes de digitos, 
 * espaços e pontos para invalidar a linha caso seja encontrado algum caracter 
 * não válido. 
 *
 * Não há tratamento de erro caso existam informações desnecessárias por linha
 *
 * Entrada: string, tamanho da string.
 * Retorno: 0
 */
char validateWeightedGraphLineInput(char *line, unsigned lineLength) {
	unsigned i;

#ifdef _DEBUG_	
	printf("Validando a linha: %s", line);
	if (lineLength == 1)
			printf("Linha vazia encontrada\n");
#endif
	
	for (i = 0 ; i < lineLength - 1 ; i++) {
		if (!isdigit(line[i]) && (line[i] != ' ') 
		    && (line[i] != '.') && (line[i] != '-')) {
			fprintf(stderr, "Caractere inválido %c na posição %d\n", 
			        line[i], i);
			return INVALID_GRAPH_FILE;
		}
	}
	
	return OK;
}

/*
 * Esta função retorna a quantidade de vértices do arquivo ou um valor negativo
 * em caso de erro na validação do arquivo de leitura. O ponteiro do arquivo é
 * mantido no ponto imediato após a leitura dos vertíces, não há volta para o
 * início do arquivo.
 */
int getVertexFromFile(FILE *file) {
	char *line = NULL;
	size_t linecap = 0;
	ssize_t lineLength;
	
	lineLength = getline(&line, &linecap, file);
	
	if (validateGraphLineInput(line, lineLength))
		return (INVALID_GRAPH_FILE);
	
	return atoi(line);
}

/*
 * Esta função retorna a quantidade de arestas do grafo ou um valor negativo em 
 * caso de erro na validação do arquivo de leitura. O ponteiro do arquivo é man-
 * tido no ultimo ponto de leitura. A estrtura de dados utilizada é uma matriz
 * de adjacencia utilizando manipulação bit a bit para melhor aprovietamento de
 * memória.
 */
int loadAdjacencyMatrixFromFile(FILE *file, uint64_t **matrix) {
	unsigned i,j;
	unsigned arestas = 0;
	
	char *line = NULL;
	size_t linecap = 0;
	ssize_t lineLength;
	char *delimiter;

	while ((lineLength = getline(&line, &linecap, file)) > 0) {
		
		if (validateGraphLineInput(line, lineLength))
			return (INVALID_GRAPH_FILE);
		
		i = strtol(line, &delimiter, 10);
		j = strtol(delimiter, NULL, 10);
		if ((i != 0) && (j != 0)) {
			populateMatrix(matrix, i, j);
			populateMatrix(matrix, j, i);
			arestas++;

#ifdef _DEBUG_
			printf("Guardei %u e %u\n", i, j);
			printf("Passei aqui pela %u vez!\n", arestas);
#endif	

		}	
	}
	return arestas;
}

/*
 * Função que aloca matriz com suporte a pesos. Não foi otimizada, nem ouve
 * testes.
 */
double **createWeightedMatrix(unsigned size_x, unsigned size_y) {
	double **matrix;
	unsigned i;
	
#ifdef _DEBUG_
	printf("Solicitado x*y = %i,%i\n", size_x, size_y);
	printf("Alocado na memória: %i*%i\n", size_x, size_y);
#endif

	matrix = (double**) malloc(size_x * sizeof(double*));
		for (i = 0; i < size_x; i++)
			/* Usando calloc para já inicializar as posições da matriz */
			matrix[i] = (double*) calloc(size_y, sizeof(double));
	
	return matrix;
}

/*
 * Função que carrega uma matriz de adjacências de um arquivo com pesos. Os
 * pesos são guardados em uma matriz do tipo double. 
 */
int loadWeightedAdjacencyMatrixFromFile(FILE *file, double **matrix) {
	unsigned i,j;
	double k;
	unsigned arestas = 0;
	
	char *line = NULL;
	size_t linecap = 0;
	ssize_t lineLength;
	char *delimiter;

	while ((lineLength = getline(&line, &linecap, file)) > 0) {
		
		if (validateWeightedGraphLineInput(line, lineLength))
			return (INVALID_GRAPH_FILE);
		
		i = strtol(line, &delimiter, 10);
		j = strtol(delimiter, &delimiter, 10);
		k = strtod(delimiter, NULL);
		if ((i != 0) && (j != 0)) {
			matrix[i-1][j-1] = k;
			matrix[j-1][i-1] = k;
			arestas++;

#ifdef _DEBUG_
			printf("Guardei %0.2f na posicao %u,%u\n", k, i, j);
			printf("Passei aqui pela %u vez!\n\n", arestas);
#endif	

		}	
	}
	return arestas;
}

/* BUG FIX PHASE (Usar posição de memória zero automaticamente)
 * Esta função auxiliar cria um vetor de listas encadeadas e inicializa cada
 * umas das listas com a finalidade de servir como estrutura de dados em forma-
 * to de lista de adjacências.
 */
typeAresta *createList(unsigned vertices) {
	typeAresta *graph;
	unsigned i;
	
	graph = (typeAresta*) malloc(vertices * sizeof(typeAresta));
	
	for (i = 0 ; i <= vertices ; i++) {
		graph[i].vertice = i;
		graph[i].next = NULL;

#ifdef _DEBUG_
		printf("Inicializei graph[%u]\n", i);
#endif
		
	}
	return graph;
}

/* BUG FIX PHASE (Memory Management)
 * Esta função auxiliar coloca uma nova adjacência em uma lista encadeada de
 * adjacências. Não há tratamento de erro caso ocorra negação memória. Isso deve
 * ser implementado ainda.
 */
void setAdjacency(typeAresta *node, unsigned adjacency) {
	typeAresta *newAdjacency;
	
	newAdjacency = (typeAresta *) malloc(sizeof(typeAresta));
	newAdjacency -> vertice = adjacency;
	newAdjacency -> next = NULL;

	while (node -> next != NULL)
		node = node -> next;
	
	node -> next = newAdjacency;
}

/*
 * Função que verifica se existe um node duplicado em uma dada lista de adjacências
 * Retorna 0 caso não seja nada ecnotrado e 1 se sim.
 */
char duplicatesInList(typeAresta *graph, unsigned vertex1, unsigned vertex2) {
	typeAresta *temp;
	temp = &graph[vertex1];
	
	while(temp -> next != NULL) {
		if ((temp -> next -> vertice) == vertex2)
			return 1;
		temp = temp -> next;
	}
	
	return 0;
}

/*
 * Esta função retorna a quantidade de arestas do grafo ou um valor negativo em 
 * caso de erro na validação do arquivo de leitura. O ponteiro do arquivo é man-
 * tido no ultimo ponto de leitura. A estrutura de dados utilizada é um array de
 * listas encadeadas para resultar na lista de adjacências.
 */
int loadAdjacencyListFromFile(FILE *file, typeAresta *graph) {
	unsigned i,j;
	unsigned arestas;
	
	char *line = NULL;
	size_t linecap = 0;
	ssize_t lineLength;
	char *delimiter;

	arestas = 0;
	while ((lineLength = getline(&line, &linecap, file)) > 0) {
		
		if (validateGraphLineInput(line, lineLength))
			return (INVALID_GRAPH_FILE);
		
		i = strtol(line, &delimiter, 10);
		j = strtol(delimiter, NULL, 10);
		
		if (!(duplicatesInList(graph, i, j)) || !(duplicatesInList(graph, j, i))) {
			if ((i != 0) && (j != 0)) {
				setAdjacency(&graph[i], j);
				setAdjacency(&graph[j], i);
				arestas++;
#ifdef _DEBUG_
				printf("Guardei %u e %u\n", i, j);
				printf("Passei aqui pela %u vez!\n", arestas);
#endif	
			}
		} else {
			fprintf(stderr, "WARNING: Duplicada encontrada no arquivo vertíces: %u <-> %u\n", i, j);
		}
	}	
	
	return arestas;
	
}

/* BUG FIX PHASE (Memory Management)
 * Está função retorna um vetor de graus de um dado grafo representado por uma
 * matriz de adjacências.
 */
unsigned *calculateGraphDegreeFromMatrix(uint64_t **matrix, unsigned vertexes) {
	unsigned i,j;
	unsigned degree;
	unsigned *listOfDegrees;
	
	listOfDegrees = (unsigned *) calloc(vertexes + 1, sizeof(unsigned));
	
	for (i = 1 ; i <= vertexes ; i++) {
		degree = 0;
		for (j = 1 ; j <= vertexes ; j++) {
			if (getValueFromMatrix(matrix, i, j))
				degree++;
		}
		listOfDegrees[degree]++;
	}
	
	return listOfDegrees;
}

/* BUG FIX PHASE (Memory Management)
 * Esta função retorna um vetor de graus de um dado grafo representado por uma
 * lista encadeada de adjacências.
 */
unsigned *calculateGraphDegreeFromList(typeAresta *list, unsigned vertexes) {
	unsigned i;
	unsigned degree;
	unsigned *listOfDegrees;
	typeAresta *temp;
	
	listOfDegrees = (unsigned *) calloc(vertexes, sizeof(unsigned));
	
	for (i = 0 ; i < vertexes ; i++) {
		degree = 0;
		temp = &list[i];
		while(temp -> next != NULL) {
			temp = temp -> next;
			degree++;
		}
		
#ifdef _DEBUG_
		printf("Grau do vértice %u, encontrado: %u\n", i, degree);
#endif
		
		listOfDegrees[degree]++;
	}
	
	return listOfDegrees;
}

/*
 * Função para adicinoar um item em uma dada fila
 */
queue *addItemInQueue (queue *queuePointer, int data) {
	queue *lp = queuePointer;

	if (queuePointer != NULL) {
		while (queuePointer -> link != NULL)
	    	queuePointer = queuePointer -> link;
		
		queuePointer -> link = (queue *) malloc(sizeof(queue));
		queuePointer = queuePointer -> link;
		queuePointer -> link = NULL;
		queuePointer -> data = data;
		
		return lp;
		
	} else {
		queuePointer = (queue *) malloc(sizeof(queue));
		queuePointer -> link = NULL;
		queuePointer -> data = data;
		
		return queuePointer;
		
    }
}

/*
 * Função para remover um item em uma dada fila
 */
queue *removeItemFromQueue (queue *queuePointer) {
	queue *temp;

	temp = queuePointer -> link;
	free (queuePointer);

	return temp;
}

/*
 * Função para limpar por completo uma dada fila
 */
void clearQueue (queue *queuePointer) {
	while (queuePointer != NULL)
		queuePointer = removeItemFromQueue(queuePointer);
}

/*
 * Função para verificar se uma fila está vazia
 */
int queueIsEmpty(queue *queuePointer) {
	return (queuePointer == NULL ? 1:0);
}

/*
 * Função para imprimir na tela uma fila
 */
void printQueue (queue *queuePointer) {
	if (queuePointer == NULL)
		printf("The queue is empty!\n");
	else {
		printf("Queue: ");
		while (queuePointer != NULL) {
			printf("%d ", queuePointer -> data);
			queuePointer = queuePointer -> link;
		}
	}
	printf("\n");
}

/*
 * Stack
 */

stack *stackPush(stack *head, unsigned data) {
	stack *tmp = (stack *) malloc(sizeof(stack));

	if (tmp == NULL) {
		printf("Memory fail\n"); exit(0);
	}
	tmp -> data = data;
	tmp -> link = head;
	head = tmp;
	
	return head;
}

stack *stackPop(stack *head) {
	stack *tmp = head;
	
	if (head == NULL)
		return NULL;

	head = head -> link;
	free(tmp);
	
	return head;
}

stack *clearStack (stack *head) {
	while (head != NULL)
		head = stackPop(head);

	return head;
}

int stackIsEmpty(stack *head) {
	return (head == NULL ? 1:0);
}

void printStack(stack *head) {
	stack *current;
	current = head;
	
	if (current != NULL) {
		printf("Stack: ");
		do {
			printf("%d ",current -> data);
			current = current -> link;
        }
		while (current != NULL);
			printf("\n");
	} else {
        printf("The stack is empty\n");
    }
}

/* IMPLEMENTATION | BUG-FIX (Error handling and memory checks)
 * Função para execução de busca em largura num grafo em matriz. Root não pode
 * nunca ser maior que vertexes; se não a pesquisa falha. Falta documentação.
 */
void bfsFromMatrix(uint64_t **matrix, unsigned vertexes, unsigned root) {
	unsigned *markedItems;
	unsigned *pai;
    queue *queue;
	unsigned tempVertex;
	unsigned i;

#ifdef _DEBUG_
	printf("\n\nBFS: Start\n");
#endif

	/* Alocar um vértice a mais, visto que a posição 0 não é utilizada */
	markedItems = (unsigned *) calloc(vertexes + 1, sizeof(unsigned));
	
	/* Não preciso inicializar esse vetor */
	pai = (unsigned *) malloc((vertexes + 1) * sizeof(unsigned));
	
	queue = NULL;
	
	queue = addItemInQueue(queue, root);
	markedItems[root] = 1;

#ifdef _DEBUG_
	printf("BFS: Starting from vertex: %u\n", root);
	printf("BFS: Marquei o vértice raiz: %u\n", root);
#endif
	
	while (!queueIsEmpty(queue)) {
		tempVertex = queue -> data;
		queue = removeItemFromQueue(queue);

		/* Não sei qual a diferença em inverter o loop */
		for (i = 1 ; i <= vertexes ; i++)
		//for (i = vertexes ; i > 0 ; --i)
			if (getValueFromMatrix(matrix, tempVertex, i) && !markedItems[i]) {
				queue = addItemInQueue(queue, i);
				
				markedItems[i] = markedItems[tempVertex] + 1;
				pai[i] = tempVertex;

#ifdef _DEBUG_
				printf("BFS: Marquei o vértice %u ligado ao vértice %u\n", i, tempVertex);
#endif
			}
	}
	
#ifdef _DEBUG_
	for (i = 1 ; i <= vertexes ; i++)
		printf("Marked Items: %u com nível %u\n",i,markedItems[i] - 1);
	
	for (i = 1 ; i <= vertexes ; i++)
		printf("O pai de %u é %u\n", i, pai[i]);
#endif
	
	for (i = 1 ; i <= vertexes ; i++)
		if (markedItems[i] > 10)
			printf("Maior nível: %u\n", markedItems[i] - 1);
	
}

/* IMPLEMENTATION | BUG-FIX (Error handling and memory)
 * Função para execução de busca em profundidade num grafo em matriz. Root não 
 * pode nunca ser maior que vertexes; se não a pesquisa falha. Falta documentação.
 */
void dfsFromMatrix(uint64_t **matrix, unsigned vertexes, unsigned root) {
	unsigned *markedItems;
	unsigned *pai;
    stack *stack;
	unsigned tempVertex;
	unsigned i;
	
	unsigned *level = (unsigned *) malloc((vertexes + 1) * sizeof(unsigned));

#ifdef _DEBUG_
	printf("\n\nDFS: Start\n");
#endif

	/* Alocar um vértice a mais, visto que a posição 0 não é utilizada */
	markedItems = (unsigned *) calloc(vertexes + 1, sizeof(unsigned));
	
	/* Não preciso inicializar esse vetor */
	pai = (unsigned *) malloc((vertexes + 1) * sizeof(unsigned));
	
	stack = NULL;
	
	stack = stackPush(stack, root);
	level[root] = 0;

#ifdef _DEBUG_
	printf("DFS: Starting from vertex: %u\n", root);
	printf("DFS: Marquei o vértice raiz: %u\n", root);
#endif
	
	while (!stackIsEmpty(stack)) {
		tempVertex = stack -> data;
		stack = stackPop(stack);		
		printf("\nTirei %u da pilha!\n", tempVertex);
		printf("%u esta com marcacao %u!\n", tempVertex, markedItems[tempVertex]);

		printf("Vetor de Marcações:\n");
		for (i = 0 ; i <= vertexes ; i++) {
			printf("markedItems[%u] = %u\n", i, markedItems[i]);
		}

		if (!markedItems[tempVertex]) {
			printf("Marquei %u\n", tempVertex);
			markedItems[tempVertex] = 1; 
		
			/* Não sei qual a diferença em inverter o loop */
			for (i = 1 ; i <= vertexes ; i++) {
			//for (i = vertexes ; i > 0 ; --i)
			
				if (getValueFromMatrix(matrix, tempVertex, i) && !markedItems[i]) {
					stack = stackPush(stack, i);
					printf("Coloquei %u da pilha!\n", i);
					pai[i] = tempVertex;
					level[i] = level[tempVertex] + 1;
					printStack(stack);
				}
			}
		}
	}
	
	for (i = 1 ; i <= vertexes ; i++)
		printf("Marked Items: node[%u] with level = %u\n", i, level[i]);
	
	for (i = 1 ; i <= vertexes ; i++)
		printf("O pai de %u é %u\n", i, pai[i]);
	
}

/* IMPLEMENTATION | BUG-FIX (Error handling and memory, not working at this moment)
 * Função para execução de busca em largura num grafo em lista de adj. Root não 
 * pode nunca ser maior que vertexes; se não a pesquisa falha. Falta documentação.
 */
void bfsFromList(typeAresta *list, unsigned vertexes, unsigned root) {
	unsigned *markedItems;
	unsigned *pai;
    queue *queue;
	unsigned tempVertex;
	typeAresta *temp;

#ifdef _DEBUG_
	unsigned i;
#endif

#ifdef _DEBUG_
	printf("\n\nBFS: Start\n");
#endif

	/* Alocar um vértice a mais, visto que a posição 0 não é utilizada */
	markedItems = (unsigned *) calloc(vertexes + 1, sizeof(unsigned));
	
	/* Não preciso inicializar esse vetor */
	pai = (unsigned *) malloc((vertexes + 1) * sizeof(unsigned));
	
	queue = NULL;
	
	queue = addItemInQueue(queue, root);
	markedItems[root] = 1;

#ifdef _DEBUG_
	printf("BFS: Starting from vertex: %u\n", root);
	printf("BFS: Marquei o vértice raiz: %u\n", root);
#endif
	
	while (!queueIsEmpty(queue)) {
		tempVertex = queue -> data;
		queue = removeItemFromQueue(queue);
		temp = &list[tempVertex];

		while(temp -> next != NULL) {
			if (!markedItems[temp -> next -> vertice]) {
				queue = addItemInQueue(queue, temp -> next -> vertice);				
				markedItems[temp -> next -> vertice] = markedItems[tempVertex] + 1;
				
#ifdef _DEBUG_
				printf("BFS: Marquei o vértice %u ligado ao vértice %u\n", temp -> next -> vertice, tempVertex);
#endif
				
				pai[temp -> next -> vertice] = tempVertex;
				
			}
			temp = temp -> next;
		}
	
	}
		
	
#ifdef _DEBUG_
	for (i = 1 ; i <= vertexes ; i++)
		printf("Marked Items: %u com nível %u\n",i,markedItems[i] - 1);
	
	for (i = 1 ; i <= vertexes ; i++)
		printf("O pai de %u é %u\n", i, pai[i]);
#endif
	
}

/*
 * DFS da lista
 */
void dfsFromList(typeAresta *list, unsigned vertexes, unsigned root) {
	unsigned *markedItems;
	unsigned *pai;
	stack *stack;
	unsigned tempVertex;
	typeAresta *temp;
	
	unsigned *level = (unsigned *) malloc((vertexes + 1) * sizeof(unsigned));

#ifdef _DEBUG_
	unsigned i;
#endif

#ifdef _DEBUG_
	printf("\n\nDFS: Start\n");
#endif

	/* Alocar um vértice a mais, visto que a posição 0 não é utilizada */
	markedItems = (unsigned *) calloc(vertexes + 1, sizeof(unsigned));
	
	/* Não preciso inicializar esse vetor */
	pai = (unsigned *) malloc((vertexes + 1) * sizeof(unsigned));
	
	stack = NULL;
	
	stack = stackPush(stack, root);
	level[root] = 0;

#ifdef _DEBUG_
	printf("DFS: Starting from vertex: %u\n", root);
	printf("DFS: Marquei o vértice raiz: %u\n", root);
#endif
	
	while (!stackIsEmpty(stack)) {
		tempVertex = stack -> data;
		stack = stackPop(stack);		

#ifdef _DEBUG_
		printf("\nTirei %u da pilha!\n", tempVertex);
		printf("%u esta com marcacao %u!\n", tempVertex, markedItems[tempVertex]);
#endif

		temp = &list[tempVertex];

#ifdef _DEBUG_
		printf("Vetor de Marcações:\n");
		for (i = 0 ; i <= vertexes ; i++) {
			printf("markedItems[%u] = %u\n", i, markedItems[i]);
		}
#endif

		if (!markedItems[tempVertex]) {

#ifdef _DEBUG_
			printf("Marquei %u\n", tempVertex);
#endif

			markedItems[tempVertex] = 1; 
		
			/* Não sei qual a diferença em inverter o loop */
			while(temp -> next != NULL) {
				if (!markedItems[temp -> next -> vertice]) {
					stack = stackPush(stack, temp -> next -> vertice);

#ifdef _DEBUG_
					printf("Coloquei %u da pilha!\n", i);
#endif

					pai[temp -> next -> vertice] = tempVertex;
					level[temp -> next -> vertice] = level[tempVertex] + 1;
					printStack(stack);
				}
				temp = temp -> next;
			}
		}
	}

#ifdef _DEBUG_
	for (i = 1 ; i <= vertexes ; i++)
		printf("Marked Items: node[%u] with level = %u\n", i, level[i]);
	
	for (i = 1 ; i <= vertexes ; i++)
		printf("O pai de %u é %u\n", i, pai[i]);
#endif
	
}

/*
 * Trabalho II
 */

typedef struct node_t node_t, *heap_t;
typedef struct edge_t edge_t;
struct edge_t {
	node_t *nd;	/* target of this edge */
	edge_t *sibling;/* for singly linked list */
	double len;	/* edge cost */
};
struct node_t {
	edge_t *edge;	/* singly linked list of edges */
	node_t *via;	/* where previous node is in shortest path */
	double dist;	/* distance from origining node */
	unsigned name;	/* the, er, name */
	int heap_idx;	/* link to heap position for updating distance */
};
 
 
/* --- edge management --- */
#define BLOCK_SIZE 1024 * 64
edge_t *edge_root = 0; 
edge_t *e_next = 0;

/* Don't mind the memory management stuff, they are besides the point.
   Pretend e_next = malloc(sizeof(edge_t)) */
void add_edge(node_t *a, node_t *b, double d) {
	if (e_next == edge_root) {
		edge_root = malloc(sizeof(edge_t) * (BLOCK_SIZE + 1));
		edge_root[BLOCK_SIZE].sibling = e_next;
		e_next = edge_root + BLOCK_SIZE;
	}
	--e_next;
 
	e_next->nd = b;
	e_next->len = d;
	e_next->sibling = a->edge;
	a->edge = e_next;
}
 
void free_edges() {
	for (; edge_root; edge_root = e_next) {
		e_next = edge_root[BLOCK_SIZE].sibling;
		free(edge_root);
	}
}
 
/* --- priority queue stuff --- */
heap_t *heap;
int heap_len;
 
void set_dist(node_t *nd, node_t *via, double d) {
	int i, j;
 
	/* already knew better path */
	if (nd->via && d >= nd->dist) return;
 
	/* find existing heap entry, or create a new one */
	nd->dist = d;
	nd->via = via;
 
	i = nd->heap_idx;
	if (!i) i = ++heap_len;
 
	/* upheap */
	for (; i > 1 && nd->dist < heap[j = i/2]->dist; i = j)
		(heap[i] = heap[j])->heap_idx = i;
 
	heap[i] = nd;
	nd->heap_idx = i;
}
 
node_t * pop_queue() {
	node_t *nd, *tmp;
	int i, j;
 
	if (!heap_len) return 0;
 
	/* remove leading element, pull tail element there and downheap */
	nd = heap[1];
	tmp = heap[heap_len--];
 
	for (i = 1; i < heap_len && (j = i * 2) <= heap_len; i = j) {
		if (j < heap_len && heap[j]->dist > heap[j+1]->dist) j++;
 
		if (heap[j]->dist >= tmp->dist) break;
		(heap[i] = heap[j])->heap_idx = i;
	}
 
	heap[i] = tmp;
	tmp->heap_idx = i;
 
	return nd;
}
 
/* --- Dijkstra stuff; unreachable nodes will never make into the queue --- */
void calc_all (node_t *start) {
	node_t *lead;
	edge_t *e;
	
	set_dist(start, start, 0);
	while ((lead = pop_queue()))
		for (e = lead->edge; e; e = e->sibling) {
			set_dist(e->nd, lead, lead->dist + e->len); //Dijkstra?
			//set_dist(e->nd, lead, e->len); //Prim?
			//printf("%f + %f = %f\n",lead->dist, e->len, lead->dist + e->len);
		}
}
 
void show_path (node_t *nd) {
	if (nd->via == nd)
		printf("%u", nd->name);
	else if (!nd->via)
		printf("%u(unreached)", nd->name);
	else {
		show_path(nd->via);
		printf("-> %u(%0.2f) ", nd->name, nd->dist);
	}
}

double getDist (node_t *nd) {
	return nd->dist;
}

unsigned dijkstra (FILE *file, unsigned vertexes) {
	unsigned i,j;
	double k;
	
	unsigned arestas = 0;
	
	char *line = NULL;
	size_t linecap = 0;
	ssize_t lineLength;
	char *delimiter;

	node_t *nodes = calloc(sizeof(node_t), vertexes);

#ifdef _DEBUG_
	printf("%u\n", vertexes);
#endif
	for (i = 0; i < vertexes; i++) {
		nodes[i].name = i;
#ifdef _DEBUG_
		printf("%u\n", nodes[i].name);
#endif
	}
#ifdef _DEBUG_
	printf("\n");
#endif

	while ((lineLength = getline(&line, &linecap, file)) > 0) {
		
		if (validateWeightedGraphLineInput(line, lineLength))
			return (INVALID_GRAPH_FILE);
		
		i = strtol(line, &delimiter, 10);
		j = strtol(delimiter, &delimiter, 10);
		k = strtod(delimiter, NULL);
		if ((i != 0) && (j != 0)) {
			add_edge(nodes + i-1, nodes + j-1, k);
			add_edge(nodes + j-1, nodes + i-1, k);
			arestas++;

#ifdef _DEBUG_
			printf("Guardei %0.2f na posicao %u,%u\n", k, i, j);
			printf("Passei aqui pela %u vez!\n\n", arestas);
#endif	

		}	
	}
	
	heap = calloc(sizeof(heap_t), vertexes + 1);
	heap_len = 0;
 
	calc_all(nodes); //Aonde começa o Dijkstra
			
/*	
	show_path(nodes + 5708);
	putchar('\n');
	
	show_path(nodes + 11364);
	putchar('\n');
	
	show_path(nodes + 11385);
	putchar('\n');
	
	show_path(nodes + 309496);
	putchar('\n');
	
	show_path(nodes + 343929);
	putchar('\n');
	
	show_path(nodes + 471364);
	putchar('\n');
*/
	
	for (i = 0; i < vertexes; i++) {
		show_path(nodes + i);
		putchar('\n');
	}

	for (i = 0 ; i < vertexes ; i++) {
		printf("Node %u = %0.2f\n", i, getDist(nodes + i));
	}
 
/* real programmers don't free memories (they use Fortran) */
	free_edges();
	free(heap);
	free(nodes);
	
	return OK;
}

double distanciaMedia (FILE *file, unsigned vertexes, unsigned start) {
	unsigned i,j;
	double k;
	
	unsigned arestas = 0;
	
	char *line = NULL;
	size_t linecap = 0;
	ssize_t lineLength;
	char *delimiter;

	node_t *nodes = calloc(sizeof(node_t), vertexes);

#ifdef _DEBUG_
	printf("%u\n", vertexes);
#endif
	for (i = 0; i < vertexes; i++) {
		nodes[i].name = i;
#ifdef _DEBUG_
		printf("%u\n", nodes[i].name);
#endif
	}
#ifdef _DEBUG_
	printf("\n");
#endif

	while ((lineLength = getline(&line, &linecap, file)) > 0) {
		
		if (validateWeightedGraphLineInput(line, lineLength))
			return (INVALID_GRAPH_FILE);
		
		i = strtol(line, &delimiter, 10);
		j = strtol(delimiter, &delimiter, 10);
		k = strtod(delimiter, NULL);
		if ((i != 0) && (j != 0)) {
			add_edge(nodes + i-1, nodes + j-1, k);
			add_edge(nodes + j-1, nodes + i-1, k);
			arestas++;

#ifdef _DEBUG_
			printf("Guardei %0.2f na posicao %u,%u\n", k, i, j);
			printf("Passei aqui pela %u vez!\n\n", arestas);
#endif	

		}	
	}
	
	heap = calloc(sizeof(heap_t), vertexes + 1);
	heap_len = 0;
 
//	calc_all(nodes); //Aonde começa o Dijkstra
			
/*	
	show_path(nodes + 5708);
	putchar('\n');
	
	show_path(nodes + 11364);
	putchar('\n');
	
	show_path(nodes + 11385);
	putchar('\n');
	
	show_path(nodes + 309496);
	putchar('\n');
	
	show_path(nodes + 343929);
	putchar('\n');
	
	show_path(nodes + 471364);
	putchar('\n');
*/
	
	double sum = 0;
	double temp;
	//unsigned counter = 0;
	
	calc_all(nodes + start);
	for (i = 0 ; i < vertexes ; i++) {
		if ((temp = getDist(nodes + i))) {
			sum += temp;
//			counter++;
		}
	}
	
	free_edges();
	free(heap);
	free(nodes);
	
	//printf("%0.2f\n", sum);
	return sum;
	
	//Combinação: Dois ultimos divididos por dois


}

/*
 *
 *
 * PRIM PRIM PRIM PRIM
 *
 *
 */

// A structure to represent a node in adjacency list
struct AdjListNode
{
	long dest;
	long weight;
	struct AdjListNode* next;
};

// A structure to represent an adjacency liat
struct AdjList
{
	struct AdjListNode *head;  // pointer to head node of list
};

// A structure to represent a graph. A graph is an array of adjacency lists.
// Size of array will be V (number of vertices in graph)
struct Graph
{
	long V;
	struct AdjList* array;
};

// A utility function to create a new adjacency list node
struct AdjListNode* newAdjListNode(long dest, long weight)
{
	struct AdjListNode* newNode =
		(struct AdjListNode*) malloc(sizeof(struct AdjListNode));
	newNode->dest = dest;
	newNode->weight = weight;
	newNode->next = NULL;
	return newNode;
}

// A utility function that creates a graph of V vertices
struct Graph* createGraph(long V)
{
	struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph));
	graph->V = V;

	//Create an array of adjacency lists.  Size of array will be V
	graph->array = (struct AdjList*) malloc(V * sizeof(struct AdjList));

	// Initialize each adjacency list as empty by making head as NULL
	for (int i = 0; i < V; ++i)
		graph->array[i].head = NULL;

	return graph;
}
 
// Adds an edge to an undirected graph
void addEdge(struct Graph* graph, long src, long dest, long weight)
{
	// Add an edge from src to dest.  A new node is added to the adjacency
	// list of src.  The node is added at the begining
	struct AdjListNode* newNode = newAdjListNode(dest, weight);
	newNode->next = graph->array[src].head;
	graph->array[src].head = newNode;

	// Since graph is undirected, add an edge from dest to src also
	newNode = newAdjListNode(src, weight);
	newNode->next = graph->array[dest].head;
	graph->array[dest].head = newNode;
}

// Structure to represent a min heap node
struct MinHeapNode
{
	long v;
	long key;
};

// Structure to represent a min heap
struct MinHeap
{
	long size; // Number of heap nodes present currently
	long capacity; // Capacity of min heap
	long *pos; // This is needed for decreaseKey()
	struct MinHeapNode **array;
};

// A utility function to create a new Min Heap Node
struct MinHeapNode* newMinHeapNode(long v, long key)
{
	struct MinHeapNode* minHeapNode =
		(struct MinHeapNode*) malloc(sizeof(struct MinHeapNode));
	minHeapNode->v = v;
	minHeapNode->key = key;
	return minHeapNode;
}

// A utilit function to create a Min Heap
struct MinHeap* createMinHeap(long capacity)
{
	struct MinHeap* minHeap =
		(struct MinHeap*) malloc(sizeof(struct MinHeap));
	minHeap->pos = (long *)malloc(capacity * sizeof(long));
	minHeap->size = 0;
	minHeap->capacity = capacity;
	minHeap->array =
		(struct MinHeapNode**) malloc(capacity * sizeof(struct MinHeapNode*));
	return minHeap;
}

// A utility function to swap two nodes of min heap. Needed for min heapify
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b)
{
	struct MinHeapNode* t = *a;
	*a = *b;
	*b = t;
}

// A standard function to heapify at given idx
// This function also updates position of nodes when they are swapped.
// Position is needed for decreaseKey()
void minHeapify(struct MinHeap* minHeap, long idx)
{
	long smallest, left, right;
	smallest = idx;
	left = 2 * idx + 1;
	right = 2 * idx + 2;

	if (left < minHeap->size &&
		minHeap->array[left]->key < minHeap->array[smallest]->key )
		smallest = left;

	if (right < minHeap->size &&
		minHeap->array[right]->key < minHeap->array[smallest]->key )
	smallest = right;
 
	if (smallest != idx)
	{
		// The nodes to be swapped in min heap
		struct MinHeapNode *smallestNode = minHeap->array[smallest];
		struct MinHeapNode *idxNode = minHeap->array[idx];

		// Swap positions
		minHeap->pos[smallestNode->v] = idx;
		minHeap->pos[idxNode->v] = smallest;

		// Swap nodes
		swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);

		minHeapify(minHeap, smallest);
	}
}

// A utility function to check if the given minHeap is ampty or not
long isEmpty(struct MinHeap* minHeap)
{
	return minHeap->size == 0;
}

// Standard function to extract minimum node from heap
struct MinHeapNode* extractMin(struct MinHeap* minHeap)
{
	if (isEmpty(minHeap))
		return NULL;

	// Store the root node
	struct MinHeapNode* root = minHeap->array[0];

	// Replace root node with last node
	struct MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
	minHeap->array[0] = lastNode;

	// Update position of last node
	minHeap->pos[root->v] = minHeap->size-1;
	minHeap->pos[lastNode->v] = 0;

	// Reduce heap size and heapify root
	--minHeap->size;
	minHeapify(minHeap, 0);

	return root;
}

// Function to decreasy key value of a given vertex v. This function
// uses pos[] of min heap to get the current index of node in min heap
void decreaseKey(struct MinHeap* minHeap, long v, long key)
{
	// Get the index of v in  heap array
	long i = minHeap->pos[v];

	// Get the node and update its key value
	minHeap->array[i]->key = key;

	// Travel up while the complete tree is not hepified.
	// This is a O(Logn) loop
	while (i && minHeap->array[i]->key < minHeap->array[(i - 1) / 2]->key)
	{
		//Swap this node with its parent
		minHeap->pos[minHeap->array[i]->v] = (i-1)/2;
		minHeap->pos[minHeap->array[(i-1)/2]->v] = i;
		swapMinHeapNode(&minHeap->array[i],  &minHeap->array[(i - 1) / 2]);
 
		//move to parent index
		i = (i - 1) / 2;
	}
}

// A utility function to check if a given vertex
// 'v' is in min heap or not
bool isInMinHeap(struct MinHeap *minHeap, int v)
{
	if (minHeap->pos[v] < minHeap->size)
		return true;
	return false;
}

// A utility function used to print the constructed MST
void printArr(long arr[], long weight[], long n)
{
	long totalWeight = 0;

	for (int i = 1 ; i < n; ++i) {
		printf(" %i %li %li\n", i, arr[i], weight[i]);
		totalWeight += weight[i];
	}
	
	printf("Peso Total: %li\n", totalWeight/100000000000);
}

// The main function that constructs Minimum Spanning Tree (MST)
// using Prim's algorithm
void PrimMST(struct Graph* graph)
{
	long V = graph->V;// Get the number of vertices in graph
	long parent[V];   // Array to store constructed MST
	long key[V];      // Key values used to pick minimum weight edge in cut

	//minHeap represents set E
	struct MinHeap* minHeap = createMinHeap(V);

	// Initialize min heap with all vertices. Key value of
	// all vertices (except 0th vertex) is initially infinite
	for (long v = 1; v < V; ++v)
	{
		parent[v] = -1;
		key[v] = 1000000000000000;
		minHeap->array[v] = newMinHeapNode(v, key[v]);
		minHeap->pos[v] = v;
	}

	// Make key value of 0th vertex as 0 so that it
	// is extracted first
	key[0] = 0;
	minHeap->array[0] = newMinHeapNode(0, key[0]);
	minHeap->pos[0]   = 0;

	// Initially size of min heap is equal to V
	minHeap->size = V;

	// In the followin loop, min heap contains all nodes
	// not yet added to MST.
	while (!isEmpty(minHeap))
	{
		// Extract the vertex with minimum key value
		struct MinHeapNode* minHeapNode = extractMin(minHeap);
		long u = minHeapNode->v; // Store the extracted vertex number

		// Traverse through all adjacent vertices of u (the extracted
		// vertex) and update their key values
		struct AdjListNode* pCrawl = graph->array[u].head;
		while (pCrawl != NULL)
		{
			long v = pCrawl->dest;

			// If v is not yet included in MST and weight of u-v is
			// less than key value of v, then update key value and
			// parent of v
			if (isInMinHeap(minHeap, v) && pCrawl->weight < key[v])
			{
				key[v] = pCrawl->weight;
				parent[v] = u;
				decreaseKey(minHeap, v, key[v]);
			}
			pCrawl = pCrawl->next;
		}
	}

	// print edges of MST
	printArr(parent, key, V);
}

// Driver program to test above functions
int prim (FILE *file, unsigned vertexes) {
	// Let us create the graph given in above fugure
	struct Graph* graph = createGraph(vertexes);
	
	long i,j;
	double k;
	
	unsigned arestas = 0;
	
	char *line = NULL;
	size_t linecap = 0;
	ssize_t lineLength;
	char *delimiter;

	while ((lineLength = getline(&line, &linecap, file)) > 0) {
		
		if (validateWeightedGraphLineInput(line, lineLength))
			return (INVALID_GRAPH_FILE);
		
		i = strtol(line, &delimiter, 10);
		j = strtol(delimiter, &delimiter, 10);
		k = strtod(delimiter, NULL);
		k *= 100000000000;
		if ((i != 0) && (j != 0)) {
			addEdge(graph, i-1, j-1, (long)k);
			addEdge(graph, j-1, i-1, (long)k);
			arestas++;

#ifdef _DEBUG_
			printf("Guardei %0.2f na posicao %li,%li\n", k, i, j);
			printf("Passei aqui pela %u vez!\n\n", arestas);
#endif	

		}	
	}

	PrimMST(graph);	

	return OK;
}

/************************************************/




/*
 * Função para descobrir componentes conexões de um dado grafo; a base da função
 * é uma BFS; mas neste caso utilizamos o vetor de marcação não para guardar o
 * nível da BFS, mas sim a que componente conexa as arestas pertencem.
 * Esta função está com uma alocação estática de memória que deve ser consertada.
 */
void connectedComponentsFromMatrix(uint64_t **matrix, unsigned vertexes) {
	unsigned *markedItems;
    queue *queue;
	unsigned tempVertex;
	unsigned i;	
	unsigned j;
	unsigned numVertexes[100000] = { 0 }; //Refazer declaração com malloc();
	unsigned connectedComponents;

	/* Alocar um vértice a mais, visto que a posição 0 não é utilizada */
	markedItems = (unsigned *) calloc(vertexes + 1, sizeof(unsigned));
	
	/* Inicio das componentes conexas; nenhuma a priori */
	connectedComponents = 0;
	//numVertexes = (unsigned *) malloc(sizeof(unsigned));	
	
	for (j = 1 ; j <= vertexes ; j++) {		
		if (!markedItems[j]) {
			connectedComponents++;
			//numVertexes = (unsigned *) realloc(numVertexes, connectedComponents * sizeof(unsigned));
			
			/* Reset na fila */
			queue = NULL;
			queue = addItemInQueue(queue, j);
			markedItems[j] = connectedComponents;
			
			while (!queueIsEmpty(queue)) {
				tempVertex = queue -> data;
				queue = removeItemFromQueue(queue);
				
				/* Contar vertices */
				numVertexes[connectedComponents]++;
		
				/* Não sei qual a diferença em inverter o loop */
				for (i = 1 ; i <= vertexes ; i++)
				//for (i = vertexes ; i > 0 ; --i)
					if (getValueFromMatrix(matrix, tempVertex, i) && !markedItems[i]) {
						queue = addItemInQueue(queue, i);
						markedItems[i] = connectedComponents;
					}
			}
			
#ifdef _DEBUG_			
			printf("Numero de vertices do grafo: %u\n", numVertexes[connectedComponents]);
	
			for (i = 1 ; i <= vertexes ; i++)
				printf("Marked Items: %u com nível %u\n", i, markedItems[i]);
#endif
		}
	}
	fprintf(stdout, "Numero de componentes conexos: %u\n", connectedComponents);
	
	for (i = 1 ; i <= connectedComponents ; i++)
		fprintf(stdout, "O número de vértices do componente conexo %u é %u\n", i, numVertexes[i]);
	
}

/*
 * Função para descobrir componentes conexões de um dado grafo; a base da função
 * é uma BFS; mas neste caso utilizamos o vetor de marcação não para guardar o
 * nível da BFS, mas sim a que componente conexa as arestas pertencem.
 * Esta função está com uma alocação estática de memória que deve ser consertada.
 */
void connectedComponentsFromList(typeAresta *list, unsigned vertexes) {
	unsigned *markedItems;
    queue *queue;
	unsigned tempVertex;
	unsigned i;	
	unsigned j;
	unsigned numVertexes[100000] = { 0 }; //Refazer declaração com malloc();
	unsigned connectedComponents;
	typeAresta *temp;

	/* Alocar um vértice a mais, visto que a posição 0 não é utilizada */
	markedItems = (unsigned *) calloc(vertexes + 1, sizeof(unsigned));
	
	/* Inicio das componentes conexas; nenhuma a priori */
	connectedComponents = 0;
	//numVertexes = (unsigned *) malloc(sizeof(unsigned));	
	
	for (j = 1 ; j <= vertexes ; j++) {		
		if (!markedItems[j]) {
			connectedComponents++;
			//numVertexes = (unsigned *) realloc(numVertexes, connectedComponents * sizeof(unsigned));
			
			/* Reset na fila */
			queue = NULL;
			queue = addItemInQueue(queue, j);
			markedItems[j] = connectedComponents;
			
			while (!queueIsEmpty(queue)) {
				tempVertex = queue -> data;
				queue = removeItemFromQueue(queue);
				temp = &list[tempVertex];
				
				/* Contar vertices */
				numVertexes[connectedComponents]++;
		
				/* Não sei qual a diferença em inverter o loop */
				while(temp -> next != NULL) {
					if (!markedItems[temp -> next -> vertice]) {
						queue = addItemInQueue(queue, temp -> next -> vertice);
						markedItems[temp -> next -> vertice] = connectedComponents;
					}
				temp = temp -> next;
				}
			}
			
#ifdef _DEBUG_			
			printf("Numero de vertices do grafo: %u\n", numVertexes[connectedComponents]);
	
			for (i = 1 ; i <= vertexes ; i++)
				printf("Marked Items: %u com nível %u\n", i, markedItems[i]);
#endif
		}
	}
	fprintf(stdout, "Numero de componentes conexos: %u\n", connectedComponents);
	
	for (i = 1 ; i <= connectedComponents ; i++)
		fprintf(stdout, "O número de vértices do componente conexo %u é %u\n", i, numVertexes[i]);
	
}

/*
 * Função para escrever no arquivo de saída do programa.
 */
void generateOutputFile(FILE *file, unsigned vertices, unsigned arestas, unsigned *graus) {
	unsigned i;

	fprintf(file, "# n = %d\n", vertices);
	fprintf(file, "# m = %d\n", arestas);
	fprintf(file, "# d_medio = %0.5f\n", (float) 2 * arestas / vertices);
	for (i = 1 ; i <= vertices ; i++) {
		fprintf(file, "%d %0.5f\n", i, (float) graus[i] / vertices);
	}
}

/*
 * Função simples e que não é errorsafe apenas para imprimir na tela uma matriz 
 * de adjacencias.
 */
void printAdjacencyMatrix(unsigned vertex, uint64_t **matrix) {
	unsigned i,j;
	
	printf("x | ");
	for (i = 1 ; i <= vertex ; i++) {
		printf("%u ", i);
	}
	printf("\n--+");
	for (i = 1 ; i <= vertex ; i++) {
		printf("--");
	}
	printf("\n");
	for (i = 1 ; i <= vertex ; i++) {
		printf("%u | ", i);
		for (j = 1 ; j <= vertex ; j++) {
			printf("%u ", getValueFromMatrix(matrix, i, j));
		}
		printf("\n");
	}
}

/*
 * Função simples e que não é errorsafe apenas para imprimir na tela uma lista 
 * de adjacencias utilizando um array de listas encadeadas.
 */
void printAdjacencyList(unsigned vertex, typeAresta *graph) {
	typeAresta *temp;
	unsigned i;
	
	for (i = 0 ; i <= vertex ; i++) {
		temp = &graph[i];
		printf("%u: ", i);
		while(temp -> next != NULL) {
			printf("%d -> ", temp -> next -> vertice);
			temp = temp -> next;
		}
		printf("NULL\n");
	}
}

