#include "exception_xmacro.h"


#define X(name, code, type) [code] = type,
const char *exception_messages[] = {
    EXCEPTION_TABLE
};
#undef X


void handle_exception(Exception_code code, const char *file, int line, const char *func, const char *format, ...) {
    const char *type = (code >= 0 && code <= 3) ? exception_messages[code] : "UNKNOWN_EXCEPTION";

    
    fprintf(stderr, "[EXCEPTION] %s\n", type);

    
    va_list args;
    va_start(args, format);
    fprintf(stderr, "Message       : ");
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "Location      : File: %s, Line: %d, Function: %s\n", file, line, func);

    exit(EXIT_FAILURE);
}
