/*
 * 
 * Main program
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>

#include "errors.h"
#include "const.h"
#include "types.h"
#include "functions.h"

int main(int argc, char **argv) {
	char filename[MAX_FILENAME_LENGTH];
	FILE *file;

#ifdef _DEBUG_
//	unsigned i,j;
#endif

#ifndef _DEBUG_
	unsigned i;
#endif

	unsigned vertices;
	unsigned arestas;

//	struct timeval t1, t2;

	datastructure typeOfDatastructure = matrix;
	double **matrix;
	typeAresta *list;
	
	unsigned *graus;

	/* Verificação de argumentos de linha de comando */
	if (argc > ARGS+1) {
		printf("Uso: %s <arquivo>\n",argv[0]);
		exit(INVALID_ARGS);
	}

	if (argc <= ARGS) {
		printf("Entre com o nome do arquivo: ");
		fgets(filename, MAX_FILENAME_LENGTH, stdin);
		filename[strlen(filename)-1] = '\0';
	} else
		strcpy(filename,argv[1]);

	/* Tentar abrir arquivo texto */
	if (!(file = fopen(filename, "r"))) {
		fprintf(stderr, "ERRO: Falha ao abrir arquivo com informação do grafo\n");
		exit(ERROR_OPENING_FILE);
	}
	
	/* Pegar vertices do arquivo */
	if (!(vertices = getVertexFromFile(file))) {
		fprintf(stderr, "ERRO: O arquivo fornecido não é valido\n");
		exit(vertices);
	}

#ifdef _DEBUG_
	printf("getVertexFromFile retornou o valor %u\n", vertices);
#endif

	switch (typeOfDatastructure) {
		case 0:
		
/*			matrix = createWeightedMatrix(vertices, vertices);
			if (!(arestas = loadWeightedAdjacencyMatrixFromFile(file, matrix))) {
				fprintf(stderr, "ERRO: O arquivo fornecido não é valido\n");
				exit(arestas);
			}

#ifdef _DEBUG_
			printf("loadWeightedAdjacencyMatrixFromFile retornou o valor %u\n", arestas);
#endif
*/
			break;
		
		case 1:
	
			list = createList(vertices);
			if (!(arestas = loadAdjacencyListFromFile(file, list))) {
				fprintf(stderr, "ERRO: O arquivo fornecido não é valido\n");
				exit(arestas);
			}
	
#ifdef _DEBUG_
			printf("loadAdjacencyListFromFile retornou o valor %u\n", arestas);
			printAdjacencyList(vertices, list);
#endif
	
			break;
		
		default:
			fprintf(stderr, "ERRO: Estrutura de dados inválida\n");
			exit(INVALID_DATA_STRUCTURE);
			break;
	}
	
	/* Fechar arquivo */
//	fclose(file);
	
#ifdef _DEBUG_
	printf("Passei das estruturas de dados!\n");
#endif
	
	/* Inicializar os graus */
	graus = (unsigned *) calloc(vertices + 1, sizeof(unsigned));
	
	//double *distTemp = (double*) calloc(vertices, sizeof(double));
	double distTemp = 0;
		
	switch (typeOfDatastructure) {
		case 0:
		
			//gettimeofday(&t1, NULL);
			//for (i = 1 ; i <= vertices ; i++)
			//bfsFromMatrix(matrix, vertices, 4);
			//dfsFromMatrix(matrix, vertices, 4);
			//connectedComponentsFromMatrix(matrix, vertices);
			//gettimeofday(&t2, NULL);
			
			//graus = calculateGraphDegreeFromMatrix(matrix, vertices);
		
			//dijkstra(file, vertices);
			prim(file, vertices);
			
		/*
			for (i = 0 ; i < vertices ; i++) {
				distTemp += distanciaMedia(file, vertices, i);
				rewind(file);
				getline(NULL, NULL, file);
			}
			
			printf("Distancia Media: %f\n",(distTemp/((vertices * (vertices-1))/2)));
		*/	
			break;
			
		case 1:
			
			//gettimeofday(&t1, NULL);
			//for (i = 1 ; i <= vertices ; i++) {
			//	printf("Vértice %u\n", i);
			//	bfsFromList(list, vertices, i);
			//}
			//dfsFromList(list, vertices, 4);
			//connectedComponentsFromList(list, vertices);
			//gettimeofday(&t2, NULL);
		
			//graus = calculateGraphDegreeFromList(list, vertices);
			break;
			
		default:
			fprintf(stderr, "ERRO: Estrutura de dados inválida\n");
			exit(INVALID_DATA_STRUCTURE);
			break;
	}
/*	
	double elapsedTime;
	elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
	elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
	printf("Tempo: %fms.\n", elapsedTime);
*/
	/* Tentar abrir arquivo texto para a saída */
/*	strcpy(filename,"output");
	if (!(file = fopen(filename, "w"))) {
		fprintf(stderr, "ERRO: Falha ao abrir arquivo de saída para escrita\n");
		exit(ERROR_OPENING_FILE);
	}
	
	generateOutputFile(file, vertices, arestas, graus);
*/		
	fclose(file);
	free(graus);

	return EXIT_SUCCESS;

}
