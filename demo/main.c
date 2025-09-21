/**
 * @file        main.c
 * @brief       Main application entry point
 * @details     This file contains the main function to run the memory pool tests
 */

 #include <stdio.h>
 #include "test_pool.h"
 
 /**
  * @brief Main function
  * @return int 0 on success, non-zero on failure
  */
 int main(void)
 {
     printf("Memory Pool Allocator Test Suite\n");
     printf("================================\n\n");
     
     /* Run all tests */
     run_all_tests();
     
     printf("\nTest execution complete.\n");
     return 0;
 }