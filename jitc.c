/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * jitc.c
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dlfcn.h>
#include "system.h"
#include "jitc.h"

/**
 * Needs:
 *   fork()
 *   execv()
 *   waitpid()
 *   WIFEXITED()
 *   WEXITSTATUS()
 *   dlopen()
 *   dlclose()
 *   dlsym()
 */

struct jitc {
    void *handle;
};

/**
 * Compiles a C program into a dynamically loadable module.
 *
 * input : the file pathname of the C program
 * output: the file pathname of the dynamically loadable module
 *
 * return: 0 on success, otherwise error
 */

int jitc_compile(const char *input, const char *output) {

    pid_t pid = fork();

    if (pid == -1) {
        TRACE("fork failed");
        return -1;
    }
    // child, compile gcc
    else if (pid == 0) { 
        const char *argv[] = {"gcc", "-shared", "-o", output, "-fpic", "-O3", input, NULL};

        execv("/usr/bin/gcc", argv);
        // if success, current process image is replaced, nothing is returned
        // if failed
        perror("execv failed");
        return -1;        
    }
    // parent, waitpid()
    else {
        int status;

        if (waitpid(pid, &status, 0) == -1) {
            TRACE("waitpid failed");
            return -1;
        }
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            TRACE("Child did not exit normally or exited with non-zero status"); 
            return -1;
        }
    }
    return 0;
}

/**
 * Loads a dynamically loadable module into the calling process' memory for
 * execution.
 *
 * pathname: the file pathname of the dynamically loadable module
 *
 * return: an opaque handle or NULL on error
 */

struct jitc *jitc_open(const char *pathname) {

    struct jitc *jitc = malloc(sizeof(struct jitc));
    if (!jitc) {
        TRACE("malloc failed");
        return NULL;
    }

    jitc->handle = dlopen(pathname, RTLD_LAZY);
    if (!jitc->handle) {
        TRACE(dlerror());
        FREE(jitc);
        return NULL;
    }

    return jitc;
}

/**
 * Unloads a previously loaded dynamically loadable module.
 *
 * jitc: an opaque handle previously obtained by calling jitc_open()
 *
 * Note: jitc may be NULL
 */

void jitc_close(struct jitc *jitc) {

    if (!jitc || !jitc->handle) {
        TRACE("jitc or handle is NULL");
        return;
    }

    dlclose(jitc->handle);
    free(jitc);
}

/**
 * Searches for a symbol in the dynamically loaded module associated with jitc.
 *
 * jitc: an opaque handle previously obtained by calling jitc_open()
 *
 * return: the memory address of the start of the symbol, or 0 on error
 */

long jitc_lookup(struct jitc *jitc, const char *symbol) {

    if (!jitc || !jitc->handle) {
        TRACE("jitc or handle is NULL");
        return 0;
    }

    void *address = dlsym(jitc->handle, symbol);
    if (!address) {
        TRACE(dlerror());
        return 0;
    }

    return (long) address;
}