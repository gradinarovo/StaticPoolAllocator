/**
 * @file        test_pool.c
 * @brief       Test suite for static memory pool implementation
 * @details     This file contains unit tests for the memory pool functionality
 */

 #include <stdio.h>
 #include "pool.h"
 
 /* Test case counter */
 static uint32 test_count = 0;
 static uint32 test_passed = 0;
 
 /**
  * @brief Helper macro for test assertions
  */
 #define TEST_ASSERT(condition) \
     do { \
         test_count++; \
         if (condition) { \
             test_passed++; \
             printf("Test %u: PASSED\n", test_count); \
         } else { \
             printf("Test %u: FAILED at %s:%d\n", test_count, __FILE__, __LINE__); \
         } \
     } while(0)
 
 /* Test function declarations */
 static void test_pool_init(void);
 static void test_single_allocation(void);
 static void test_multiple_allocations(void);
 static void test_free_and_reuse(void);
 static void test_null_handling(void);
 static void test_boundary_conditions(void);
 
 /* Test pool handle */
 static TPool_handle test_pool;
 
 /**
  * @brief Run all tests
  */
 void run_all_tests(void)
 {
     printf("Starting memory pool tests...\n\n");
     
     test_pool_init();
     test_single_allocation();
     test_multiple_allocations();
     test_free_and_reuse();
     test_null_handling();
     test_boundary_conditions();
     
     printf("\nTest summary: %u/%u tests passed\n", test_passed, test_count);
 }
 
 /**
  * @brief Test pool initialization
  */
 static void test_pool_init(void)
 {
     /* Test with NULL handle */
     pool_init(NULL_PTR);
     
     /* Test valid initialization */
     pool_init(&test_pool);
     TEST_ASSERT(pool_get_free_count(&test_pool) == POOL_NUM_BLOCKS);
 }
 
 /**
  * @brief Test single block allocation
  */
 static void test_single_allocation(void)
 {
    /* Allocate a block and verify it's not NULL */
    uint32* block = (uint32*)pool_alloc(&test_pool);
    TEST_ASSERT(block != NULL);
    
    /* Verify the block is within the pool memory */
    TEST_ASSERT((uint8*)block >= (uint8*)&test_pool);
    TEST_ASSERT((uint8*)block < (uint8*)(&test_pool + 1));
    
    /* Write test data to the block */
    *block = 0xDEADBEEF;
    
    /* Verify the data was written correctly */
    TEST_ASSERT(*block == 0xDEADBEEF);
    
    /* Free the block */
    pool_free(&test_pool, block);
    
    /* Verify the block is still accessible but marked as free */
    uint32* new_block = (uint32*)pool_alloc(&test_pool);
    TEST_ASSERT(new_block == block);  /* Should get the same block back */
    
    /* Clean up */
    pool_free(&test_pool, new_block);
    TEST_ASSERT(pool_get_free_count(&test_pool) == POOL_NUM_BLOCKS);
 }
 
 /**
  * @brief Test multiple allocations
  */
 static void test_multiple_allocations(void)
 {
    /* Define a test pattern */
    const uint32 pattern = 0xABCD1234;
    const uint32 num_blocks = POOL_NUM_BLOCKS;
    uint32* blocks[num_blocks];
    
    /* Allocate all blocks and write unique data to each */
    for (uint32 i = 0; i < num_blocks; i++) {
        blocks[i] = (uint32*)pool_alloc(&test_pool);
        TEST_ASSERT(blocks[i] != NULL);
        
        /* Write a unique value to each block */
        *blocks[i] = pattern + i;
    }
    
    /* Next allocation should fail */
    TEST_ASSERT(pool_alloc(&test_pool) == NULL);
    
    /* Verify all blocks contain the correct data */
    for (uint32 i = 0; i < num_blocks; i++) {
        TEST_ASSERT(*blocks[i] == (pattern + i));
    }
    
    /* Free all blocks */
    for (uint32 i = 0; i < num_blocks; i++) {
        pool_free(&test_pool, blocks[i]);
    }
    
    /* Now we should be able to allocate again */
    for (uint32 i = 0; i < num_blocks; i++) {
        blocks[i] = (uint32*)pool_alloc(&test_pool);
        TEST_ASSERT(blocks[i] != NULL);
        
        /* Write a different pattern */
        *blocks[i] = ~(pattern + i);
    }
    
    /* Verify the new pattern */
    for (uint32 i = 0; i < num_blocks; i++) {
        TEST_ASSERT(*blocks[i] == ~(pattern + i));
    }
    
    /* Clean up */
    for (uint32 i = 0; i < num_blocks; i++) {
        pool_free(&test_pool, blocks[i]);
    }
 }
 
 /**
  * @brief Test free and reuse of blocks
  */
 static void test_free_and_reuse(void)
 {
     void* block1 = pool_alloc(&test_pool);
     void* block2 = pool_alloc(&test_pool);
     
     TEST_ASSERT(block1 != NULL_PTR);
     TEST_ASSERT(block2 != NULL_PTR);
     TEST_ASSERT(block1 != block2);
     
     /* Free first block and allocate again */
     pool_free(&test_pool, block1);
     void* block3 = pool_alloc(&test_pool);
     TEST_ASSERT(block3 != NULL_PTR);
     
     /* Clean up */
     pool_free(&test_pool, block2);
     pool_free(&test_pool, block3);
 }
 
 /**
  * @brief Test NULL parameter handling
  */
 static void test_null_handling(void)
 {
     /* Test with NULL handle */
     TEST_ASSERT(pool_alloc(NULL_PTR) == NULL_PTR);
     pool_free(NULL_PTR, NULL_PTR);
     
     /* Test free with NULL block */
     void* block = pool_alloc(&test_pool);
     TEST_ASSERT(block != NULL_PTR);
     pool_free(&test_pool, NULL_PTR);  /* Should not crash */
     pool_free(NULL_PTR, block);       /* Should not crash */
     pool_free(&test_pool, block);     /* Proper cleanup */
 }
 
 /**
  * @brief Test boundary conditions
  */
 static void test_boundary_conditions(void)
 {
     /* Test with invalid block pointer */
     uint8 dummy;
     pool_free(&test_pool, &dummy);  /* Should not crash */
     
     /* Test with pointer just before pool */
     uint8* before_pool = (uint8*)&test_pool - 1;
     pool_free(&test_pool, before_pool);  /* Should not crash */
     
     /* Test with pointer just after pool */
     uint8* after_pool = (uint8*)&test_pool + sizeof(TPool_handle);
     pool_free(&test_pool, after_pool);  /* Should not crash */
 }