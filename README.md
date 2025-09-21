# Static Memory Pool Allocator

A lightweight, static memory pool allocator implementation in C, designed for embedded systems and other memory-constrained environments.

## Features

- Fixed-size block allocation with O(1) complexity
- Thread-unsafe (external synchronization required for multi-threaded use)
- Configurable pool size and block size
- Comprehensive test suite
- Memory-efficient implementation

## Table of Contents

- [Requirements](#requirements)
- [Building](#building)
- [Usage](#usage)
- [Configuration](#configuration)
- [API Reference](#api-reference)
- [Examples](#examples)
- [Running Tests](#running-tests)
- [License](#license)

## Requirements

- C compiler (GCC, Clang, or compatible)
- Make
- Git (for cloning the repository)

## Building

```bash
# Clone the repository
git clone https://github.com/gradinarovo/StaticPoolAllocator.git
cd StaticPoolAllocator

# Build the project
mingw32-make  # On Windows with MinGW
# or
make          # On Linux/macOS
```

## Usage

1. Include the header file:
   ```c
   #include "pool.h"
   ```

2. Initialize the pool:
   ```c
   TPool_handle my_pool;
   pool_init(&my_pool);
   ```

3. Allocate memory:
   ```c
   void* block = pool_alloc(&my_pool);
   if (block != NULL) {
       // Use the allocated block
       // ...
       
       // Free the block when done
       pool_free(&my_pool, block);
   }
   ```

## Configuration

Edit `cfg/pool_cfg.h` to configure:
- `POOL_NUM_BLOCKS`: Number of blocks in the pool
- `POOL_BLOCK_SIZE`: Size of each block in bytes
- `POOL_ALIGNMENT`: Memory alignment for blocks

## API Reference

### `void pool_init(TPool_handle* p_handle)`
Initialize the memory pool.

### `void* pool_alloc(TPool_handle* p_handle)`
Allocate a block from the pool. Returns `NULL` if no blocks are available.

### `void pool_free(TPool_handle* p_handle, void* p_block)`
Free a previously allocated block.

### `uint32 pool_get_free_count(const TPool_handle* p_handle)`
Get the number of free blocks available in the pool.

## Examples

### Basic Usage

```c
#include "pool.h"

int main() {
    TPool_handle pool;
    pool_init(&pool);
    
    // Allocate and use memory
    int* numbers = (int*)pool_alloc(&pool);
    if (numbers != NULL) {
        for (int i = 0; i < 10; i++) {
            numbers[i] = i * i;
        }
        
        // Free the memory when done
        pool_free(&pool, numbers);
    }
    
    return 0;
}
```

## Running Tests

The test suite verifies the functionality of the memory pool:

```bash
mingw32-make run  # On Windows with MinGW
# or
make run          # On Linux/macOS
```

## License

```
   Copyright 2025 Ivan Mateev

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
