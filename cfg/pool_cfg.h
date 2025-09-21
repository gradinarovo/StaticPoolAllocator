/**
 * @file        pool_cfg.h
 * @brief       Static Memory Pool Configuration
 * @details     Configuration header for static memory pool allocator module.
 *              This file contains user-configurable parameters for the memory pool.
 *
 * @attention   This file must be included by the application to configure
 *              the memory pool parameters before using the allocator.
 */

#ifndef POOL_CFG_H
#define POOL_CFG_H

/**
 * @brief   Size of each memory block in bytes
 * @details This defines the size of individual blocks in the memory pool.
 *          All allocations will be of this fixed size.
 */
#define POOL_BLOCK_SIZE        (32U)

/**
 * @brief   Total number of blocks in the memory pool
 * @details This defines the maximum number of blocks that can be allocated
 *          from the pool.
 */
#define POOL_NUM_BLOCKS        (4U)

#endif /* POOL_CFG_H */