#include"mempool.hpp"

#include <iostream>
#include <csignal>

#define MAGIC 0xDEADBEEF

namespace memalloc
{

/**
 * Check if a block is free and of a correct size. 
 **/
mempool_block* mempool_chunk::check_block(mempool_block* block, std::size_t size)
{
   return ((block->m_free) && (block->m_size >= size)) ? block : nullptr;
}

/**
 * Find the first free block that is large enough,
 * which lies after the inputtet block.
 **/
mempool_block* mempool_chunk::find_block(mempool_block* block, std::size_t size)
{
   mempool_block* b = block;
   
   // find free block
   while (  b 
         && (  !b->m_free 
            || (b->m_size < size)
            )
         ) 
   {
      b = b->m_next;
   }

   return b;
}

/**
 * Find a free block block of at least size,
 * between block_from and block_to.
 **/
mempool_block* mempool_chunk::find_block(mempool_block* block_from, mempool_block* block_to, std::size_t size)
{
   mempool_block* b = block_from;
   // find free block
   while (  b 
         && (  !b->m_free 
            || (b->m_size < size)
            )
         ) 
   {
      b = b->m_next;
      if(b == block_to) return nullptr;
   }

   return b;
}

/**
 * Pad size, such that next block will fit with start of a "CACHELINE".
 **/
std::size_t mempool_chunk::padding(std::size_t size) const
{
   auto pad_diff = (size + sizeof(mempool_block))%CACHELINE;
   size += (CACHELINE - pad_diff);
   return size;
}

#ifdef CHECK_BLOCKS_REACHABLE
/**
 * Debug function to check whether a given block is reachable from the start block.
 **/
bool mempool_chunk::check_reachable_from_start_block(mempool_block* block) const
{
   mempool_block* b = m_start_block;
   while(b)
   {
      if(b == block) 
      {
         return true;
      }
      b = b->m_next;
   }

   std::cout << " START BLOCK : " << std::endl << *m_start_block << std::endl;
   return false;
}
#endif /* CHECK_BLOCKS_REACHABLE */

/**
 * Debug function to print current block "stack" from start block.
 **/
void mempool_chunk::print_stack() const
{
   mempool_block* b = m_start_block;
   while(b)
   {
      std::cout << b << std::endl;
      b = b->m_next;
   }
}

/**
 * Construct mempool_chunk of given size.
 **/
mempool_chunk::mempool_chunk(std::size_t size)
   :  m_chunk(new char[size])
{
   // initialize the first block
   mempool_block* new_block = reinterpret_cast<mempool_block*>( m_chunk.get() );
   if(!new_block) std::cout << " COULD NOT ALLOCATE NEW CHUNK " << std::endl;
   new_block->m_prev = nullptr;
   new_block->m_next = nullptr;
   new_block->m_free = true;
   new_block->m_size = size - sizeof(mempool_block);
   new_block->m_root = reinterpret_cast<void*>( m_chunk.get() );
#ifdef CHECK_MEMORY_CORRUPTION
   new_block->m_front_magic = MAGIC;
   new_block->m_middle_magic = MAGIC;
#endif /* CHECK_MEMORY_CORRUPTION */

   m_start_block = new_block;
   m_last_allocated_block = new_block;
   m_last_deallocated_block = new_block;
}

/**
 * Check if resource was allocated with this chunk.
 **/
bool mempool_chunk::same_root(void* resource)
{
   mempool_block* block = reinterpret_cast<mempool_block*>(reinterpret_cast<char*>(resource) - sizeof(mempool_block));
   return (block->m_root == m_start_block->m_root);
}

/**
 * Acquire a pointer from a mempool_chunk.
 **/
void* mempool_chunk::acquire(std::size_t size, void* hint)
{
   if(!size) return nullptr;

   // add padding
   size = padding(size);

#ifdef CHECK_BLOCKS_REACHABLE
   if(!check_reachable_from_start_block(m_last_allocated_block))
   {
      std::cout << " WARNING LAST ALLOCATED IS NOT REACABLE FROM START BLOCK " << std::endl;
      std::raise(SIGABRT);
   }
   if(!check_reachable_from_start_block(m_last_deallocated_block))
   {
      std::cout << " WARNING LAST DEALLOCATED IS NOT REACABLE FROM START BLOCK " << std::endl;
      std::raise(SIGABRT);
   }
#endif /* CHECK_BLOCKS_REACHABLE */
   
   // try to find a free block
   mempool_block* block = check_block(m_last_deallocated_block, size); 
   if(!block) 
   {
      block = find_block(m_last_allocated_block->m_next, size);
   }
   if(!block) 
   {
      block = find_block(m_start_block, m_last_allocated_block, size);
   }
   if(!block) return nullptr;
#ifdef CHECK_MEMORY_CORRUPTION
   if(block->m_front_magic != MAGIC)
   {
      std::cout << " ACQUIRE : WARNING FRONT MAGIC CORRUPTION " << std::endl;
   }
   if(block->m_middle_magic != MAGIC)
   {
      std::cout << " ACQUIRE : WARNING middle MAGIC CORRUPTION " << std::endl;
   }
#endif /* CHECK_MEMORY_CORRUPTION */

#ifdef CHECK_BLOCKS_REACHABLE
   if(!check_reachable_from_start_block(block))
   {
      std::cout << " WARNING TRYING TO ALLOCATE BLOCK BUT FOUND BLOCK IS NOT REACABLE FROM START BLOCK " << std::endl;
      std::raise(SIGABRT);
   }
#endif /* CHECK_BLOCKS_REACHABLE */
   
   // acquire the block
   if( (block->m_size - size) < (2*sizeof(mempool_block)) )
   {
      // if rest of block size is to small to divide, just use it all
      block->m_free = false;
   }
   else
   {
      // else make new block and insert
      mempool_block* new_block = reinterpret_cast<mempool_block*>(reinterpret_cast<char*>(block) + size + sizeof(mempool_block));
      if(block->m_next) block->m_next->m_prev = new_block;
      new_block->m_next = block->m_next;
      
      block->m_next = new_block;
      new_block->m_prev = block;
      
      block->m_free = false;
      
      new_block->m_size = block->m_size - size - sizeof(mempool_block);
      block->m_size = size;
      
      new_block->m_free = true;
      new_block->m_root = block->m_root;
#ifdef CHECK_MEMORY_CORRUPTION
      new_block->m_front_magic = MAGIC;
      new_block->m_middle_magic = MAGIC;
#endif /* CHECK_MEMORY_CORRUPTION */
      
#ifdef CHECK_BLOCKS_REACHABLE
      if(!check_reachable_from_start_block(new_block))
      {
         std::cout << " WARNING NEW BLOCK IS NOT REACABLE FROM START BLOCK " << std::endl;
         std::raise(SIGABRT);
      }
#endif /* CHECK_BLOCKS_REACHABLE */
   }
   
   m_last_allocated_block = block;

#ifdef CHECK_BLOCKS_REACHABLE
   if(!check_reachable_from_start_block(m_last_allocated_block))
   {
      std::cout << " SECOND CHECK " << std::endl;
      std::cout << " WARNING LAST ALLOCATED IS NOT REACABLE FROM START BLOCK " << std::endl;
      std::raise(SIGABRT);
   }
#endif /* CHECK_BLOCKS_REACHABLE */

   return reinterpret_cast<char* >(block) + sizeof(mempool_block);
}

/**
 * Release resource of size.
 **/
void mempool_chunk::release(void* resource, std::size_t size)
{
   if(!resource) return;
   
   mempool_block* block = reinterpret_cast<mempool_block*>(reinterpret_cast<char*>(resource) - sizeof(mempool_block));
#ifdef CHECK_BLOCKS_REACHABLE
   if(!check_reachable_from_start_block(block))
   {
      std::cout << " WARNING TRYING TO DEALLOCATE BLOCK BUT FOUND BLOCK IS NOT REACABLE FROM START BLOCK " << std::endl;
      std::raise(SIGABRT);
   }
#endif /* CHECK_BLOCKS_REACHABLE */
   
#ifdef CHECK_MEMORY_CORRUPTION
   if(block->m_front_magic != MAGIC)
   {
      std::cout << " WARNING FRONT MAGIC CORRUPTION " << std::endl;
   }
   if(block->m_middle_magic != MAGIC)
   {
      std::cout << " WARNING middle MAGIC CORRUPTION " << std::endl;
   }
#endif /* CHECK_MEMORY_CORRUPTION */

   // check if both adjacent (next and prev) blocks are free
   if (  block->m_prev 
      && block->m_next 
      )
   {
      if(block->m_prev->m_free && block->m_next->m_free)
      {
#ifdef CHECK_BLOCKS_REACHABLE
         if(!check_reachable_from_start_block(block->m_prev))
         {
            std::cout << " WARNING PREV IS NOT REACABLE FROM START BLOCK " << std::endl;
            std::raise(SIGABRT);
         }
         if(!check_reachable_from_start_block(block->m_next))
         {
            std::cout << " WARNING NEXT IS NOT REACABLE FROM START BLOCK " << std::endl;
            std::raise(SIGABRT);
         }
#endif /* CHECK_BLOCKS_REACHABLE */

         // update size of previous block 
         block->m_prev->m_size += block->m_size + block->m_next->m_size + 2*sizeof(mempool_block);

         // take out current and next block
         block->m_prev->m_next = block->m_next->m_next;
         if(block->m_next->m_next) block->m_next->m_next->m_prev = block->m_prev;
         
         m_last_deallocated_block = block->m_prev;

         if (  block == m_last_allocated_block 
            || block->m_next == m_last_allocated_block 
            )
         {
            m_last_allocated_block = m_last_deallocated_block;
         }
         
#ifdef CHECK_BLOCKS_REACHABLE
         if(!check_reachable_from_start_block(m_last_allocated_block))
         {
            std::cout << " WARNING LAST ALLOCATED IS NOT REACABLE FROM START BLOCK " << std::endl;
            std::cout << " LAST ALLOCATED " << std::endl << *m_last_allocated_block << std::endl;
            std::raise(SIGABRT);
         }
         if(!check_reachable_from_start_block(m_last_deallocated_block))
         {
            std::cout << " WARNING LAST DEALLOCATED IS NOT REACABLE FROM START BLOCK " << std::endl;
            std::raise(SIGABRT);
         }
#endif /* CHECK_BLOCKS_REACHABLE */
         
         return;
      }
   }
   
   // check if previous block is free
   if(block->m_prev)
   {
      if(block->m_prev->m_free)
      {
#ifdef CHECK_BLOCKS_REACHABLE
         if(!check_reachable_from_start_block(block->m_prev))
         {
            std::cout << " WARNING PREV IS NOT REACABLE FROM START BLOCK " << std::endl;
            std::raise(SIGABRT);
         }
#endif /* CHECK_BLOCKS_REACHABLE */
         // update size of previous block
         block->m_prev->m_size += block->m_size + sizeof(mempool_block);
         
         // take current block out of list
         block->m_prev->m_next = block->m_next;
         if(block->m_next) block->m_next->m_prev = block->m_prev;
         
         m_last_deallocated_block = block->m_prev;
         
         if (  block == m_last_allocated_block )
         {
            m_last_allocated_block = m_last_deallocated_block;
         }
#ifdef CHECK_BLOCKS_REACHABLE
         if(!check_reachable_from_start_block(m_last_allocated_block))
         {
            std::cout << " WARNING LAST ALLOCATED IS NOT REACABLE FROM START BLOCK " << std::endl;
            std::raise(SIGABRT);
         }
         if(!check_reachable_from_start_block(m_last_deallocated_block))
         {
            std::cout << " WARNING LAST DEALLOCATED IS NOT REACABLE FROM START BLOCK " << std::endl;
            std::raise(SIGABRT);
         }
#endif /* CHECK_BLOCKS_REACHABLE */
         
         return;
      }
   }

   // check if next block is free
   if(block->m_next)
   {
      if(block->m_next->m_free)
      {
#ifdef CHECK_BLOCKS_REACHABLE
         if(!check_reachable_from_start_block(block->m_next))
         {
            std::cout << " WARNING NEXT IS NOT REACABLE FROM START BLOCK " << std::endl;
            std::raise(SIGABRT);
         }
#endif /* CHECK_BLOCKS_REACHABLE */
         // update size of current block
         block->m_size += block->m_next->m_size + sizeof(mempool_block);

         // take next block out of list 
         if ( block->m_next == m_last_allocated_block )
         {
            m_last_allocated_block = block;
         }

         block->m_next = block->m_next->m_next;
         if(block->m_next) 
         {
            block->m_next->m_prev = block;
         }
         
         block->m_free = true;
         m_last_deallocated_block = block;

#ifdef CHECK_BLOCKS_REACHABLE
         if(!check_reachable_from_start_block(m_last_allocated_block))
         {
            std::cout << " WARNING LAST ALLOCATED IS NOT REACABLE FROM START BLOCK " << std::endl;
            std::cout << " LAST ALLOCATED " << std::endl << *m_last_allocated_block << std::endl;
            std::raise(SIGABRT);
         }
         if(!check_reachable_from_start_block(m_last_deallocated_block))
         {
            std::cout << " WARNING LAST DEALLOCATED IS NOT REACABLE FROM START BLOCK " << std::endl;
            std::raise(SIGABRT);
         }
#endif /* CHECK_BLOCKS_REACHABLE */
         
         return;
      }
   }
   
   // free up block 
   block->m_free = true;
   m_last_deallocated_block = block;
}

/**
 * Grow a new chunk.
 **/
mempool_chunk& mempool::grow(std::size_t size)
{
   m_pool.emplace_back(size);
   return m_pool.back();
}


/**
 * Construct a new memory pool, starting with one chunk of given size.
 **/
mempool::mempool(std::size_t size)
   :  m_size(size)
{
   m_last_allocate_chunk = &grow(m_size);
   m_last_deallocate_chunk = m_last_allocate_chunk;
}

/**
 * Destructor (everything is default destructable).
 **/
mempool::~mempool()
{
}

/**
 * Acquire a pointer of size.
 **/
void* mempool::acquire(std::size_t size, void* hint)
{
   if(!size) return nullptr;
   
   // try to acquire from last allocated chunk
   void* p;
   if( (p = m_last_allocate_chunk->acquire(size, hint)) )
   {
      m_stat.allocated += size;
      return p;
   }
   
   // else try other chunks
   for(auto& chunk : m_pool)
   {
      if( &chunk == m_last_allocate_chunk ) continue;
      if( (p = chunk.acquire(size, hint)) )
      {
         m_last_allocate_chunk = &chunk;
         m_stat.allocated += size;
         return p;
      }
   }

   // else grow new chunk
   m_last_allocate_chunk = &grow(std::max(size + sizeof(mempool_block), m_size));
   p = m_last_allocate_chunk->acquire(size, hint);
   if(!p) std::cout << " WARNING COULD NOT ALLOCATE" << std::endl;
   m_stat.allocated += size;
   return p;
}

/**
 * Release resource of size.
 **/
void mempool::release(void* resource, std::size_t size)
{
   if(!resource) return;
   
   for(auto& chunk : m_pool)
   {
      if(chunk.same_root(resource))
      {
         chunk.release(resource, size);
         m_last_deallocate_chunk = &chunk;
         return;
      }
   }

   mempool_block* block = reinterpret_cast<mempool_block*>(reinterpret_cast<char*>(resource) - sizeof(mempool_block));
   std::cout << " Warning could not release " << std::endl << *block << std::endl;
   std::raise(SIGABRT);
}

/**
 * Initialize m_mem of mempool_holder.
 **/
mempool mempool_holder::m_mem;

} /* namespace memalloc */
