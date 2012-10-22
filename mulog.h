/* Copyright 2011 Kyle Dassoff. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY KYLE DASSOFF ''AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MULOG_H_
#define _MULOG_H_

#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The implementation of a mulog logger
 * Client will hold a pointer to one of these to use a logger
 */
struct mulog_t;
typedef struct mulog_t mulog_t;
typedef struct mulog_t *mulog_ref;

/* Indicates different logger types */
enum mulog_type {
    mulog_t_file,       // outputs to a C file handle
    mulog_t_con,        // outputs to the terminal/console, optionally with color
    mulog_t_split,      // sends messages to two different mulog objects
    mulog_t_dummy       // no-op mulog object
};
typedef enum mulog_type mulog_type;

/* Indicates the different logging levels */
enum mulog_level {
	mulog_l_debug,
	mulog_l_info,
	mulog_l_warning,
	mulog_l_error
};
typedef enum mulog_level mulog_level;

/* Controls output formatting of time */
enum mulog_timefmt {
    mulog_tm_long,      // strftime locale-dependent in localtime "%c %Z"
    mulog_tm_short,     // strftime locale-dependent in localtime "%x %X %Z"
    mulog_tm_fixed,     // strftime locale-independent in UTC time "%Y-%m-%d %H:%M:%S"
    mulog_tm_na         // returned by mulog_query_timefmt for split loggers
};
typedef enum mulog_timefmt mulog_timefmt;

enum mulog_status {
    mulog_ok,           // indicates operation success
    mulog_err_type,     // indicates an operation performed on an invalid logger type
    mulog_err_inval     // indicates an invalid parameter
};
typedef enum mulog_status mulog_status;

/* ==================
 * Creation functions
 * ==================
 */

/* Create a mulog logger that outputs to file handle f using time format tm
 * If with_debug != 0, debugging messages will be logged
 * otherwise, calls to mulog_debug will be silently ignored
 * Useful to allow a program to decide its verbosity level at runtime
 */
mulog_status mulog_create_file(mulog_ref *l, FILE *f, mulog_timefmt timefmt, int with_debug);

/* Create a mulog logger that outputs to the console with time format tm
 * with_debug has the same effect as above
 * with_color controls whether or not color output is used on Unix and Win32 consoles
 */
mulog_status mulog_create_con(mulog_ref *l, mulog_timefmt timefmt, int with_debug, int with_color);

/* Create a mulog object that splits out to the two given mulog loggers
 * The sink loggers use their respective time formats, debug switches, and color switches
 * Behavior is undefined if either sink logger is destroyed when this logger type is
 * created or used with the messaging functions
 */
mulog_status mulog_create_split(mulog_ref *l, mulog_ref left, mulog_ref right);

/* Creates a dummy mulog object that discards all messages */
mulog_status mulog_create_dummy(mulog_ref *l);

/* ===================
 * Messaging functions
 * ===================
 */

/* Basic log function, with no formatting */
mulog_status mulog_append(mulog_ref l, const char * str, size_t len);

/* The signature of each of the following is similar to fprintf or vfprintf, but with the FILE handle replaced by a mulog_ref */
/* Outputs an error message to the given logger */
void mulog_verr(mulog_ref l, const char * str, va_list va);
void mulog_err(mulog_ref l, const char * str, ...);

/* Outputs a warning message to the given logger */
void mulog_vwarn(mulog_ref l, const char * str, va_list va);
void mulog_warn(mulog_ref l, const char * str, ...);

/* Outputs an informational message to the given logger */
void mulog_vinfo(mulog_ref l, const char * str, va_list va);
void mulog_info(mulog_ref l, const char * str, ...);

/* Outputs a debugging/verbose informational message to the given logger
 * if and only if the logger's with_debug flag is set
 * (in the case of a spliting logger, it depends of the with_debug flag of the sink loggers)
 */
void mulog_vdbg(mulog_ref l, const char* str, va_list va);
void mulog_dbg(mulog_ref l, const char* str, ...);

/* =======================================
 * Logger query and modification functions
 * =======================================
 */

/* Returns the type of the logger */
mulog_type mulog_get_type(mulog_ref l);

/* Returns the file handle used by a file logger, or NULL for other types of loggers */
FILE *mulog_get_file(mulog_ref l);
/* Replaces the file handle used by a file logger */
mulog_status mulog_set_file(mulog_ref l, FILE *f);

/* Returns the value of the with_debug flag (0 -- off, 1 -- on) of a file or con logger,
 * or -1 for a dummy or split logger
 */
int mulog_get_with_debug(mulog_ref l);
/* Sets the with_debug flag of a file or con logger */
mulog_status mulog_set_with_debug(mulog_ref l, int with_debug);

/* Returns the value of the with_color flag (0 -- off, 1 -- on) of a con logger,
 * or -1 for other logger types
 */
int mulog_get_with_color(mulog_ref l);
/* Sets the with_color flag of a con logger */
mulog_status mulog_set_with_color(mulog_ref l, int with_color);

/* Returns the value of the timefmt flag */
mulog_timefmt mulog_get_timefmt(mulog_ref l);
/* Sets the value of the timefmt flag */
mulog_status mulog_set_timefmt(mulog_ref l, mulog_timefmt timefmt);

/* Returns the left or right sink logger (respectively) of a sink logger, or NULL for other types */
mulog_ref mulog_get_left(mulog_ref l);
mulog_ref mulog_get_right(mulog_ref l);
/* Sets the left or right sink logger (respectively) of a sink logger */
mulog_status mulog_set_left(mulog_ref l, mulog_ref left);
mulog_status mulog_set_right(mulog_ref l, mulog_ref right);

/* ====================
 * Destruction function
 * ====================
 */

/* Destroys a logger object */
void mulog_destroy(mulog_ref l);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // header guard
