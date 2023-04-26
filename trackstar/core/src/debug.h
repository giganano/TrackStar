/*
This file is part of the TrackStar package.
Copyright (C) 2023 James W. Johnson (giganano9@gmail.com)
License: MIT License. See LICENSE in top-level directory
at https://github.com/giganano/TrackStar.git.

Handles logging from TrackStar's backend for development and debugging purposes.
TrackStar adopts a conventional format in which there are six different levels
of verbosity for logging:

	1. *info*
		General information regrading the executed process.
	2. *trace*
		Individual function calls and the files in which they are implemented.
	3. *debug*
		Prints the functions being called, the files in which they are
		implemented, the line numbers calling the logging print statement,
		and variable states.
	4. *warning*
		Prints regardless of the user's logging level and whether or not
		they have ignored warnings in Python. Does not stop the program.
	5. *error*
		A state is reached in which TrackStar cannot continue the calculation.
		This exits the Python interpreter always.
	6. *fatal*
		A state is reached in which TrackStar cannot *safely* continue the
		calculation. This exits the Python interpreter always.

Verbose output from function in this header print to ``stderr``.
*/

#ifndef DEBUG_H
#define DEBUG_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
Failsafe: define the __func__ identifier if it is not already defined for
maximum portability.
*/
#if __STDC_VERSION__ < 199901L
	#if __GNUC__ >= 2
		#define __func__ __FUNCTION__
	#else
		#define __func__ "<unknown>"
	#endif /* __GNUC__ */
#endif /* __STDC_VERSION__ */

/* Different level of verbosity */
#define INFO 1u
#define TRACE 2u
#define DEBUG 3u

/* for printing errors and warning messages in red. */
#define RED "\033[0m"
#define BOLDRED "\033[1m\033[31m"
#define RESET "\033[0m"

/*
.. cpp:function:: logging_level();

Determine the depth of TrackStar's verbose logging by obtaining the integer
value of the environment variable ``TRACKSTAR_LOGGING_LEVEL``: 1 for *info*,
2 for *trace*, and *3* for debug.

.. note:: This function is ``#define``'d as a compile-time macro.
*/
#define logging_level() ({ \
	char *_loglevel = getenv("TRACKSTAR_LOGGING_LEVEL"); \
	_loglevel != NULL ? atoi(_loglevel) : 0; \
})


/*
.. cpp:function:: info_print(fmt, ...);

Prints a statement to ``stderr`` if and only if the logging level is equal to
1 (*info*).

.. note:: This function is ``#define``'d as a compile-time macro.

Parameters
----------
fmt : ``char *``
	A string formatter.
... : ``char *``
	Substrings for formatting into ``fmt``.

Usage: ``info_print("%s\n", "Some message to print.");``
*/
#define info_print(fmt, ...) \
	do { \
		if (logging_level() == INFO) { \
			fprintf(stderr, fmt, __VA_ARS__); \
		} else {} \
	} while (0)


/*
.. cpp:function:: trace_print()

Prints the name of the file and function that is being executed to stderr if
and only if the logging level is equal to 2 (*trace*).

.. note:: Thsi function is ``#define``'d as a compile-time macro.

Usage: ``trace_print();``
*/
#define trace_print() \
	do { \
		if (logging_level() == TRACE) { \
			fprintf(stderr, "%s:%d:%s()\n", __FILE__, __LINE__, __func__); \
		} else {} \
	} while (0)


/*
Print the value of variables to stderr if and only if the logging level is
equal to 3 (*debug*).

.. note:: This function is ``#define``'d as a compile-time macro.

Parameters
----------
fmt : ``char *``
	A string formatter.
... : ``char *``
	Substrings for formatting into ``fmt``.

Usage: ``debug_print("x = %e ; y = %e\n", x, y);``
*/
#define debug_print(fmt, ...) \
	do { \
		if (logging_level() == DEBUG) { \
			fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, \
				__func__, __VA_ARGS__); \
		} else {} \
	} while (0)


/*
.. cpp:function:: warning_print(fmt, ...);

Prints a warning message to the console and exits the current process, quitting
the Python interpreter. This runs regardless of the logging level and whether
or not the user has turned off Python warnings.

.. note:: This function is ``#define``'d as a compile-time macro.

.. attention::

	This message is intended for developers, so if this is raised in an end
	user's system, it should be interpreted as an issue within TrackStar.

Parameters
----------
fmt : ``char *``
	A string formatter.
... : ``char *``
	Substrings for formatting into ``fmt``.

Usage: ``warning_print("%s\n", "Some message to print.");``
*/
#define warning_print(fmt, ...) \
	do { \
		fprintf(stderr, RED"Warning: "RESET fmt, __VA_ARGS__); \
	} while (0)


/*
.. cpp:function:: error_print(fmt, ...);

Prints an error message to the console and exits the current process, quitting
the Python interpreter. This runs regardless of the logging level and whether
or not the user has turned off Python warnings.

.. note:: This function is ``#define``'d as a compile-time macro.

.. attention::

	This message is intended for developers, so if this is raised in an end
	user's system, it should be interpreted as an issue within TrackStar.

Parameters
----------
fmt : ``char *``
	A string formatter.
... : ``char *``
	Substrings for formatting into ``fmt``.

Usage: ``error_print("%s\n", "Some message to print.");``
*/
#define error_print(fmt, ...) \
	do { \
		fprintf(stderr, BOLDRED"Error!"RESET" %s:%d:%s(): " fmt, \
			__FILE__, __LINE__, __func__, __VA_ARGS__); \
		exit(1); \
	} while (0)


/*
.. cpp:function:: fatal_print(fmt, ...);

Prints a fatal message to the console and exits the current process, quitting
the Python interpreter. This runs regardless of the logging level and whether
or not the user has turned off Python warnings.

.. note:: This function is ``#define``'d as a compile-time macro.

.. attention::

	This message is intended for developers, so if this is raised in an end
	user's system, it should be interpreted as an issue within TrackStar.

Parameters
----------
fmt : ``char *``
	A string formatter.
... : ``char *``
	Substrings for formatting into ``fmt``.

Usage: ``fatal_print("%s\n", "Some message to print.");``
*/
#define fatal_print(fmt, ...) \
	do { \
		fprintf(stderr, BOLDRED"Fatal!"RESET" %s:%d:%s(): " fmt, \
			__FILE__, __LINE__, __func__, __VA_ARGS__); \
		exit(1); \
	} while (0)

#endif /* DEBUG_H */
