#ifndef ALLOCATE_UNIQUE_PTR_H_INCLUDED
#define ALLOCATE_UNIQUE_PTR_H_INCLUDED

#include <memory>

namespace memalloc
{

namespace detail
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

//! delete single pointer
template
   <  class T
   ,  class Alloc
   >
struct deleter_pointer
{
   Alloc& alloc_;
   
   deleter_pointer(Alloc& alloc)
      :  alloc_(alloc)
   {
   }

   deleter_pointer(deleter_pointer&&) = default;
   deleter_pointer& operator=(deleter_pointer&& other)
   {
      assert(alloc_ == other.alloc_);
   }

   void operator()(T* t)
   {
      alloc_.deallocate(t, 1);
   }
};

//! delete array
template
   <  class T
   ,  class Alloc
   >
struct deleter_array
{
   Alloc& alloc_;
   std::size_t n_;
   
   deleter_array(Alloc& alloc, std::size_t n)
      :  alloc_(alloc)
      ,  n_(n)
   {
   }

   deleter_array(deleter_array&&) = default;
   deleter_array& operator=(deleter_array&& other)
   {
      assert(alloc_ == other.alloc_);
      std::swap(n_, other.n_);
   }

   void operator()(T* t)
   {
      alloc_.deallocate(t, n_);
   }
};

//! delete and construct pointer
template
   <  class T
   ,  class Alloc
   >
struct deleter_destroy_pointer
{
   Alloc& alloc_;
   
   deleter_destroy_pointer(Alloc& alloc)
      :  alloc_(alloc)
   {
   }

   deleter_destroy_pointer(deleter_destroy_pointer&&) = default;
   deleter_destroy_pointer& operator=(deleter_destroy_pointer&& other)
   {
      assert(alloc_ == other.alloc_);
   }

   void operator()(T* t)
   {
      alloc_.destroy(t);
      alloc_.deallocate(t, 1);
   }
};

//! delete and construct array
template
   <  class T
   ,  class Alloc
   >
struct deleter_destroy_array
{
   Alloc& alloc_;
   std::size_t n_;
   
   deleter_destroy_array(Alloc& alloc, std::size_t n)
      :  alloc_(alloc)
      ,  n_(n)
   {
   }

   deleter_destroy_array(deleter_destroy_array&&) = default;
   deleter_destroy_array& operator=(deleter_destroy_array&& other)
   {
      assert(alloc_ == other.alloc_);
      std::swap(n_, other.n_);
   }

   void operator()(T* t)
   {
      auto p = t;
      for(int i = 0; i < n_; ++i)
      {
         alloc_.destroy(p);
         ++p;
      }
      alloc_.deallocate(t, n_);
   }
};

} /* namespace detail */


/**
 * Allocate unique pointer
 **/
template 
   <  class T
   ,  class Alloc
   ,  class SmartPtr = std::unique_ptr<T, detail::deleter_pointer<typename detail::decay_type<T>::type, Alloc> >
   >
SmartPtr allocate_unique_pointer(Alloc& alloc, typename Alloc::const_pointer hint = nullptr)
{
   using value_type = typename detail::decay_type<T>::type;
   static_assert(std::is_same<value_type, typename Alloc::value_type>::value, "Not correct allocator.");
   return SmartPtr( alloc.allocate(1, hint), detail::deleter_pointer<value_type, Alloc>(alloc) );
}

/**
 * Allocate unique array
 **/
template 
   <  class T
   ,  class Alloc
   ,  class SmartPtr = std::unique_ptr<T[], detail::deleter_array<typename detail::decay_type<T>::type, Alloc> >
   >
SmartPtr allocate_unique_array(Alloc& alloc, std::size_t n, typename Alloc::const_pointer hint = nullptr)
{
   using value_type = typename detail::decay_type<T>::type;
   static_assert(std::is_same<value_type, typename Alloc::value_type>::value, "Not correct allocator.");
   return SmartPtr( alloc.allocate(n, hint), detail::deleter_array<value_type, Alloc>(alloc, n) );
}

/**
 * Allocate and construct unique pointer
 **/
template 
   <  class T
   ,  class Alloc
   ,  class... Args
   ,  class SmartPtr = std::unique_ptr<T, detail::deleter_destroy_pointer<typename detail::decay_type<T>::type, Alloc> >
   >
SmartPtr allocate_and_construct_unique_pointer(Alloc& alloc, typename Alloc::const_pointer hint = nullptr, Args&&... args)
{
   using value_type = typename detail::decay_type<T>::type;
   static_assert(std::is_same<value_type, typename Alloc::value_type>::value, "Not correct allocator.");
   auto ptr = alloc.allocate(1, hint);
   alloc.construct(ptr, std::forward<Args>(args)...);
   return SmartPtr( ptr, detail::deleter_destroy_pointer<value_type, Alloc>(alloc) );
}


/**
 * Allocate and construct unique array
 **/
template 
   <  class T
   ,  class Alloc
   ,  class... Args
   ,  class SmartPtr = std::unique_ptr<T[], detail::deleter_destroy_array<typename detail::decay_type<T>::type, Alloc> >
   >
SmartPtr allocate_and_construct_unique_array(Alloc& alloc, std::size_t n, typename Alloc::const_pointer hint = nullptr, Args&&... args)
{
   using value_type = typename detail::decay_type<T>::type;
   static_assert(std::is_same<value_type, typename Alloc::value_type>::value, "Not correct allocator.");
   auto ptr = alloc.allocate(n, hint);
   auto p = ptr;
   for(int i = 0; i < n; ++i)
   {
      alloc.construct(p, std::forward<Args>(args)...);
      ++p;
   }
   return SmartPtr( ptr, detail::deleter_destroy_array<value_type, Alloc>(alloc, n) );
}

} /* namespace memalloc */

#endif /* ALLOCATE_UNIQUE_PTR_H_INCLUDED */
