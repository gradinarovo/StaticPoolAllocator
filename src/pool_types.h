/**
 * @file        pool_types.h
 * @brief       Internal type definitions for static memory pool
 * @details     This file contains internal type definitions for the static memory pool
 *              implementation. These types should not be accessed directly by users
 *              of the memory pool.
 */

#ifndef POOL_TYPES_H
#define POOL_TYPES_H

#include "std_types.h"
#include "pool_cfg.h"

/* Number of bits in a byte */
#define BITS_PER_BYTE 8U

/**
 * @brief   Memory pool handle structure
 * @details This structure contains the internal state of a memory pool.
 *          The actual memory and allocation bitmap are stored here.
 */
typedef struct pool_handle {
    uint8  memory[POOL_NUM_BLOCKS * POOL_BLOCK_SIZE];  /**< Raw memory pool */
    uint8  bitmap[(POOL_NUM_BLOCKS + (BITS_PER_BYTE - 1U)) / BITS_PER_BYTE];  /**< Allocation bitmap (1 bit per block) */
} TPool_handle;

#endif /* POOL_TYPES_H */