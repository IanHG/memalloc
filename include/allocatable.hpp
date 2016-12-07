#ifndef ALLOCATALBE_H_INCLUDED
#define ALLOCATALBE_H_INCLUDED

#include "allocator.hpp"
#include "allocate_smart.hpp"

namespace memalloc
{


template
   <  class Alloc
   >
class allocatable_impl
   :  public Alloc
{
   public:
      using allocator_type = Alloc;
      using pointer = typename allocator_type::pointer;
      using const_pointer = typename allocator_type::const_pointer;
      using const_void_pointer = typename allocator_type::const_pointer;
      
      using unique_pointer = decltype(memalloc::allocate_unique_pointer<typename Alloc::value_type>(std::declval<allocator_type&>(), nullptr));
      using unique_array = decltype(memalloc::allocate_unique_array<typename Alloc::value_type>(std::declval<allocator_type&>(), 0, nullptr));
      
      using constructed_unique_pointer = decltype(memalloc::allocate_and_construct_unique_pointer<typename Alloc::value_type>(std::declval<allocator_type&>(), nullptr));
      using constructed_unique_array = decltype(memalloc::allocate_and_construct_unique_array<typename Alloc::value_type>(std::declval<allocator_type&>(), 0, nullptr));

      inline allocatable_impl() = default;

      inline explicit allocatable_impl(const allocatable_impl&) = default;

      inline ~allocatable_impl() = default;
      
      inline allocator_type& get_allocator_type()
      {
         return static_cast<allocator_type&>(*this);
      }
      
      inline const allocator_type& get_allocator_type() const
      {
         return static_cast<const allocator_type&>(*this);
      }

      inline pointer allocate(std::size_t n, const_pointer hint = nullptr)
      {
         return get_allocator_type().allocate(n, hint);
      }

      inline void deallocate(pointer ptr, std::size_t n)
      {
         get_allocator_type().deallocate(ptr, n);
      }

      inline auto allocate_unique_pointer(const_pointer hint = nullptr)
      {
         return memalloc::allocate_unique_pointer<typename Alloc::value_type>(this->get_allocator_type(), hint);
      }
      
      inline auto allocate_unique_array(std::size_t n, const_pointer hint = nullptr)
      {
         return memalloc::allocate_unique_array<typename Alloc::value_type>(this->get_allocator_type(), n, hint);
      }

      template<class... Args>
      inline auto allocate_and_construct_unique_pointer(const_pointer hint = nullptr, Args&&... args)
      {
         return memalloc::allocate_and_construct_unique_pointer<typename Alloc::value_type>(this->get_allocator_type(), hint, std::forward<Args>(args)...);
      }
      
      template<class... Args>
      inline auto allocate_and_construct_unique_array(std::size_t n, const_pointer hint = nullptr, Args&&... args)
      {
         return memalloc::allocate_and_construct_unique_array<typename Alloc::value_type>(this->get_allocator_type(), n, hint, std::forward<Args>(args)...);
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
      using impl_type = allocatable_impl<typename Alloc::template rebind<T>::other>;
      using allocator_type = typename impl_type::allocator_type;

      inline explicit allocatable() = default;
      inline explicit allocatable(const allocatable&) = default;
      inline ~allocatable() = default;
      
      inline explicit allocatable(allocator_type& alloc)
         :  impl_type(alloc)
      {
      }

      inline explicit allocatable(allocator_type&& alloc)
         :  impl_type(std::move(alloc))
      {
      }
};

} /* namespace memalloc */

#endif /* ALLOCATALBE_H_INCLUDED */
