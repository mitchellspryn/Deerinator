/* @note	Define MESSAGES_DEBUG_MODE_ON to enable debug messages.
 */
#ifndef MESSAGES_H
#define MESSAGES_H

//#define MESSAGES_DEBUG_MODE_ON

#include <stdio.h>




#ifdef MESSAGES_DEBUG_MODE_ON
  /**
   * Display a debug message.
   *
   * @param[in]   ...     Parameters as you would provide to printf
   */
  #define DEBUG_MSG(...)		\
    fprintf(stderr, "%s: %d: DEBUG: ",  \
            __FILE__, __LINE__),	\
      fprintf(stderr, __VA_ARGS__),	\
      fprintf(stderr, "\n")
#else
  /**
   * Ignore debug messages.
   *
   * @param[in]   ...     *IGNORED*
   */
  #define DEBUG_MSG(...) do {} while (0)
#endif




/**
 * Display an error message.
 *
 * @param[in]   ...     Parameters as you would provide to printf
 */
#define ERROR_MSG(...) \
  fprintf(stderr, "%s: %d: error: ",		\
          __FILE__, __LINE__),			\
    fprintf(stderr, __VA_ARGS__),		\
    fprintf(stderr, "\n")



/**
 * Display a warning message.
 *
 * @param[in]   ...     Parameters as you would provide to printf
 */
#define WARN_MSG(...) \
  fprintf(stderr, "%s: %d: warning: ",		\
          __FILE__, __LINE__),                  \
    fprintf(stderr, __VA_ARGS__), 		\
    fprintf(stderr, "\n")



/**
 * Display a message.
 *
 * @param[in]   ...     Parameters as you would provide to printf
 */
#define MSG(...) \
  fprintf(stdout, __VA_ARGS__),			\
    fprintf(stdout, "\n")




#endif
