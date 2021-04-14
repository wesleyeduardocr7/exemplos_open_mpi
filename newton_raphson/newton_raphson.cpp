#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mpi.h"

int main(int argc, char* argv[]) {

	float raiz_newton_raphson(float x0);
		
	int myrank;
	MPI_Status status;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	if (myrank == 0){
		float x0 = 1000000000;	
		printf("\nX0 = %f\n", x0);
		MPI_Send(&x0, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
	}

	if (myrank == 1) {

		float x0_recv = 0.;
		
		MPI_Recv(&x0_recv, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD,&status);
		
		double t_initial = MPI_Wtime();
		float xn = raiz_newton_raphson(x0_recv);
		double t_final = MPI_Wtime() - t_initial;
		
		printf("\nTempo de execucao: %lf", t_final / ((CLOCKS_PER_SEC / 1000)));

		MPI_Send(&xn, 1, MPI_FLOAT, 2, 0, MPI_COMM_WORLD);
	}

	if (myrank == 2) {

		float raiz_recv = 0.;
		MPI_Recv(&raiz_recv, 1, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

		printf("\nRaiz = %f\n", raiz_recv);
	}

	MPI_Finalize();
	return 0;
}


float raiz_newton_raphson(float x0) {
		
	int i = 0,qtd_iteracoes = 1;
	float xn = 0.0, fxn, raiz;
	float eps = 1.0E-6;
	float f(float);
	float df(float);
	fxn = 1.0;

	for (i; f(xn) > eps; i++) {
		xn = x0 - f(x0) / df(x0);
		fxn = fabs(f(xn));
		qtd_iteracoes++;
		x0 = xn;
	}

	printf("\nConvergiu Apos  %d Iteracoes.\n", qtd_iteracoes);
	return xn;
}


float f(float x) {
	return (x * x) - 5 * x + 6;
}

float df(float x) {
	return 2 * x - 5;
}
