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
      template<class... Args>
      inline void construct(T* p, Args&&... args) 
      { 
         new(static_cast<void*>(p)) T(std::forward<Args>(args)...); 
      }
      
      //! Destroy
      inline void destroy(T* p) 
      { 
         p->~T(); 
      }
};

} /* namespace memalloc */

#endif /* OBJECT_TRAITS_H_INCLUDED */
