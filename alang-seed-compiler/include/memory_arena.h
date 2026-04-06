#pragma once

#include <cstddef>
#include <vector>
#include <new>
#include <cassert>

namespace alang {
        
    class Arena
    {
    public:

        Arena();

        void* allocate(size_t size, size_t alignment);

        template<typename T, typename... Args>
        T* alloc(Args&&... args);

        void reset();
        
        ~Arena();

    private:

        static constexpr size_t BLOCK_SIZE = 4096;

        struct Block
        {
            char* memory;
            size_t used;
            size_t size;
        };

        std::vector<Block> blocks;

        void new_block(size_t min_size);
    };

    template<typename T, typename... Args>
    T* Arena::alloc(Args&&... args) 
    {
        void* mem = allocate(sizeof(T), alignof(T));
        return new (mem) T(std::forward<Args>(args)...);
    }

}