#ifndef ALLOCATALBE_H_INCLUDED
#define ALLOCATALBE_H_INCLUDED

#include "allocator.hpp"
#include "allocate_unique_ptr.hpp"

namespace memalloc
{


template
   <  class Alloc
   >
class allocatable_impl
   :  public Alloc
{
   public:
      using allocator = Alloc;
      using pointer = typename allocator::pointer;
      using const_pointer = typename allocator::const_pointer;
      using unique_pointer = decltype(memalloc::allocate_unique_ptr<typename Alloc::value_type>(std::declval<allocator&>(), 0, nullptr));
      using unique_array = decltype(memalloc::allocate_unique_ptr<typename Alloc::value_type[]>(std::declval<allocator&>(), 0, nullptr));

      inline allocatable_impl() = default;

      inline explicit allocatable_impl(const allocatable_impl&) = default;

      inline ~allocatable_impl() = default;
      
      inline allocator& allocator_type()
      {
         return static_cast<allocator&>(*this);
      }
      
      inline const allocator& allocator_type() const
      {
         return static_cast<const allocator&>(*this);
      }

      inline pointer allocate(std::size_t n, const_pointer hint = nullptr)
      {
         return allocator_type().allocate(n, hint);
      }

      inline void deallocate(pointer ptr, std::size_t n)
      {
         allocator_type().deallocate(ptr, n);
      }

      inline auto allocate_unique_pointer(const_pointer hint = nullptr)
      {
         return memalloc::allocate_unique_ptr<typename Alloc::value_type>(this->allocator_type(), 1, nullptr);
      }

      inline auto allocate_unique_array(std::size_t n, const_pointer hint = nullptr)
      {
         return memalloc::allocate_unique_ptr<typename Alloc::value_type[]>(this->allocator_type(), n, nullptr);
      }
};

template
   <  class T
   ,  class Alloc = allocator<T>
   >
class allocatable
   :  public allocatable_impl<typename Alloc::template rebind<T>::other>
{
   public:
      inline explicit allocatable() = default;
      inline explicit allocatable(const allocatable&) = default;
      inline ~allocatable() = default;

};

} /* namespace memalloc */

#endif /* ALLOCATALBE_H_INCLUDED */
