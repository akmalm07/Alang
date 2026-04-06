#include "memory_arena.h"


namespace alang {
    
    Arena::Arena()
    {
        new_block(BLOCK_SIZE);
    }

    void* Arena::allocate(size_t size, size_t alignment)
    {
        Block& block = blocks.back();

        size_t current = reinterpret_cast<size_t>(block.memory + block.used);
        size_t aligned = (current + alignment - 1) & ~(alignment - 1);

        size_t offset = aligned - reinterpret_cast<size_t>(block.memory);

        if (offset + size > block.size)
        {
            new_block(size + alignment);
            return allocate(size, alignment);
        }

        block.used = offset + size;

        return block.memory + offset;
    }

    void Arena::new_block(size_t min_size)
    {
        size_t size = (min_size > BLOCK_SIZE) ? min_size : BLOCK_SIZE;

        Block block;
        block.memory = static_cast<char*>(::operator new(size));
        block.used = 0;
        block.size = size;

        blocks.push_back(block);
    }

    void Arena::reset()
    {
        for (auto& b : blocks)
            ::operator delete(b.memory);
        blocks.clear();
        new_block(BLOCK_SIZE);
	}

    Arena::~Arena()
    {
        for (auto& b : blocks)
            ::operator delete(b.memory);
    }

}