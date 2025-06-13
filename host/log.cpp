#include "log.h"

static enum LOG_LEVEL current_log_level = FATAL;

void set_log_level(enum LOG_LEVEL level) {
    current_log_level = level;
}

enum LOG_LEVEL get_log_level() {
    return current_log_level;
}

#if not defined(CUSTOM_LOGGING)
_debug debug(std::cout);
_info info(std::cout);
_warning warning(std::cout);
_error error(std::cout);
#endif