MuLog is a simple logging library written in C99.

It currently includes 4 types of loggers, all of which work with the same message functions.
    - File loggers write to a given file using the C standard library's FILE* handle (which must be opened/closed by
      client). Each logger may be configured to output or discard debug messages (from mulog_dbg())
    - Con[sole] loggers write to the stdout stream for mulog_dbg() (if debug messages are enabled) and mulog_info(), and
      to the stderr stream for mulog_warn() and mulog_err(). It may also be configured to send colorized output or not.
      The colors used are compiled in to the MuLog library, though it is simple to edit the implementations of the message
      functions to change which colors are used.
    - Split loggers forward the message calls to two target loggers. Other than which two loggers it uses, it has no
      configuration options. The target loggers keep their own configuration options (such as debug enabled/disabled, or
      colorized output). Any logger type may be set as a target logger, including other split loggers.
    - Dummy loggers simply discard their output. As an alternative, a mulog_ref variable (which is what is passed to all
      functions and is a pointer type) may be set to NULL. All mulog_functions (except for the create) functions check
      that the mulog_ref parameter is not NULL; if it is they just do nothing.

The mulog_create_XXX() functions create the above types of loggers and assign a handle to the given mulog_ref variable.
mulog_err(), mulog_warn(), mulog_info(), mulog_dbg(), send output to the given logger. They all have signatures of the form
(mulog_ref, const char*, ...) where the 1st parameter is the target logger, the 2nd parameter is a printf format string,
and the remaining parameters are the arguments to print to printf. I.e., the signature is identical to fprintf, but with the
FILE* handle replaced by a mulog_ref handle.

Three formats for outputting the message timestamp are currently supported and are listed and described in the mulog_timefmt
enum. One outputs times in UTC and is locale-independent; the other two use the current timezone and locale set within the
C standard library.