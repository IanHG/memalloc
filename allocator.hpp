#ifndef ALLOCATOR_H_INCLUDED
#define ALLOCATOR_H_INCLUDED

#include "object_traits.hpp"
#include "standard_alloc_policy.hpp"

namespace memalloc
{

template
   <  class T
   ,  class Policy = standard_alloc_policy<T>
   ,  class Traits = object_traits<T> 
   >
class allocator 
   :  public Policy
   ,  public Traits 
{
   private : 
      using AllocationPolicy = Policy;
      using TTraits = Traits;
   
   public : 
      using size_type = typename AllocationPolicy::size_type;
      using difference_type = typename AllocationPolicy::difference_type;
      using pointer = typename AllocationPolicy::pointer;
      using const_pointer = typename AllocationPolicy::const_pointer;
      using reference = typename AllocationPolicy::reference;
      using const_reference = typename AllocationPolicy::const_reference;
      using value_type = typename AllocationPolicy::value_type;
   
   public : 
      template<typename U>
      struct rebind 
      {
          using other = allocator<U, typename AllocationPolicy::template rebind<U>::other, typename TTraits::template rebind<U>::other >;
      };
   
   public : 
      inline explicit allocator() {}

      inline ~allocator() {}
      
      inline allocator(allocator const& rhs): Traits(rhs), Policy(rhs) {}
      
      template <class U>
      inline allocator(allocator<U> const&) {}
      
      template <class U, class P, class T2>
      inline allocator(allocator<U, P, T2> const& rhs) : Traits(rhs), Policy(rhs) {}
};

template<class T, class P, class Tr>
inline bool operator==(allocator<T, P, Tr> const& lhs, allocator<T, P, Tr> const& rhs) 
{ 
   return operator==(static_cast<const P&>(lhs), static_cast<const P&>(rhs)); 
}


template<class T, class P, class Tr, class T2, class P2, class Tr2>
inline bool operator==(allocator<T, P, Tr> const& lhs, allocator<T2, P2, Tr2> const& rhs)
{ 
   return operator==(static_cast<const P&>(lhs), static_cast<const P2&>(rhs)); 
}

template<class T, class P, class Tr, class Otherallocator>
inline bool operator==(allocator<T, P, Tr> const& lhs, Otherallocator const& rhs) 
{ 
   return operator==(static_cast<const P&>(lhs), rhs); 
}

template<class T, class P, class Tr>
inline bool operator!=(allocator<T, P, Tr> const& lhs, allocator<T, P, Tr> const& rhs) 
{ 
   return !operator==(lhs, rhs); 
}

template<class T, class P, class Tr, class T2, class P2, class Tr2>
inline bool operator!=(allocator<T, P, Tr> const& lhs, allocator<T2, P2, Tr2> const& rhs) 
{ 
   return !operator==(lhs, rhs); 
}

template<class T, class P, class Tr, class Otherallocator>
inline bool operator!=(allocator<T, P, Tr> const& lhs, Otherallocator const& rhs) 
{ 
   return !operator==(lhs, rhs); 
}

} /* namespace memalloc */

#endif /* ALLOCATOR_H_INCLUDED */
