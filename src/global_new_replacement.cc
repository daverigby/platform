/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 *     Copyright 2016 Couchbase, Inc
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

/* Global replacement of operators new and delete.
 *
 * By linking this file into a binary we globally replace new & delete.
 *
 * This allows us to track how much memory has been allocated from C++ code,
 * both in total and per ep-engine instance, by allowing interested parties to
 * register hook functions for new and delete.
 *
 * Usage:
 *
 * Link this file into /all/ binaries (executables, shared libraries and DLLs)
 * for which you want to have memory tracking for C++ allocations.
 * 
 * (Note: On most *ix-like platforms (Linux and OS X tested), it is sufficient
 *        to link this file into the main executable (e.g. `memcached`) and
 *        all shared libraries and dlopen'ed plugins will use these functions
 *        automagically. However this isn't the case on Windows/MSVC, where you
 *        need to link into all binaries for the overrider to take effect.
 *
 * Implementation:
 *
 * According to the C++11 spec
 * (http://en.cppreference.com/w/cpp/memory/new/operator_new#Global_replacements),
 * overriding base `operator new` and `operator delete` /shoul/d be sufficent
 * to handle these two allocation functions is sufficient to handle all
 * C++ allocations, however this isn't true on Windows/MSVC, where we alse
 * need to override the array forms.
 */

#include "config.h"

#include <platform/cb_malloc.h>

#include <cstdlib>
#include <new>

/* DJR TEMP - fix by 2016/10
 * The following symbols are marked as weak to simplify migrating existing
 * code. For example ep-engine has test code which defines its own operator
 * new/delete, and if we don't define these symbols as weak we'll get a linker
 * error when this patch is merged.
 */
#if defined(WIN32)
#  define WEAK_SYMBOL
#  define NOEXCEPT
#else
#  define WEAK_SYMBOL __attribute__((weak))
#  define NOEXCEPT noexcept
#endif

void* operator new(std::size_t count) WEAK_SYMBOL {
    void* result = cb_malloc(count);
    if (result == nullptr) {
        throw std::bad_alloc();
    }
    return result;
}

void operator delete(void* ptr) NOEXCEPT WEAK_SYMBOL {
    cb_free(ptr);
}

#if defined(WIN32)
// Also need to override the array forms for Windows/MSVC
void* operator new[](std::size_t count) {
    void* result = cb_malloc(count);
    if (result == nullptr)
    {
        throw std::bad_alloc();
    }
    return result;
}

void operator delete[](void *ptr) {
    cb_free(ptr);
}
#endif
