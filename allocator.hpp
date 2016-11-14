#ifndef ALLOCATOR_H_INCLUDED
#define ALLOCATOR_H_INCLUDED

#include "object_traits.hpp"
#include "standard_alloc_policy.hpp"

namespace memalloc
{

template
   <  typename T
   ,  typename Policy = standard_alloc_policy<T>
   ,  typename Traits = object_traits<T> 
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
      
      template <typename U>
      inline allocator(allocator<U> const&) {}
      
      template <typename U, typename P, typename T2>
      inline allocator(allocator<U, P, T2> const& rhs):Traits(rhs), Policy(rhs) {}
};

template<typename T, typename P, typename Tr>
inline bool operator==(allocator<T, P, Tr> const& lhs, allocator<T, P, Tr> const& rhs) 
{ 
   return operator==(static_cast<const P&>(lhs), static_cast<const P&>(rhs)); 
}


template<typename T, typename P, typename Tr, typename T2, typename P2, typename Tr2>
inline bool operator==(allocator<T, P, Tr> const& lhs, allocator<T2, P2, Tr2> const& rhs)
{ 
   return operator==(static_cast<const P&>(lhs), static_cast<const P2&>(rhs)); 
}

template<typename T, typename P, typename Tr, typename Otherallocator>
inline bool operator==(allocator<T, P, Tr> const& lhs, Otherallocator const& rhs) 
{ 
   return operator==(static_cast<const P&>(lhs), rhs); 
}

template<typename T, typename P, typename Tr>
inline bool operator!=(allocator<T, P, Tr> const& lhs, allocator<T, P, Tr> const& rhs) 
{ 
   return !operator==(lhs, rhs); 
}

template<typename T, typename P, typename Tr, typename T2, typename P2, typename Tr2>
inline bool operator!=(allocator<T, P, Tr> const& lhs, allocator<T2, P2, Tr2> const& rhs) 
{ 
   return !operator==(lhs, rhs); 
}

template<typename T, typename P, typename Tr, typename Otherallocator>
inline bool operator!=(allocator<T, P, Tr> const& lhs, Otherallocator const& rhs) 
{ 
   return !operator==(lhs, rhs); 
}

} /* namespace memalloc */

#endif /* ALLOCATOR_H_INCLUDED */
