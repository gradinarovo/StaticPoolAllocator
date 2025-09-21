/**
 * @file        pool.c
 * @brief       Static Memory Pool Implementation
 * @details     This file contains the implementation of a static memory pool
 *              with fixed-size block allocation.
 */

 #include "pool.h"
 #include "helper_routines.h"
 
 /**
 * @brief Calculate the number of bytes needed for the allocation bitmap
 * @details The bitmap uses 1 bit per block to track allocation status (1 = allocated, 0 = free).
 *          This macro calculates the minimum number of bytes needed to store the bitmap.
 *          The formula (POOL_NUM_BLOCKS + 7) / 8 ensures we have enough bits by:
 *          1. Adding 7 to round up to the next full byte (since integer division truncates)
 *          2. Dividing by 8 to convert bits to bytes
 *          Example: For 10 blocks: (10 + 7) / 8 = 17/8 = 2.125 → 2 bytes (16 bits)
 *                   For 16 blocks: (16 + 7) / 8 = 23/8 = 2.875 → 2 bytes (16 bits)
 */
#define BITMAP_BYTES ((POOL_NUM_BLOCKS + 7) / 8)
 
/**
 * @brief Set a specific bit in the allocation bitmap
 * @param bitmap Pointer to the bitmap array
 * @param index  Bit index to set (0-based)
 * 
 * @details This function sets the bit at the specified index in the bitmap to 1,
 *          indicating that the corresponding memory block is allocated.
 *          The function calculates the byte containing the target bit and the bit's
 *          position within that byte, then performs a bitwise OR operation to set it.
 * 
 * @note  - The bitmap uses 1 to indicate an allocated block
 *        - No bounds checking is performed on the index
 */
 static void set_bit(uint8* bitmap, uint32 index) 
 {
    uint32 byte_index = index / 8;   /* Find which byte contains the bit */
    uint32 bit_offset = index % 8;   /* Find the bit's position within the byte */
    bitmap[byte_index] |= (1U << bit_offset);  /* Set the bit using bitwise OR */
}
 
/**
 * @brief Clear a specific bit in the allocation bitmap
 * @param bitmap Pointer to the bitmap array
 * @param index  Bit index to clear (0-based)
 * 
 * @details This function clears the bit at the specified index in the bitmap to 0,
 *          indicating that the corresponding memory block is free.
 *          The function calculates the byte containing the target bit and the bit's
 *          position within that byte, then performs a bitwise AND with the inverse
 *          of the bitmask to clear it.
 * 
 * @note  - The bitmap uses 0 to indicate a free block
 *        - No bounds checking is performed on the index
 */
 static void clear_bit(uint8* bitmap, uint32 index)
{
    uint32 byte_index = index / 8;                   /* Find which byte contains the bit */
    uint32 bit_offset = index % 8;                   /* Find the bit's position within the byte */
    uint8  mask = (uint8)(1U << bit_offset);         /* Create mask for target bit */
    bitmap[byte_index] &= ~mask;                     /* Clear the bit using bitwise AND with inverted mask */
}
 
/**
 * @brief Test if a specific bit in the allocation bitmap is set
 * @param bitmap Pointer to the bitmap array
 * @param index  Bit index to test (0-based)
 * @return uint8 Returns 1 if the bit is set (allocated), 0 if clear (free)
 * 
 * @details This function checks the value of the bit at the specified index
 *          in the bitmap. The function calculates the byte containing the target
 *          bit and the bit's position within that byte, then extracts and returns
 *          the bit's value.
 * 
 * @note  - The bitmap uses 1 to indicate an allocated block
 *        - No bounds checking is performed on the index
 *        - The return value is always 0 or 1
 */
 static uint8 test_bit(const uint8* bitmap, uint32 index) 
 {
    uint32 byte_index = index / 8;                   /* Find which byte contains the bit */
    uint32 bit_offset = index % 8;                   /* Find the bit's position within the byte */
    return (bitmap[byte_index] >> bit_offset) & 1U;  /* Return the bit value (0 or 1) */
}
 
 /**
 * @brief Find the first free block in the bitmap
 * 
 * @param bitmap Pointer to the bitmap array where each bit represents a block
 * @param num_blocks Total number of blocks to check in the bitmap
 * @return int32 Index of the first free block (0 to num_blocks-1) if found,
 *         -1 if no free blocks are available
 * 
 * @note  - The function scans the bitmap from the start until it finds
 *           the first clear bit (0) indicating a free block
 *        - The search is linear and stops at the first available block
 *        - Thread safety must be handled by the caller if used in a multi-threaded context
 */
 static sint32 find_first_free(const uint8* bitmap, uint32 num_blocks) 
 {
     uint32 i;
     for (i = 0U; i < num_blocks; i++) 
     {
         if (0U == test_bit(bitmap, i)) 
         {
             return (sint32)i;
         }
     }

     return -1;  /* No free block found */
 }
 
