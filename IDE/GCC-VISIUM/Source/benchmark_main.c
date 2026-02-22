/* benchmark_main.c
 *
 * Copyright (C) 2006-2025 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfcrypt/benchmark/benchmark.h>
#include <stdio.h>

/*************************************************************************************
NOTE RR: With WOLFSSL_STATIC_MEMORY defined, wolfSSL/wolfCrypt uses static memory pools, 
not malloc, and RSA will request large chunks (big-int buffers). 
If the pools aren’t sized/initialized for those request types, 
RSA fails with "out of memory error".

Static memory pool has to be initialized using wc_LoadStaticMemory()
before calling Wolfcrypt tests.
*/
static unsigned char gHeap[256* 1024];
static WOLFSSL_HEAP_HINT* gHeapHint = NULL;
/**********************************************************************************/

#ifndef NO_CRYPT_BENCHMARK
typedef struct func_args {
    int    argc;
    char** argv;
    int    return_code;
} func_args;

static func_args args = { 0 } ;
#endif

int main(void)
{
    int ret;
#ifndef NO_CRYPT_BENCHMARK
    wolfCrypt_Init();
    ret = wc_LoadStaticMemory(&gHeapHint, gHeap, sizeof(gHeap), 0, 0);
    printf("\nBenchmark Test\n");
    benchmark_test(&args);
    ret = args.return_code;
    printf("Benchmark Test: Return code %d\n", ret);

    wolfCrypt_Cleanup();
#else
    ret = NOT_COMPILED_IN;
#endif
    return ret;
}
