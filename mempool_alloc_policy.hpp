#ifndef MEMPOOLALLOCATOR_H_INCLUDED
#define MEMPOOLALLOCATOR_H_INCLUDED

#include"mempool.hpp"

namespace memalloc
{

template<typename T>
class mempool_alloc_policy
   :  private mempool_holder
{
   public:
      using value_type = T;
      using pointer = value_type*;
      using const_pointer = const value_type*;
      using reference = value_type&;
      using const_reference = const value_type&;
      using size_type = std::size_t;
      using difference_type = std::ptrdiff_t;
   
   public:
      template<typename U> struct rebind
      {
         using other = mempool_alloc_policy<U>; 
      };
   
   public:
      /* construct/destruct */
      mempool_alloc_policy() throw() = default;
      mempool_alloc_policy(const mempool_alloc_policy&) throw() = default;
      template<typename U> mempool_alloc_policy(const mempool_alloc_policy<U>&) throw() {};
      ~mempool_alloc_policy() throw() = default;
      
      /* alloc / dealloc */
      //! allocate
      pointer allocate(size_type size, const_pointer hint = 0) 
      { 
         return static_cast<pointer>( m_mem.acquire(size*sizeof(value_type)) ); 
      }
      //! deallocate
      void deallocate(pointer p, size_type n) 
      { 
         m_mem.release(p, n*sizeof(value_type)); 
      }
      
      /* max size */
      size_type max_size() const throw() { return size_t(-1) / sizeof(value_type); }
         
      /* operator overload */
      template<typename U>
      bool operator==(const mempool_alloc_policy<U>&) throw() { return true; }
};

} /* namespace memalloc */

#endif /* MEMPOOLALLOCATOR_H_INCLUDED */
