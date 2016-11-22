#ifndef MEMPOOL_H_INCLUDED
#define MEMPOOL_H_INCLUDED

#include<iostream>
#include<stdlib.h>
#include<list>
#include<algorithm>
#include<memory>

namespace memalloc
{

struct mempool_block
{
   mempool_block* m_next;
   bool           m_free;
   std::size_t    m_size;
   mempool_block* m_prev;
   void*          m_root;

   mempool_block() = default;
   
   ~mempool_block() = default;
};

class mempool_chunk
{
   private:
      std::unique_ptr<char[]> m_chunk;
      mempool_block*  m_start_block;
      mempool_block*  m_last_allocated_block;
      mempool_block*  m_last_deallocated_block;
      std::size_t m_pad_to = 64;

      mempool_block* check_block(mempool_block*, std::size_t size);

      mempool_block* find_block(mempool_block*, std::size_t size);
      
      std::size_t padding(std::size_t size) const;
   public:
      mempool_chunk(std::size_t size);

      bool same_root(void* resource);

      void* acquire(std::size_t size, void* hint);

      void release(void* resource, std::size_t size);
};

struct mempool_stat
{
   unsigned long allocated;
};

class mempool
{
   private:
      enum mempool_default { init_size = 1024*1024*1024 };
      mempool_stat m_stat;
      std::list<mempool_chunk> m_pool;
      mempool_chunk* m_last_allocate_chunk;
      mempool_chunk* m_last_deallocate_chunk;
      size_t         m_size;
      
      //! Grow a new chunk and return a reference to the newly allocated chunk
      mempool_chunk& grow(std::size_t size);
   
   public:
      //!
      mempool(std::size_t size = mempool_default::init_size);
      
      //!
      ~mempool();
      
      //! Acquire a chunk of memory
      void* acquire(std::size_t size, void* hint = nullptr);

      //! Release a chunk of memory
      void release(void* resource, std::size_t size);
};

struct mempool_holder
{
   static mempool m_mem;
};

} /* namespace memalloc */

#endif /* MEMPOOL_H_INCLUDED */
