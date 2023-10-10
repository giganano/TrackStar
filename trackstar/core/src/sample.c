/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git.
*/

#if defined(_OPENMP)
	#include <omp.h>
#endif /* _OPENMP */
#include <stdlib.h>
#include "sample.h"


/*
.. cpp:function:: extern SAMPLE *sample_initialize(unsigned long sample_size);

Allocate memory for and return a pointer to a ``SAMPLE`` object.
Automatically allocates memory for the appropriate number of ``DATUM`` objects,
but does not allocate memory for each individual ``DATUM`` stored. That is,
after calling this function, the user will have to call ``datum_initialize``
the appropriate number of times to copy their data into this object.

Parameters
----------
sample_size : ``unsigned long``
	The number of data vectors in the sample. This will be copied over to the
	attribute ``n_vectors`` of the newly constructed object.

Returns
-------
s : ``SAMPLE *``
	The newly constructed sample, ready to have its vectors initialized.
*/
extern SAMPLE *sample_initialize(unsigned long sample_size) {

	SAMPLE *s = (SAMPLE *) malloc (sizeof(SAMPLE));
	s -> n_vectors = sample_size;
	s -> data = (DATUM **) malloc ((*s).n_vectors * sizeof(DATUM *));
	return s;

}


/*
.. cpp:function:: extern void sample_free(SAMPLE *s);

Free up the memory associated with a ``SAMPLE`` object.

Parameters
----------
s : ``SAMPLE *``
	The sample to be freed.
*/
extern void sample_free(SAMPLE *s) {

	if (s != NULL) {

		if ((*s).data != NULL) {
			unsigned long i;
			for (i = 0ul; i < (*s).n_vectors; i++) datum_free(s -> data[i]);
			free(s -> data);
		} else {}
		free(s);

	} else {}

}
