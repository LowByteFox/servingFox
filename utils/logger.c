#include "logger.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define COLOR_RED "\x1b[31m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_RESET "\x1b[0m"

void logError(const char* msg)
{
    int err = errno;
    char err_str[256];
    strerror_r(err, err_str, sizeof(err_str));
    printf("[%sFAIL%s] (%s): %s\n", COLOR_RED, COLOR_RESET, err_str, msg);
}

void logWarn(const char* msg)
{
    printf("[%sWARN%s]: %s\n", COLOR_YELLOW, COLOR_RESET, msg);
}

void logInfo(const char* msg)
{
    printf("[%sINFO%s]: %s\n", COLOR_BLUE, COLOR_RESET, msg);
}

void logSuccess(const char* msg)
{
    printf("[%sDONE%s]: %s\n", COLOR_GREEN, COLOR_RESET, msg);
}