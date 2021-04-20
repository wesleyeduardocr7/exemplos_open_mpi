#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "matrix.h"
#include "stdlib.h"
#include "timer.h"
#include "results.h"
#include "jacobi.h"

jacobi_result* jacobi_mpi(matrix *m, bool verbose, int process_num, int process_count) {

	int *counts = malloc(process_count * sizeof(int));
	int *disps = malloc(process_count * sizeof(int));
	int resto = m->size % process_count;
	int qtd = ceil((double) m->size / process_count);
	int i;
	for (i = 0; i < process_count; i++) {
		counts[i] = qtd + 2;
		if (i == 0) {
			disps[i] = 0;			
		} else {
			disps[i] = disps[i - 1] + counts[i - 1];		
		}
		if (i == resto - 1) qtd--;
	}

	qtd = ceil((double) m->size / process_count);
	int initial_line;
	if (resto > 0 && process_num > resto - 1) {
		initial_line = qtd * resto + (qtd - 1) * (process_num - resto);
		qtd--;
	} else {
		initial_line = qtd * process_num;

	}
	int end_line = initial_line + qtd - 1;

	if (verbose) {		
		MPI_Barrier(MPI_COMM_WORLD);
		printf("A Process %i: %i -> %i (%i)\n",	process_num, initial_line, end_line, counts[process_num] - 2);		
	}

	int j, z, k = 0, terminar = 0;
	int n1 = disps[process_num] + qtd;
	int n2 = disps[process_num] + qtd + 1;
	double norma = 0, norma_ant = 0, n1_sum, n2_sum, soma;

	double *shared = malloc((m->size + 2 * process_count) * sizeof(double));
	double *x0 = malloc(m->size * sizeof(double));	
	double x2;

	for (i = 0; i < m->size; i++) {
		x0[i] = 1.0;
	}

	while (k < 100 && !terminar) {

		shared[n1] = 0;
		shared[n2] = 0;
		for (i = initial_line, z = disps[process_num]; i <= end_line; i++, z++) {

			soma = 0;
			item_matrix *item = m->a[i];
			if (item) {
				double diagonal_value = 0;
				while (item->column >= 0) {
					j = item->column;
					if (j != i) {
						soma += item->value * x0[j];
					} else {
						diagonal_value = item->value;
					}
					item++;

				}

				if (verbose) printf("Process %i: linha = %i, soma = %f\n", process_num, i, soma);
				if (diagonal_value == 0) {
					puts("nok");
					MPI_Finalize();
					exit(1);
				}
				shared[z] = (m->b[i] - soma) / diagonal_value;
				x2 = shared[z] - x0[i];

				shared[n1] += x2 * x2;
				shared[n2] += shared[z] * shared[z];
			} else {
				puts("nokx");
				MPI_Finalize();
				exit(0);
			}

		}

		MPI_Allgatherv(
				MPI_IN_PLACE,
				0,
				MPI_DATATYPE_NULL,
				&(shared[0]),
				counts,
				disps,
				MPI_DOUBLE,
				MPI_COMM_WORLD);

		if (process_num == 0) {

			n1_sum = 0;
			n2_sum = 0;
			for (i = 0; i < process_count; i++) {
				n1_sum += shared[disps[i] + counts[i] - 2];
				n2_sum += shared[disps[i] + counts[i] - 1];
			}
			norma = sqrt(n1_sum / n2_sum);
			
			if ((k > 1 && (norma <= precision)) || isnan(norma)) {
				terminar = 1;
			} else {
				norma_ant = norma;
			}

		}

		MPI_Bcast(&terminar, 1, MPI_INT, 0, MPI_COMM_WORLD);

		if (!terminar) {
			k++;
			for (i = 0, z = 0; i < process_count; i++) {
				for (j = 0; j < counts[i] - 2; ++j) {
					x0[z++] = shared[disps[i] + j];
				}
			}
		}
		if (verbose && process_num == 0) {
			printf("\nx0 = ");
			for (i = 0; i < m->size; i++) {
				printf("%f, ", x0[i]);
			}
			printf("\n");
		}

	}

	if (process_num == 0) {
		jacobi_result* res = malloc(sizeof(jacobi_result));
		res->x = malloc(m->size * sizeof(double));
		for (i = 0, z = 0; i < process_count; i++) {
			for (j = 0; j < counts[i] - 2; ++j) {
				res->x[z++] = shared[disps[i] + j];
			}
		}
		res->e = norma;
		res->k = k;
		free(counts);
		free(disps);
		free(shared);
		free(x0);

		return res;
	} else {		
		free(counts);
		free(disps);
		free(shared);
		free(x0);
		return NULL;
	}
}
