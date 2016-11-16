#include <iostream>
#include <memory>
#include <functional>
#include <type_traits>
#include <cassert>

#include "timer.hpp"
#include "allocator.hpp"
#include "allocatable.hpp"
#include "mempool_allocator.hpp"
#include "mempool_alloc_policy.hpp"
#include "allocate_smart.hpp"


//template
//   <  class U
//   ,  class Alloc = memalloc::allocator<U>
//   ,  class SmartPtr = std::unique_ptr<U[], std::function<void(U*)> >
//   >
//class deleted_unique_ptr:
//   public SmartPtr
//{
//   private:
//      using allocator_type = Alloc;
//
//   public:
//      deleted_unique_ptr(int size, typename Alloc::const_pointer hint = nullptr)
//         :  SmartPtr 
//            (  typename allocator_type::template rebind<U>::other().allocate(size, nullptr)
//            ,  [size](U* u){ typename allocator_type::template rebind<U>::other().deallocate(u, size); }
//            )
//      {
//      }
//};

template
   <  class T = double
   ,  class Alloc = memalloc::allocator<T> 
   >
class test_lol
   :  private memalloc::allocatable<T, Alloc>
   ,  private memalloc::allocatable<T*, Alloc>
{
   using allocator_tp = memalloc::allocatable<T, Alloc>;
   using allocator_tpp = memalloc::allocatable<T*, Alloc>;

   std::size_t n;
   std::vector<int, Alloc> lol;
   double* p;
   double** pp;
   typename allocator_tp::unique_array sptr;
   typename allocator_tp::constructed_unique_pointer sptr2;

   public:
      test_lol(std::size_t n_)
         : n(n_)
         , lol(n)
         , p(allocator_tp::allocate(n))
         , pp(allocator_tpp::allocate(n))
         , sptr(allocator_tp::allocate_unique_array(n))
         , sptr2(allocator_tp::allocate_and_construct_unique_pointer(nullptr))
      {
         //std::cout << " constructong " << std::endl;
      }
      
      ~test_lol()
      {
         //std::cout << " destroying " << std::endl;
         allocator_tp::deallocate(p, n);
         allocator_tpp::deallocate(pp, n);
      }

      double& operator[](int i)
      {
         return p[i];
      }
};

template
   <  class T = double
   ,  class Alloc = memalloc::allocator<T> 
   >
class test_none
   :  private memalloc::allocatable<T, Alloc>
{
};

template
   <  class T = double
   ,  class Alloc = memalloc::allocator<T> 
   >
class test_tp
   :  private memalloc::allocatable<T, Alloc>
{
};

template
   <  class T = double
   ,  class Alloc = memalloc::allocator<T> 
   >
class test_tp_tpp
   :  private memalloc::allocatable<T, Alloc>
   ,  private memalloc::allocatable<T*, Alloc>
{
};

int main()
{
   //using test_type = test_lol<double>;
   using test_type = test_lol<double, memalloc::mempool_allocator<double> >;
   
   memalloc::allocator<double> alloc;
   //decltype(alloc)::template rebind<test_type >::other alloc3;
   //memalloc::allocator<double, memalloc::mempool_alloc_policy<double> > alloc2;
   memalloc::allocator<double, memalloc::mempool_alloc_policy<double> > alloc2;
   decltype(alloc2)::template rebind<test_type >::other alloc3;
   //int n = 1;
   //int nrepeat = 1;
   int n = 10000000;
   int nrepeat = 10;
   double* ptr;
   int size = 10000;

   //std::cout << sizeof(test_none<>) << std::endl;
   //std::cout << sizeof(test_tp<>) << std::endl;
   //std::cout << sizeof(test_tp_tpp<>) << std::endl;
   //std::cout << sizeof(test_tp<double, memalloc::mempool_allocator<double> >) << std::endl;
   //std::cout << sizeof(test_tp_tpp<double, memalloc::mempool_allocator<double> >) << std::endl;
   
   //test_lol<double, memalloc::mempool_alloc_policy<double> > test;
   ////test_lol<double, memalloc::mempool_alloc_policy<double> > test2 = test;

   //test[0] = 3.15;

   //std::cout << test[0] << std::endl;

   //auto sptr  = memalloc::allocate_unique_ptr<double[]>(alloc, 10);
   //auto sptr2 = memalloc::allocate_unique_ptr<double[]>(alloc2, 10);
   //auto sptr3 = memalloc::allocate_unique_ptr<double[]>(alloc, 12);

   //std::swap(sptr, sptr3);

   //sptr[0] = 3.3333;
   //std::cout << sptr[0] << std::endl;
   
   for(int irepeat = 0; irepeat < nrepeat; ++irepeat)
   {
      timer t;
      for(int i = 0; i < n; ++i)
      {
         t.start();
         auto sptr = memalloc::allocate_and_construct_unique_pointer<test_type>(alloc3, nullptr, 10);
         
         //auto sptr = memalloc::allocate_unique_ptr<test_type>(alloc3, 1, nullptr, 10);
         //alloc3.construct(sptr.get(), 10);
         //alloc3.destroy(sptr.get());
         
         //auto sptr = std::unique_ptr<double[]>(new double[size]);
         //test_lol<double, memalloc::mempool_allocator<double> > test(size);
         //test_lol<double> test(size);
         //deleted_unique_ptr<double, decltype(alloc)> ptr(size);
         //ptr = alloc.allocate(size);
         //alloc.deallocate(ptr, size);
         //ptr = new double[size];
         //delete[] ptr;
         t.stop();
      }
      std::cout << " Average time : " << t.average_time<std::milli>() << " ms" << std::endl;
   }


   return 0;
}
