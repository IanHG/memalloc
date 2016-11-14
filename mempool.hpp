#ifndef MEMPOOL_H_INCLUDED
#define MEMPOOL_H_INCLUDED

#include<iostream>
#include<stdlib.h>
#include<list>
#include<algorithm>

namespace memalloc
{

struct mempool_block
{
   mempool_block* _prev;
   mempool_block* _next;
   size_t        _size;
   bool          _free;
   void*         _root;

   mempool_block(mempool_block* prev, mempool_block* next, size_t size, int free)
      :  _prev(prev)
      ,  _next(next)
      ,  _size(size)
      ,  _free(free) 
   { 
   }
   
   ~mempool_block() = default;
};

class mempool
{
   private:
      enum pool_defaults{ init_size = 1024*1024*1024 };
      std::list<char*> _pool;
      mempool_block*   _blocks;
      size_t           _size;
      
      //!
      void grow(mempool_block* block, size_t size);
      struct killer
      { 
         void operator() (char* ptr) { delete[] ptr; }
      };
   public:
      //!
      mempool(size_t size=init_size);
      //!
      ~mempool();
      
      //! Acquire a chunk of memory
      void* acquire(size_t size);
      //! Release a chunk of memory
      void release(void* resource, size_t size = 0);
};

} /* namespace memalloc */

#endif /* MEMPOOL_H_INCLUDED */
