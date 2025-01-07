#include "exception_xmacro.h"

// Exception messages corresponding to the Exception_code enum
#define X(name, code, type) [code] = type,
const char *exception_messages[] = {
    EXCEPTION_TABLE
};
#undef X

/**
 * @brief Handles exceptions by printing detailed error information and exiting the program.
 * 
 * @param code The exception code (of type Exception_code).
 * @param file The name of the file where the exception occurred.
 * @param line The line number in the file where the exception occurred.
 * @param func The name of the function where the exception occurred.
 * @param format The custom message format string (like printf).
 * @param ... The additional arguments for the formatted message.
 */
void handle_exception(Exception_code code, const char *file, int line, const char *func, const char *format, ...) {
    const char *type = (code >= 0 && code <= 3) ? exception_messages[code] : "UNKNOWN_EXCEPTION";

    // Print exception type
    fprintf(stderr, "\n[EXCEPTION] %s\n", type);

    // Process the custom error message using variable arguments
    va_list args;
    va_start(args, format);
    fprintf(stderr, "Message       : ");
    vfprintf(stderr, format, args);
    va_end(args);

    // Print debug information
    fprintf(stderr, "Location      : File: %s, Line: %d, Function: %s\n", file, line, func);

    // Exit the program
    exit(EXIT_FAILURE);
}
