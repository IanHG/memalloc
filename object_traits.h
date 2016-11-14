#ifndef OBJECT_TRAITS_H_INCLUDED
#define OBJECT_TRAITS_H_INCLUDED

namespace memalloc
{

template<typename T>
class object_traits 
{
   public : 
      template<typename U>
      struct rebind 
      {
          using other = object_traits<U>;
      };
   
   public : 
      inline explicit object_traits() = default;
      inline ~object_traits() = default;
      
      template <typename U>
      inline explicit object_traits(object_traits<U> const&) {}
      
      //!@{
      //! Get address of pointer
      inline T* address(T& r) { return &r; }
      inline T const* address(T const& r) { return &r; }
      //!@}
      
      //! Construct
      inline void construct(T* p, const T& t) { new(p) T(t); }
      
      //! Destroy
      inline void destroy(T* p) { p->~T(); }
};

} /* namespace memalloc */

#endif /* OBJECT_TRAITS_H_INCLUDED */
