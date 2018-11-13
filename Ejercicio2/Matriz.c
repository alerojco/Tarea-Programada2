//Ejercicio 2 
//Tarea Programada 2 
//Estructuras de Computadores Digitales 2 II-2018 
//Alejandro Rojas Cordero B36049
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

#define filas_A 14
#define columnas_A 14   //Estos valores puede cambiarse 
#define columnas_B 14

int printMatrixC(double matrix[filas_A][columnas_B]) {

	for (int i = 0; i < filas_A; i++) {// Imprime MatrixC
        	printf("\n"); 
         	for (int j = 0; j < columnas_B; j++) 
            		printf("%6.2f   ", matrix[i][j]);
      	}

}

int main (int argc, char *argv[]) {

	int rank, size, ierr;

	MPI_Status status;

	ierr = MPI_Init(&argc,&argv);
	ierr = MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	ierr = MPI_Comm_size(MPI_COMM_WORLD,&size);

	double MatrixA[filas_A][columnas_A];
	double MatrixB[columnas_A][columnas_B];
	double MatrixC[filas_A][columnas_B];

	int offset, filas, aux;

	double tiempo_inicio = MPI_Wtime();
	
	if (rank == 0) {// Proceso Principal

		// Llena la matriz de numeros
		for (int i = 0; i < filas_A; i++)
        	 	for (int j = 0; j < columnas_A; j++)
            			MatrixA[i][j]= i + j;
      		for (int r = 0; r < columnas_A; r++)
         		for (int q = 0; q < columnas_B; q++)
            			MatrixB[r][q]= r + q;	

		offset = 0;
		aux = filas_A / (size - 1);

		for (int rank_destino = 1; rank_destino <= (size - 1); rank_destino++) {
		
			filas = (rank_destino <= (filas_A % (size - 1))) ? aux + 1 : aux;

			MPI_Send(&offset, 1, MPI_INT, rank_destino, 123, MPI_COMM_WORLD);
			MPI_Send(&filas, 1, MPI_INT, rank_destino, 321, MPI_COMM_WORLD);
			MPI_Send(&MatrixA[offset][0], filas * columnas_A, MPI_DOUBLE, rank_destino, 456, MPI_COMM_WORLD);
			MPI_Send(&MatrixB, columnas_B * columnas_A, MPI_DOUBLE, rank_destino, 654, MPI_COMM_WORLD);

			offset = offset + filas;

		}

		for (int fuente = 1; fuente <= (size - 1); fuente++) {

			MPI_Recv(&offset, 1, MPI_INT, fuente, 123, MPI_COMM_WORLD, &status);
			MPI_Recv(&filas, 1, MPI_INT, fuente, 321, MPI_COMM_WORLD, &status);
			MPI_Recv(&MatrixC[offset][0], filas * columnas_B, MPI_DOUBLE, fuente, 789, MPI_COMM_WORLD, &status);
		}	

		printMatrixC(MatrixC);

	}

	if (rank > 0) {// Procesos esclavos

		MPI_Recv(&offset, 1, MPI_INT, 0, 123, MPI_COMM_WORLD, &status);
		MPI_Recv(&filas, 1, MPI_INT, 0, 321, MPI_COMM_WORLD, &status);
		MPI_Recv(&MatrixA, filas * columnas_A, MPI_DOUBLE, 0, 456, MPI_COMM_WORLD, &status);
		MPI_Recv(&MatrixB, columnas_A * columnas_B, MPI_DOUBLE, 0, 654, MPI_COMM_WORLD, &status);

		for (int x = 0; x < columnas_B; x++) 
			for (int z = 0; z < filas; z++) {

				MatrixC[z][x] = 0.0;

				for (int w = 0; w < columnas_A; w++) 

					MatrixC[z][x] = MatrixC[z][x] + MatrixA[z][w] * MatrixB[w][x];

			}

		MPI_Send(&offset, 1, MPI_INT, 0, 123, MPI_COMM_WORLD);
		MPI_Send(&filas, 1, MPI_INT, 0, 321, MPI_COMM_WORLD);
		MPI_Send(&MatrixC, filas * columnas_B, MPI_DOUBLE, 0, 789, MPI_COMM_WORLD);
		
	}

	MPI_Finalize();
	return 0;

}
