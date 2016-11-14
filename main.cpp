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
#include "allocate_unique_ptr.hpp"


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
   :  public memalloc::allocatable<T, Alloc>
   ,  public memalloc::allocatable<T*, Alloc>
{
   using allocator_tp = memalloc::allocatable<T, Alloc>;
   using allocator_tpp = memalloc::allocatable<T*, Alloc>;

   std::size_t n;
   std::vector<int, Alloc> lol;
   double* p;
   double** pp;
   typename allocator_tp::unique_array sptr;
   typename allocator_tp::unique_pointer sptr2;

   public:
      test_lol(std::size_t n_)
         : n(n_)
         , lol(n)
         , p(allocator_tp::allocate(n))
         , pp(allocator_tpp::allocate(n))
         , sptr(allocator_tp::allocate_unique_array(n))
         , sptr2(allocator_tp::allocate_unique_pointer())
      {
      }
      
      ~test_lol()
      {
         allocator_tp::deallocate(p, n);
         allocator_tpp::deallocate(pp, n);
      }

      double& operator[](int i)
      {
         return p[i];
      }
};

int main()
{
   timer t;
   memalloc::allocator<double> alloc;
   //memalloc::allocator<double, memalloc::mempool_alloc_policy<double> > alloc2;
   memalloc::allocator<double, memalloc::mempool_alloc_policy<double> > alloc2;
   int n = 100000;
   double* ptr;
   int size = 10000;
   
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

   for(int i = 0; i < n; ++i)
   {
      t.start();
      test_lol<double, memalloc::mempool_allocator<double> > test(size);
      //test_lol<double> test(size);
      //deleted_unique_ptr<double, decltype(alloc)> ptr(size);
      //ptr = alloc.allocate(size);
      //alloc.deallocate(ptr, size);
      //ptr = new double[size];
      //delete[] ptr;
      t.stop();
   }

   std::cout << " Average time : " << t.average_time<std::milli>() << " ms" << std::endl;

   return 0;
}
