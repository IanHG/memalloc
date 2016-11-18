#include"mempool.hpp"

#include <iostream>

namespace memalloc
{

/**
 * Check if a block is free and of a correct size. 
 **/
mempool_block* mempool_chunk::check_block(mempool_block* block, std::size_t size)
{
   return (block->m_free && block->m_size >= size) ? block : nullptr;
}

/**
 * Find the first free block that is large enough,
 * which lies after the inputtet block.
 **/
mempool_block* mempool_chunk::find_block(mempool_block* block, std::size_t size)
{
   mempool_block* b = block;
   
   // find free block
   while(b && (!b->m_free || (b->m_size < size))) 
   {
      b = b->m_next;
      //if(!b) b = m_start_block;
      //if(b == block) break;
   }

   return b;
}

/**
 * Construct mempool_chunk of given size.
 **/
mempool_chunk::mempool_chunk(std::size_t size)
   :  m_chunk(new char[size])
{
   // initialize the first block
   mempool_block* new_block = reinterpret_cast<mempool_block*>( m_chunk.get() );
   new_block->m_prev = nullptr;
   new_block->m_next = nullptr;
   new_block->m_free = true;
   new_block->m_size = size - sizeof(mempool_block);
   new_block->m_root = static_cast<void*>( m_chunk.get() );

   m_start_block = new_block;
   m_last_allocated_block = new_block;
   m_last_deallocated_block = new_block;
}

/**
 *
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
   
   // try to find a free block
   mempool_block* block = check_block(m_last_deallocated_block, size); 
   if(!block) block = find_block(m_last_allocated_block, size);
   if(!block) return nullptr;
   
   // acquire the block
   if(block->m_size - size < 2*sizeof(mempool_block))
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
   }
   
   m_last_allocated_block = block;

   //std::cout << " ALLOCATED BLOCK : " << block << std::endl;
   //std::cout << " ALLOCATED PTR   : " << (void*)(reinterpret_cast<char*>(block) + sizeof(mempool_block)) << std::endl;

   return reinterpret_cast<char* >(block) + sizeof(mempool_block);
}
/**
 * release
 **/
void mempool_chunk::release(void* resource, std::size_t size)
{
   if(!resource) return;
   
   mempool_block* block = reinterpret_cast<mempool_block*>(reinterpret_cast<char*>(resource) - sizeof(mempool_block));
   //std::cout << " RELEASE BLOCK : " << block << std::endl;
   //std::cout << " RELEASE PTR   : " << (void*)(reinterpret_cast<char*>(block) + sizeof(mempool_block)) << std::endl;
   //std::cout << " RELEASE RESOU : " << resource << std::endl;

   // check if both adjacent (next and prev) blocks are free
   if (  block->m_prev 
      && block->m_next 
      )
   {
      if(block->m_prev->m_free && block->m_next->m_free)
      {
         // update size of previous block 
         block->m_prev->m_size += block->m_size + block->m_next->m_size + 2*sizeof(mempool_block);

         // take out current and next block
         block->m_prev->m_next = block->m_next->m_next;
         if(block->m_next->m_next) block->m_next->m_next->m_prev = block->m_prev;
         
         m_last_deallocated_block = block->m_prev;

         // return
         return;
      }
   }
   
   // check if previous block is free
   if(block->m_prev)
   {
      if(block->m_prev->m_free)
      {
         // update size of previous block
         block->m_prev->m_size += block->m_size + sizeof(mempool_block);
         
         // take current block out of list
         block->m_prev->m_next = block->m_next;
         if(block->m_next) block->m_next->m_prev = block->m_prev;
         
         m_last_deallocated_block = block->m_prev;
         
         // return
         return;
      }
   }

   // check if next block is free
   if(block->m_next)
   {
      if(block->m_next->m_free)
      {
         // update size of current block
         block->m_size += block->m_next->m_size + sizeof(mempool_block);

         // take next block out of list 
         block->m_next = block->m_next->m_next;
         if(block->m_next) block->m_next->m_prev = block;
         
         block->m_free = true;
         m_last_deallocated_block = block;

         // return 
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
   std::cout << " GROW !! " << std::endl;
   m_pool.emplace_back(std::max(size + sizeof(mempool_block), m_size));
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
 * Destructor
 **/
mempool::~mempool()
{
}

/**
 * acquire
 **/
void* mempool::acquire(std::size_t size, void* hint)
{
   if(!size) return nullptr;
   // try to acquire from last allocated chunk
   void* p;
   if( (p = m_last_deallocate_chunk->acquire(size, hint)) )
   {
      m_stat.allocated += size;
      return p;
   }
   
   // else try other chunks
   for(auto& chunk : m_pool)
   {
      if( (p = chunk.acquire(size, hint)) )
      {
         m_last_allocate_chunk = &chunk;
         m_stat.allocated += size;
         return p;
      }
   }

   // else grow new chunk
   p = grow(size).acquire(size, hint);
   if(!p) std::cout << " WARNING COULD NOT ALLOCATE" << std::endl;
   m_stat.allocated += size;
   return p;
}

/**
 * release
 **/
void mempool::release(void* resource, size_t size)
{
   if(!resource) return;
   
   for(auto& chunk : m_pool)
   {
      if(chunk.same_root(resource))
      {
         chunk.release(resource, size);
         return;
      }
   }

   std::cout << " Warning could not release " << std::endl;
}

} /* namespace memalloc */
