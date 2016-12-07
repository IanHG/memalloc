#ifndef MEMPOOL_H_INCLUDED
#define MEMPOOL_H_INCLUDED

#include<iostream>
#include<stdlib.h>
#include<list>
#include<algorithm>
#include<memory>

//#define CHECK_MEMORY_CORRUPTION
//#define CHECK_BLOCKS_REACHABLE

#ifndef CACHELINE
#define CACHELINE 64
#endif /* CACHELINE */

#ifndef MEMPOOLSIZE
#define MEMPOOLSIZE 1024*1024*1024
#endif /* MEMPOOLSIZE */

namespace memalloc
{

/**
 *
 **/
struct alignas(CACHELINE) mempool_block
{
#ifdef CHECK_MEMORY_CORRUPTION
   unsigned m_front_magic;
#endif /* CHECK_MEMORY_CORRUPTION */
   mempool_block* m_next;
   bool           m_free;
   std::size_t    m_size;
   mempool_block* m_prev;
   void*          m_root;
#ifdef CHECK_MEMORY_CORRUPTION
   unsigned m_middle_magic;
#endif /* CHECK_MEMORY_CORRUPTION */

   mempool_block() = default;
   
   ~mempool_block() = default;
};

inline std::ostream& operator<<(std::ostream& os, const mempool_block& b)
{
   os << "    BLOC " << &b       << "    " << reinterpret_cast<uintptr_t>(&b) << std::endl;
   os << "    NEXT " << b.m_next << "    " << reinterpret_cast<uintptr_t>(b.m_next) << std::endl;
   os << "    PREV " << b.m_prev << "    " << reinterpret_cast<uintptr_t>(b.m_prev) << std::endl;
   os << "    PTR  " << static_cast<const void*>(reinterpret_cast<const char*>(&b) + sizeof(mempool_block)) 
                     << "    "
                     << reinterpret_cast<uintptr_t>(static_cast<const void*>(reinterpret_cast<const char*>(&b) + sizeof(mempool_block))) 
                     << std::endl;
   os << "    PTRE " << static_cast<const void*>(reinterpret_cast<const char*>(&b) + sizeof(mempool_block) + b.m_size) 
                     << "    "
                     << reinterpret_cast<uintptr_t>(static_cast<const void*>(reinterpret_cast<const char*>(&b) + sizeof(mempool_block) + b.m_size)) 
                     << std::endl;
   os << "    FREE " << b.m_free << std::endl;
   os << "    SIZE " << b.m_size << std::endl;
   os << "    ROOT " << b.m_root << std::endl;
#ifdef CHECK_MEMORY_CORRUPTION
   os << "    MAG1 " << std::hex << b.m_front_magic << "   " << &b.m_front_magic << std::endl;
   os << "    MAG2 " << std::hex << b.m_middle_magic << "   " << &b.m_middle_magic << std::endl;
   os << std::dec;
#endif /* CHECK_MEMORY_CORRUPTION */
   return os;
}

/**
 *
 **/
class mempool_chunk
{
   private:
      std::unique_ptr<char[]> m_chunk;
      mempool_block*  m_start_block;
      mempool_block*  m_last_allocated_block;
      mempool_block*  m_last_deallocated_block;
      
      mempool_block* check_block(mempool_block*, std::size_t size);

      mempool_block* find_block(mempool_block*, std::size_t size);
      
      mempool_block* find_block(mempool_block*, mempool_block*, std::size_t size);
      
      std::size_t padding(std::size_t size) const;

#ifdef CHECK_BLOCKS_REACHABLE
      bool check_reachable_from_start_block(mempool_block*) const;
#endif /* CHECK_BLOCKS_REACHABLE */
      
      void print_stack() const;
   public:
      mempool_chunk(std::size_t size);

      mempool_chunk(const mempool_chunk&) = delete;
      mempool_chunk(mempool_chunk&&) = delete;
      mempool_chunk& operator=(const mempool_chunk&) = delete;
      mempool_chunk& operator=(mempool_chunk&&) = delete;

      bool same_root(void* resource);

      void* acquire(std::size_t size, void* hint);

      void release(void* resource, std::size_t size);
};

/**
 *
 **/
struct mempool_stat
{
   unsigned long allocated;
};

/**
 *
 **/
class mempool
{
   private:
      enum mempool_default { init_size = MEMPOOLSIZE };
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
      
      mempool(const mempool&) = delete;
      mempool(mempool&&) = delete;
      mempool& operator=(const mempool&) = delete;
      mempool& operator=(mempool&&) = delete;
      
      //!
      ~mempool();
      
      //! Acquire a chunk of memory
      void* acquire(std::size_t size, void* hint = nullptr);

      //! Release a chunk of memory
      void release(void* resource, std::size_t size);
};

/**
 *
 **/
struct mempool_holder
{
   static mempool m_mem;
};

} /* namespace memalloc */

#endif /* MEMPOOL_H_INCLUDED */
