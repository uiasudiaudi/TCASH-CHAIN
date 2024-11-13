// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "tcashio_setproctitle.h"

#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

/*
 * To change the process title in Linux and Solaris we have to set argv[1]
 * to NULL and to copy the title to the same place where the argv[0] points to.
 * However, argv[0] may be too small to hold a new title.  Fortunately, Linux
 * and Solaris store argv[] and environ[] one after another.  So we should
 * ensure that is the continuous memory and then we allocate the new memory
 * for environ[] and copy it.  After this we could use the memory starting
 * from argv[0] for our process title.
 */

extern char ** environ;
extern char ** tcashio_os_argv;

static char * tcashio_os_argv_last;

int tcashio_init_setproctitle() {
    char * p;
    int i = 0;
    uint32_t size = 0;

    for (i = 0; environ[i]; i++) {
        size += strlen(environ[i]) + 1;
    }

    p = (char *)malloc(size);
    if (p == NULL) {
        return tcashIO_PROCTITLE_ERROR;
    }

    tcashio_os_argv_last = tcashio_os_argv[0];

    for (i = 0; tcashio_os_argv[i]; i++) {
        if (tcashio_os_argv_last == tcashio_os_argv[i]) {
            tcashio_os_argv_last = tcashio_os_argv[i] + strlen(tcashio_os_argv[i]) + 1;
        }
    }

    for (i = 0; environ[i]; i++) {
        if (tcashio_os_argv_last == environ[i]) {
            size = strlen(environ[i]) + 1;
            tcashio_os_argv_last = environ[i] + size;

            strncpy(p, environ[i], size);
            environ[i] = (char *)p;
            p += size;
        }
    }

    tcashio_os_argv_last--;

    return tcashIO_PROCTITLE_OK;
}

void tcashio_setproctitle(const char * title) {
    tcashio_os_argv[1] = NULL;
    char new_title[1024];
    bzero(new_title, sizeof(new_title));
    sprintf(new_title, "%s%s", "tcashio: ", title);

    strncpy(tcashio_os_argv[0], new_title, tcashio_os_argv_last - tcashio_os_argv[0]);

#ifdef DEBUG
    printf("set title:%s\n", new_title);
#endif
}
