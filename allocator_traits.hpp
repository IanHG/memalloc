#ifndef ALLOCATOR_TRAITS_H_INCLUDED
#define ALLOCATOR_TRAITS_H_INCLUDED

#include <type_traits>

namespace memalloc
{

template
   <  class Alloc
   >
class allocator_traits
{
   public:
      // define some types
      using allocator_type = Alloc;
      using value_type = typename allocator_type::value_type;
      using pointer = value_type*;
      using const_pointer = typename std::pointer_traits<pointer>::rebind<const value_type>;
      using void_pointer = typename std::pointer_traits<pointer>::rebind<void>;
      using const_void_pointer = typename std::pointer_traits<pointer>::rebind<const void>;
      using difference_type = typename std::pointer_traits<pointer>::difference_type;
      using size_type = typename std::make_unsigned<difference_type>::type;
      
      // propagates
      using propagate_on_container_copy_assignment = std::false_type;
      using propagate_on_container_move_assignment = std::false_type;
      using propagate_on_container_swap = std::false_type;
      using is_always_equal = typename std::empty<Alloc>::type;
      
      // rebinds
      template<class T>
      using rebind_alloc = typename allocator_type::template rebind<T>::other;

      template<class T>
      using rebind_traits = allocator_traits<rebind_alloc<T> >;

      //! allocate
      static pointer allocate(allocator_type& alloc, size_type n, const_void_pointer hint = nullptr)
      {
         return alloc.allocate(n, hint);
      }

      //! deallocate
      static void deallocate(allocator_type& alloc, pointer p, size_type n)
      {
         alloc.deallocate(p, n);
      }

      //! construct
      template< class T, class... Args >
      static void construct(allocator_type& alloc, pointer p, Args&&... args)
      {
         alloc.construct(p, std::forward<Args>(args)...);
      }

      //! destroy
      template< class T >
      static void destroy(allocator_type& alloc, pointer p)
      {
         alloc.destroy(p);
      }

      //! max size
      static size_type max_size(const allocator_type& alloc)
      {
         return alloc.max_size();
      }

      //!
      static allocator_type select_on_container_copy_construction(const allocator_type& alloc)
      {
         return alloc;
      }
};

} /* namespace memalloc */

#endif /* ALLOCATOR_TRAITS_H_INCLUDED */
