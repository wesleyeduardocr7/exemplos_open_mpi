#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "matrix.h"
#include "stdlib.h"
#include "timer.h"
#include "results.h"
#include "jacobi.h"

int main(int argc, char *argv[]) {
	int			my_rank;		
	int			num_procs;	
	int			source;			
	int			dest = 0;		
	int			tag = 0;		
	char		message[100];	
	MPI_Status	status ;		

	MPI_Init(&argc, &argv);	
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	
	if (my_rank != 0) {		
		snprintf(message,26, "From process %d!", my_rank);		
		MPI_Send(message, strlen(message)+1, MPI_CHAR,dest, tag, MPI_COMM_WORLD);
	} else {
		printf("Num processes: %d\n",num_procs);
		for (source = 1; source < num_procs; source++) {
			MPI_Recv(message, 100, MPI_CHAR, source, tag,MPI_COMM_WORLD, &status);
				printf("Process 0 received \"%s\"\n",message);
			}
			snprintf(message, 26, "Hi, how are you?           ");
		}		
		MPI_Bcast(message, strlen(message)+1, MPI_CHAR, dest, MPI_COMM_WORLD);

	if (my_rank != 0) {
			printf("Process %d received \"%s\"\n", my_rank, message);
	}

	matrix *m = matrix_load(argv[1]);

	if (my_rank == 0)
		matrix_print(m);

	timer* t;
	if (my_rank == 0)
		t = start_timer();

	jacobi_result* result = jacobi_mpi(m, my_rank, num_procs);

	if (my_rank == 0)
		stop_timer(te);

	if (my_rank == 0 && result != NULL) {
		int i;
		printf("\nResults: ");
		for (i = 0; i < m->size; i++) {
			printf("%f, ", result->x[i]);
		}
		printf("\nIterations: %i ", result->k);
	}

	if (my_rank == 0)
		write_results(t, argv[1], num_procs, 'M', m->size, argv[3]);

	matrix_destroy(m);

	if (my_rank == 0 && result != NULL) {
		free(t);	
		free(result);
	}

	MPI_Finalize(); 

	return 0;
}
