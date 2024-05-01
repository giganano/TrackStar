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

/* Number of maximum threads = N x number of active CPUs */
#define MAX_THREADS_CPU_RATIO 10u

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

/*
.. cpp:function:: inline long max_threads_allowed();

Determine the maximum number of allowed threads. TrackStar, by default, will
not allow any more than 10 times the maximum number of active CPUs. If users
wish to relax this limit, the value of 10 is ``#define``'d using the macro
``MAX_THREADS_CPU_RATIO`` in ``trackstar/core/src/multithread.h``.
Simply recompile and reinstall by running ``python -m pip install .`` from the
top-level repository after making this change.
*/
inline long max_threads_allowed(void) {
	/* _SC_NPROCESSORS_ONLN gets number of active CPUs from unistd.h */
	return MAX_THREADS_CPU_RATIO * sysconf(_SC_NPROCESSORS_ONLN);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MULTITHREAD_H */
