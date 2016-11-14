#ifndef MEMPOOL_ALLOCATOR_H_INCLUDED
#define MEMPOOL_ALLOCATOR_H_INCLUDED

#include "allocator.hpp"
#include "mempool_alloc_policy.hpp"

namespace memalloc
{

   template<class T>
      using mempool_allocator = allocator<T, mempool_alloc_policy<T> >;

} /* namespace memalloc */

#endif /* MEMPOOL_ALLOCATOR_H_INCLUDED */
