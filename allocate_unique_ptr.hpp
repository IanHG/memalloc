#ifndef ALLOCATE_UNIQUE_PTR_H_INCLUDED
#define ALLOCATE_UNIQUE_PTR_H_INCLUDED

#include <memory>

namespace memalloc
{

template<class T>
struct decay_type
{
   using type = T;
};

template<class T>
struct decay_type<T[]>
{
   using type = T;
};

template
   <  class T
   ,  class Alloc
   >
struct deleter
{
   Alloc& alloc;
   std::size_t n;
   
   deleter(Alloc& alloc_, std::size_t n_)
      :  alloc(alloc_)
      ,  n(n_)
   {
   }

   deleter(deleter&&) = default;
   deleter& operator=(deleter&& other)
   {
      assert(alloc == other.alloc);
      std::swap(n, other.n);
   }

   void operator()(T* t)
   {
      alloc.deallocate(t, n);
   }
};

template
   <  class T
   ,  class Alloc
   >
struct deleter_destroy
{
   Alloc& alloc;
   std::size_t n;
   
   deleter_destroy(Alloc& alloc_, std::size_t n_)
      :  alloc(alloc_)
      ,  n(n_)
   {
   }

   deleter_destroy(deleter_destroy&&) = default;
   deleter_destroy& operator=(deleter_destroy&& other)
   {
      assert(alloc == other.alloc);
      std::swap(n, other.n);
   }

   void operator()(T* t)
   {
      alloc.destroy(t);
      alloc.deallocate(t, n);
   }
};

template 
   <  class T
   ,  class Alloc
   ,  class SmartPtr = std::unique_ptr<T, deleter<typename decay_type<T>::type, Alloc> >
   >
SmartPtr allocate_unique_ptr(Alloc& alloc, std::size_t n, typename Alloc::const_pointer hint = nullptr)
{
   using value_type = typename decay_type<T>::type;
   static_assert(std::is_same<value_type, typename Alloc::value_type>::value, "Not correct allocator.");
   return SmartPtr( alloc.allocate(n, hint), deleter<value_type, Alloc>(alloc, n) );
}

template 
   <  class T
   ,  class Alloc
   ,  class... Args
   ,  class SmartPtr = std::unique_ptr<T, deleter_destroy<typename decay_type<T>::type, Alloc> >
   >
SmartPtr allocate_and_construct_unique_ptr(Alloc& alloc, std::size_t n, typename Alloc::const_pointer hint = nullptr, Args&&... args)
{
   using value_type = typename decay_type<T>::type;
   static_assert(std::is_same<value_type, typename Alloc::value_type>::value, "Not correct allocator.");
   auto ptr = alloc.allocate(n, hint);
   alloc.construct(ptr, std::forward<Args>(args)...);
   return SmartPtr( ptr, deleter_destroy<value_type, Alloc>(alloc, n) );
}

} /* namespace memalloc */

#endif /* ALLOCATE_UNIQUE_PTR_H_INCLUDED */
