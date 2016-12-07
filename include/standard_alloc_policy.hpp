#ifndef STANDARD_ALLOC_POLICY_H_INCLUDED
#define STANDARD_ALLOC_POLICY_H_INCLUDED

namespace memalloc
{

template<typename T>
class standard_alloc_policy 
{
   public : 
      using value_type = T;
      using pointer = value_type*;
      using const_pointer = const value_type*;
      using reference = value_type&;
      using const_reference = const value_type&;
      using size_type = std::size_t;
      using difference_type = std::ptrdiff_t;
   
   public : 
      template<typename U>
      struct rebind 
      {
         using other = standard_alloc_policy<U>;
      };
   
   public : 
      inline explicit standard_alloc_policy() {}
      
      inline ~standard_alloc_policy() {}
      
      inline explicit standard_alloc_policy(standard_alloc_policy const&) {}
      
      template <typename U>
      inline explicit standard_alloc_policy(standard_alloc_policy<U> const&) {}
      
      inline pointer allocate(size_type cnt, typename std::allocator<void>::const_pointer = 0) 
      { 
          return reinterpret_cast<pointer>(::operator new(cnt * sizeof (T))); 
      }
      
      inline void deallocate(pointer p, size_type) 
      { 
         ::operator delete(p); 
      }
      
      inline size_type max_size() const 
      { 
         return std::numeric_limits<size_type>::max(); 
      }
};

template<typename T, typename T2>
inline bool operator==(standard_alloc_policy<T> const&, standard_alloc_policy<T2> const&) 
{ 
   return true;
}

template<typename T, typename OtherAllocator>
inline bool operator==(standard_alloc_policy<T> const&, OtherAllocator const&) 
{ 
   return false; 
}

} /* namespace memalloc */

#endif /* STANDARD_ALLOC_POLICY_H_INCLUDED */
