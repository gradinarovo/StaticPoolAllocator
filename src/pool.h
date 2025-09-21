/**
 * @file        pool.h
 * @brief       Static Memory Pool Allocator Interface
 * @details     This header defines the interface for a static memory pool allocator.
 *              The allocator provides fixed-size block allocation from a pre-allocated
 *              memory pool with O(1) allocation and deallocation complexity.
 *
 * @note        This implementation is thread-unsafe. External synchronization is required
 *              if used in a multi-threaded environment.
 */

#ifndef POOL_H
#define POOL_H

#include "pool_types.h"

/**
 * @brief   Initialize the static memory pool
 * @param   p_handle   Pointer to the pool handle to be initialized
 * @return  None
 * @pre     Memory for the pool must be statically allocated
 * @post    Pool is ready for allocation requests
 * @note    If p_handle is NULL, the function returns without taking any action
 */
void pool_init(TPool_handle* p_handle);

/**
 * @brief   Allocate a block from the memory pool
 * @param   p_handle    Pointer to the pool handle
 * @return  Pointer to the allocated block, or NULL if no blocks available
 * @pre     Pool must be initialized
 * @post    If successful, a block of size POOL_BLOCK_SIZE is allocated
 */
void* pool_alloc(TPool_handle* p_handle);

/**
 * @brief   Free a previously allocated block back to the pool
 * @param   p_handle    Pointer to the pool handle
 * @param   p_block     Pointer to the block to free
 * @return  None
 * @pre     p_block must be a valid pointer previously returned by pool_alloc()
 * @post    The block is returned to the pool and can be reused
 */
void pool_free(TPool_handle* p_handle, void* p_block);

/**
 * @brief   Get the number of free blocks in the pool
 * @param   p_handle    Pointer to the pool handle
 * @return  Number of free blocks available
 * @pre     Pool must be initialized
 */
uint32 pool_get_free_count(const TPool_handle* p_handle);


#endif /* POOL_H */