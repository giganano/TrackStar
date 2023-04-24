/*
This file is part of the trackfitting package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/trackfitting.git.
*/

#include <stdlib.h>
#include "matrix.h"


/*
Allocate memory for and return a pointer to a MATRIX object.
Automatically initializes all matrix elements to zero.

Parameters
----------
n_rows : ``unsigned short``
	The number of rows in the matrix.
n_cols : ``unsigned short``
	The number of columns in the matrix.

Returns
-------
mat : ``MATRIX *``
	The newly constructed ``n_rows`` x ``n_cols`` matrix with each element
	``mat``:math:`_{ij}` initialized to zero.
*/
extern MATRIX *matrix_initialize(unsigned short n_rows, unsigned short n_cols) {

	unsigned short i, j;
	MATRIX *m = (MATRIX *) malloc (sizeof(MATRIX));
	m -> matrix = (double **) malloc (n_rows * sizeof(double *));
	for (i = 0u; i < n_rows; i++) {
		m -> matrix[i] = (double *) malloc (n_cols * sizeof(double));
		for (j = 0u; j < n_cols; j++) m -> matrix[i][j] = 0.0;
	}

	/*
	assign the number of rows and columns by modifying the stored value of the
	memory block directly. This is the only place in the code where these
	attributes of the MATRIX object are assigned.
	*/
	*((unsigned short *) &(m -> n_rows)) = n_rows;
	*((unsigned short *) &(m -> n_cols)) = n_cols;
	return m;

}


/*
Free up the memory associated with a MATRIX object.

Parameters
----------
m : ``MATRIX *``
	The matrix to free itself.
*/
extern void matrix_free(MATRIX *m) {

	if (m != NULL) {

		if ((*m).matrix != NULL) {
			unsigned short i;
			for (i = 0u; i < (*m).n_rows; i++) {
				if ((*m).matrix[i] != NULL) free(m -> matrix[i]);
			}
			free(m -> matrix);
		} else {}

		free(m);

	} else {}

}

