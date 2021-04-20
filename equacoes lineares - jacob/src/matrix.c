#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "matrix.h"

matrix* matrix_load_mm(char* filename) {

	long i, size, size_cols, count, row, col;

	FILE *file;
	file = fopen(filename, "r");
	if (file == NULL) {
		puts("\nCannot open file");
		puts(filename);
		MPI_Finalize();
		exit(0);
	}

	fscanf(file, "%li", &size);
	fscanf(file, "%li", &size_cols);
	fscanf(file, "%li", &count);

	if (size != size_cols) {
		puts("\nMatrix not symmetric");
		MPI_Finalize();
		exit(0);
	}

	matrix *m = malloc(sizeof(matrix));
	m->size = size;

	m->b = malloc(size * sizeof(double));

	m->x = malloc(size * sizeof(double));

	m->a = malloc(size * sizeof(item_matrix*));
	for (i = 1;i < size; i++) {
		m->a[i] = NULL;
	}

	m->a[0] = malloc((count + size) * sizeof(item_matrix));

	item_matrix *position = m->a[0];
	int last_row = 1;
	for (i = 0; i < count; i++) {
		fscanf(file, "%li", &row);
		fscanf(file, "%li", &col);
		if (row != last_row) {
			position->column = -1;
			position++;
		}
		position->column = col - 1;
		if (!fscanf(file, "%lf", &position->value)) {
			break;
		}
		if (row != last_row) {
			m->a[row - 1] = position;
			last_row = row;
		}
		position++;

	}
	position->column = -1;

	char filename_b[100];
	strncpy((char*)  &filename_b, filename, strlen(filename) - 4);
	filename_b[strlen(filename) - 4] = '\0';
	strcat((char*) &filename_b, "_b.mtx");
	file = fopen(filename_b, "r");
	if (file == NULL) {
		puts("\nCannot open file");
		puts(filename_b);
		exit(0);
	}

	fscanf(file, "%li", &count);
	fscanf(file, "%li", &size_cols);

	for (i = 0; i < count; i++) {
		if (!fscanf(file, "%lf", &m->b[i])) {
			break;
		}
	}
	fclose(file);

	char filename_x[100];
	strncpy((char*) &filename_x, filename, strlen(filename) - 4);
	filename_x[strlen(filename) - 4] = '\0';
	strcat((char*) &filename_x, "_x.mtx");

	if (access(filename_x, R_OK) != -1) {

		file = fopen(filename_x, "r");
		if (file == NULL) {
			puts("\nCannot open file");
			puts(filename_x);
			MPI_Finalize();
			exit(0);
		}

		fscanf(file, "%li", &count);
		fscanf(file, "%li", &size_cols);

		for (i = 0; i < count; i++) {
			if (!fscanf(file, "%lf", &m->x[i])) {
				break;
			}
		}

		fclose(file);
	} else {
		for (i = 0; i < size; i++) {
			m->x[i] = -1.0;
		}
	}

	return m;

}

matrix* matrix_load_original(char* filename) {
	int i, j, size;
	matrix *m = malloc(sizeof(matrix));

	FILE *file;
	file = fopen(filename, "r");
	if (file == NULL) {
		puts("\nCannot open file");
		puts(filename);
		MPI_Finalize();
		exit(0);
	}

	fscanf(file, "%iu", &size);

	m->b = malloc(size * sizeof(double));

	m->x = malloc(size * sizeof(double));

	m->a = malloc(size * sizeof(item_matrix*));

	m->a[0] = malloc((size*size + size) * sizeof(item_matrix));

	char c[10];

	item_matrix *position = m->a[0];
	for (i = 0; i < size; i++) {
		m->a[i] = position;		
		for (j = 0; j < size; j++) {
			position->column = j;
			if (!fscanf(file, "%lf", &position->value)) {
				break;
			}
			position++;
		}
		position->column = -1;
		position++;

		fscanf(file, "%s", (char*) &c);

		if (!fscanf(file, "%lf", &m->b[i])) {
			break;
		}
	}

	for (i = 0; i < size; i++) {
		if (!fscanf(file, "%lf", &m->x[i])) {
			break;
		}
	}
	fclose(file);

	m->size = size;
	return m;
}


bool is_mm(char* filename) {
	char *dot = strrchr(filename, '.');
	return dot && !strcmp(dot, ".mtx");
}

matrix* matrix_load(char* filename) {
	if (is_mm(filename)) {
		return matrix_load_mm(filename);
	}
	return matrix_load_original(filename);
}


void matrix_destroy(matrix* matrix) {
	#ifndef __linux__
		return;
	#endif	
	free(matrix);
}

void matrix_print(matrix *m) {
	int i, j, s;
	s = m->size;
	for (i = 0; i < s && i < 200; i++) {
		item_matrix *item = m->a[i];
		if (item) {
			while (item->column >= 0) {
				j = item->column;
				printf("(%i,%i)=%f ", i, j, item->value);
				item++;
			}
		}
		printf("= %f\n", m->b[i]);
	}
	puts("\nExpected result: ");
	for (i = 0; i < s; i++) {
		if (i > 0) printf(", ");
		printf("%f", m->x[i]);
	}
	puts("\n");
}