/**
 * @brief Initialize the static memory pool
 * 
 * @param p_handle Pointer to the pool handle structure to be initialized
 * 
 * @note  - The function initializes all bytes of the pool handle to zero
 *        - This includes both the bitmap and the memory pool itself
 *        - If p_handle is NULL, the function returns without taking any action
 *        - This function should be called before any other pool operations
 */
 void pool_init(TPool_handle* p_handle) 
 {
     /* Check for NULL pointer */
     if (NULL_PTR != p_handle)
     {
         /* Initialize all bytes of the pool handle to zero */
         mem_set(p_handle, 0U, sizeof(TPool_handle));
     }
 }
 
/**
 * @brief Allocate a block from the memory pool
 * 
 * @param p_handle Pointer to the initialized pool handle
 * @return void*   Pointer to the allocated block, or NULL if allocation fails
 * 
 * @note  - Returns NULL if p_handle is NULL or no free blocks are available
 *        - The allocated block is marked as used in the bitmap
 *        - The memory content is not initialized
 *        - Thread safety must be handled by the caller if used in a multi-threaded context
 */
 void* pool_alloc(TPool_handle* p_handle) 
 {
     sint32 block_index;
     
     /* Check for NULL pointer */
     if (NULL_PTR == p_handle)
     {
         return NULL_PTR;
     }
     
     /* Find the first available block */
     block_index = find_first_free(p_handle->bitmap, POOL_NUM_BLOCKS);
     
     /* Check if a free block was found */
     if (block_index < 0)
     {
         return NULL_PTR;  /* No free blocks available */
     }
     
     /* Mark the block as used */
     set_bit(p_handle->bitmap, (uint32)block_index);
     
     /* Return pointer to the allocated block */
     return &p_handle->memory[block_index * POOL_BLOCK_SIZE];
 }
 
 /**
 * @brief Free a previously allocated block back to the memory pool
 * 
 * @param p_handle Pointer to the initialized pool handle
 * @param p_block  Pointer to the memory block to be freed
 * 
 * @note  - If p_handle or p_block is NULL, the function returns immediately
 *        - If p_block is not a valid pointer from this pool, the function returns
 *        - The function is idempotent (safe to call multiple times on the same block)
 *        - Only blocks that are currently allocated will be marked as free
 *        - Thread safety must be handled by the caller if used in a multi-threaded context
 *        - The function validates:
 *          - Input parameters are not NULL
 *          - The pointer is within the pool's memory range
 *          - The pointer is properly aligned
 *          - The block index is within valid range
 */
void pool_free(TPool_handle* p_handle, void* p_block) 
{
    uint32 block_index;
    uint8* pool_start;
    uint8* pool_end;
    
    /* Check for NULL pointers */
    if (NULL_PTR == p_handle || NULL_PTR == p_block)
    {
        return;
    }
    
    /* Calculate pool memory boundaries */
    pool_start = p_handle->memory;
    pool_end = pool_start + (POOL_NUM_BLOCKS * POOL_BLOCK_SIZE);
    
    /* Check if pointer is within pool bounds */
    if (p_block < (void*)pool_start || p_block >= (void*)pool_end) 
    {
        return;  /* Invalid pointer - not from this pool */
    }
    
    /* Calculate block index */
    block_index = ((uint8*)p_block - pool_start) / POOL_BLOCK_SIZE;
    
    /* Validate alignment and block boundary */
    if (((uint8*)p_block - pool_start) % POOL_BLOCK_SIZE != 0) 
    {
        return;  /* Not aligned to block boundary */
    }
    
    /* Validate block index */
    if (block_index >= POOL_NUM_BLOCKS)
    {
        return;  /* Block index out of range */
    }
    
    /* Only clear if it was allocated (defensive programming) */
    if (test_bit(p_handle->bitmap, block_index))
    {
        clear_bit(p_handle->bitmap, block_index);
    }
}
 
/**
 * @brief Get the number of free blocks in the memory pool
 * 
 * @param p_handle Pointer to the initialized pool handle
 * @return uint32  Number of free blocks available for allocation
 * 
 * @note  - Returns 0 if p_handle is NULL
 *        - The function scans the entire bitmap to count free blocks
 *        - The returned count represents the number of blocks that can be allocated
 *        - Time complexity is O(n) where n is POOL_NUM_BLOCKS
 *        - Thread safety must be handled by the caller if used in a multi-threaded context
 * 
 * @example
 *   TPool_handle* pool;
 *   pool_init(&pool);
 *   uint32 free = pool_get_free_count(pool);  // Returns POOL_NUM_BLOCKS
 */
 uint32 pool_get_free_count(const TPool_handle* p_handle) 
 {
     uint32 i;
     uint32 free_count = 0U;
     
     /* Check for NULL pointer */
     if (NULL_PTR == p_handle)
     {
         return 0U;
     }
     
     /* Count all free blocks */
     for (i = 0U; i < POOL_NUM_BLOCKS; i++)
     {
         if (!test_bit(p_handle->bitmap, i))
         {
             free_count++;
         }
     }
     
     return free_count;
 }