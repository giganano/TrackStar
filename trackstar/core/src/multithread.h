/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at: https://github.com/giganano/TrackStar.git.
*/

#ifndef MULTITHREAD_H
#define MULTITHREAD_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(_OPENMP)
	#include <omp.h>
#endif
#include <unistd.h>

/*
.. cpp:function:: inline unsigned short multithreading_enabled();

Returns 1 if multithreading is enabled and 0 otherwise. Used in TrackStar's
python wrapping to determine whether or not multithreading is enabled by
linking with the OpenMP library at compile time.
*/
inline unsigned short multithreading_enabled(void) {
	#if defined(_OPENMP)
		return 1u;
	#else
		return 0u;
	#endif
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MULTITHREAD_H */
