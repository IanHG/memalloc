#ifndef MEMPOOLALLOCATOR_H_INCLUDED
#define MEMPOOLALLOCATOR_H_INCLUDED

#include"mempool.hpp"

namespace memalloc
{

///* forward declaration */
//template<typename T>
//class mempool_alloc_policy;
//
///* specialization for void */
//template<>
//class mempool_alloc_policy<void>
//{
//   public:
//      typedef void*       pointer;
//      typedef const void* const_pointer;
//      typedef void        value_type;
//      template<typename U> struct rebind{ typedef mempool_alloc_policy<U> other; };
//};

template<typename T>
class mempool_alloc_policy
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
         return static_cast<pointer>( _mem.acquire(size*sizeof(value_type)) ); 
      }
      //! deallocate
      void deallocate(pointer p, size_type n) 
      { 
         _mem.release(p,n); 
      }
      
      /* max size */
      size_type max_size() const throw() { size_t(-1) / sizeof(value_type); }
         
      /* operator overload */
      template<typename U>
      bool operator==(const mempool_alloc_policy<U>&) throw() { return true; }

   private:
      //! Internal memory pool
      static mempool _mem;
};

template<typename T> mempool mempool_alloc_policy<T>::_mem;

} /* namespace memalloc */

#endif /* MEMPOOLALLOCATOR_H_INCLUDED */
