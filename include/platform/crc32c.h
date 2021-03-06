/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 *     Copyright 2015 Couchbase, Inc
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

//
// Generate a CRC-32C (Castagnolia)
// CRC polynomial of 0x1EDC6F41
//
// When available a hardware assisted function is used for increased performance.
//
// This module provides one function:
//
// uint32_t crc32c(const uint8_t* bug, size_t len, uint32_t crc_in)
//
//

#pragma once
#include <stdint.h>
#include <stddef.h>
#include <platform/visibility.h>

//
// This module requires X86 for the HW assisted version of the function.
// For now this #error is here because there's been no requirement for
// a portable/non-X86 version of this function in Couchbase.
// To fix will require refactoring to hide the X86 dependencies when
// built on another platform.
//
#if !defined(__x86_64__) && !defined(_M_X64) && !defined(_M_IX86)
#error "crc32c requires X86 SSE4.2 for hardware acceleration"
#endif

#ifdef CRC32C_UNIT_TEST
#undef PLATFORM_PUBLIC_API
#define PLATFORM_PUBLIC_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

PLATFORM_PUBLIC_API
uint32_t crc32c (const uint8_t* buf, size_t len, uint32_t crc_in);

#ifdef __cplusplus
}
#endif
