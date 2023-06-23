#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "crashtrace.h"

// only for this file
static char* crashtrace_path = NULL;

// local function declarations
void handler();

void crashtrace_init(char* path)
{
    crashtrace_path = path;
    signal(SIGSEGV, handler);
}

void handler()
{
    void* trace[16] = {};
    int count = backtrace(trace, 16);
    char** strings;

    strings = backtrace_symbols(trace, count);

    printf("Backtrace:\n----------\n");
    for (int i = 1; i < count; i++) {
        char* start = strchr(strings[i], '(');
        char* end = strchr(strings[i], ')');

        if (start && end) {
            *end = '\0';
        }

        char addr2line_cmd[256];
        sprintf(addr2line_cmd, "addr2line -e %s %s -pf", crashtrace_path, start + 1);
        FILE* addr2line_pipe = popen(addr2line_cmd, "r");

        char line_buf[256];
        while (fgets(line_buf, sizeof(line_buf), addr2line_pipe)) {
            char* end = strchr(line_buf, ' ');
            // replace \n with )
            char* replacement = strchr(end + 1, '\n');
            if (replacement) {
                *replacement = ')';
            }

            if (end) {
                *end = '\0';
            }
            if (!strncmp(line_buf, "??", 2)) {
                break;
            }
            printf("#%d  ", i - 1);
            printf("%s ", line_buf);

            printf("(%s\n", end + 4);
        }

        fclose(addr2line_pipe);
    }

    printf("----------\n");
    free(strings);

    exit(1);
}