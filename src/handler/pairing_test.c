/*****************************************************************************
 *   Ledger App Boilerplate.
 *   (c) 2020 Ledger SAS.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *****************************************************************************/

#include <stdint.h>  // uint*_t
#include <limits.h>  // UINT8_MAX
#include <assert.h>  // _Static_assert

#include "io.h"
#include "buffer.h"

#include "get_version.h"
#include "globals.h"
#include "constants.h"
#include "sw.h"
#include "types.h"
#include <ox_bn.h>
// #include <time.h>

const uint8_t bn254_p_raw[32] = {
    0x30, 0x64, 0x4e, 0x72, 0xe1, 0x31, 0xa0, 0x29, 0xb8, 0x50, 0x45, 0xb6, 0x81, 0x81, 0x58, 0x5d, 0x97, 0x81, 0x6a, 0x91, 0x68, 0x71, 0xca, 0x8d, 0x3c, 0x20, 0x8c, 0x16, 0xd8, 0x7c, 0xfd, 0x47
};
const uint8_t one_raw[] = { 0x01 };
const uint8_t two_raw[] = { 0x02 };

int handler_pairing_test() {
    cx_err_t err;
    cx_bn_t p, x, y, z, two;

    if ((err = cx_bn_lock(32, 0))) goto err;
    if ((err = cx_bn_alloc_init(&p, 32, bn254_p_raw, sizeof(bn254_p_raw)))) goto err;

    if ((err = cx_bn_alloc(&x, 256))) goto err;
    if ((err = cx_bn_rng(x, p))) goto err;

    if ((err = cx_bn_alloc(&y, 256))) goto err;
    if ((err = cx_bn_rng(y, p))) goto err;

    if ((err = cx_bn_alloc_init(&z, 32, one_raw, sizeof(one_raw)))) goto err;
    if ((err = cx_bn_alloc_init(&two, 32, two_raw, sizeof(two_raw)))) goto err;

    // clock_t begin = clock();
    for (uint8_t k = 0; k < 1; k++) {
        for (uint32_t i = 0; i < 2 + 256; i++) {
            for (uint32_t j = 0; j < 39; j++) {
                if ((err = cx_bn_mod_mul(x, x, y, p))) goto err;
            }
            for (uint32_t j = 0; j < 12; j++) {
                if ((err = cx_bn_mod_pow_bn(x, x, two, p))) goto err;
            }
        }
        for (uint32_t i = 0; i < 256; i++) {
            for (uint32_t j = 0; j < 23; j++) {
                if ((err = cx_bn_mod_mul(x, x, y, p))) goto err;
            }
            for (uint32_t j = 0; j < 12; j++) {
                if ((err = cx_bn_mod_pow_bn(x, x, two, p))) goto err;
            }
        }
    }
    // clock_t end = clock();

    cx_bn_unlock();

    // double duration = ((end - begin) / CLOCKS_PER_SEC) * 1000;
    uint32_t ms = 1234; //(uint32_t)duration;

    return io_send_response_pointer(
        (const uint8_t *) &ms,
        sizeof(uint32_t),
        SW_OK);

err:
    cx_bn_unlock();
    return io_send_response_pointer(
        (const uint8_t *) &err,
        sizeof(err),
        SW_DENY);
}
