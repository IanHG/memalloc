#include <iostream>
#include <memory>
#include <functional>
#include <type_traits>
#include <cassert>
#include <random>

//#include "../../libmda/util/stacktrace.h"

#include "../include/timer.hpp"
#include "../include/allocator.hpp"
#include "../include/allocatable.hpp"
#include "../include/mempool_allocator.hpp"
#include "../include/mempool_alloc_policy.hpp"
#include "../include/allocate_smart.hpp"
#include "../include/allocator_traits.hpp"


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
   T* p;
   T** pp;
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
class test_lol_lol
   :  public test_lol<T, Alloc>
{
   using base = test_lol<T, Alloc>;
   T* ptr = base::allocator_tp::allocate(1, nullptr);
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

template<class T>
using vector_type = std::vector<T, memalloc::mempool_allocator<T> >;

int main()
{
   int vec_size = 10000;
   int max_size = 10000;
   //std::cout << (void*)nullptr << std::endl;
   //vector_type<double> vec0;
   //{
   //vector_type<double> vec1(vec_size);
   //vec0.resize(vec_size);
   //vector_type<double> vec2(vec_size);

   //for(int i = 0; i < vec_size; ++i)
   //{
   //   vec1[i] = vec2[i] = double(i);
   //   vec0[i] = vec1[i] + vec2[i];
   //}
   //}

   //for(int i = 0; i < vec_size; ++i)
   //{
   //   std::cout << vec0[i] << std::endl;
   //}

   std::random_device rd;
   std::mt19937 gen(rd());
   //std::mt19937 gen(0);
   std::uniform_int_distribution<> dis(0, vec_size - 1);
   std::uniform_int_distribution<> dis_size(0, max_size - 1);

   //using test_type = test_lol<double>;
   using test_type = test_lol<double, memalloc::mempool_allocator<double> >;
   
   memalloc::allocator<double> alloc;
   decltype(alloc)::template rebind<test_type >::other alloc3;
   memalloc::allocator<double, memalloc::mempool_alloc_policy<double> > alloc2;
   //memalloc::allocator<double> alloc2;
   //decltype(alloc2)::template rebind<test_type >::other alloc3;
   //int n = 10;
   //int nrepeat = 2;
   int n = 1000000;
   //int n = 100;
   int nrepeat = 5;
   //double* ptr;
   //int size = 10000;

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
      std::vector<double*> lol(vec_size, nullptr);
      std::vector<int> lol_size(vec_size, -1);
      timer t;
      t.start();
      for(int i = 0; i < n; ++i)
      {
         auto random = dis(gen);
         auto random_size = dis_size(gen);
         //random_size = 10;
         //std::cout << "random " << random << std::endl;
         //std::cout << "random size " << random_size << std::endl;
         if(lol[random] == nullptr)
         {
            lol_size[random] = random_size;
            //std::cout << " ++++ allocating +++++ " << std::endl;
            lol[random] = alloc2.allocate(lol_size[random]);
            //std::cout << " lol pointer " << lol[random] << std::endl;
            //std::cout << " lol size    " << lol_size[random] << std::endl;
            //std::cout << " random_size " << random_size << std::endl;
            //std::cout << " ++++ allocating end +++++ " << std::endl;
            for(int j = 0; j < lol_size[random]; ++j)
            {
               //if(lol_size[random] == 844)
               //{
               //   std::cout << "j " << j << "   " << lol[random] + j << "  " << reinterpret_cast<uintptr_t>(lol[random] + j) << std::endl;
               //   //std::cout << lol[random] + j << std::endl;
               //}
               //std::cout << j << std::endl;
               lol[random][j] = 0x13371337;
            }
         }
         else
         {
            //std::cout << " ---- deallocating ---- " << std::endl;
            alloc2.deallocate(lol[random], lol_size[random]);
            lol[random] = nullptr;
            lol_size[random] = -1;
            //std::cout << " ---- deallocating end ---- " << std::endl;
         }
      }

      for(std::size_t i = 0; i < lol.size(); ++i)
      {
         if(lol[i])
         {
            alloc2.deallocate(lol[i], lol_size[i]);
         }
      }
      t.stop();
      //std::vector<decltype(memalloc::allocate_and_construct_unique_pointer<test_type>(alloc3, nullptr, 0))> sptr_vec;
      //for(int i = 0; i < n; ++i)
      //{
      //   t.start();
      //   //auto sptr = memalloc::allocate_and_construct_unique_pointer<test_type>(alloc3, nullptr, 10);
      //   sptr_vec.emplace_back(memalloc::allocate_and_construct_unique_pointer<test_type>(alloc3, nullptr, 10));
      //   
      //   //auto sptr = memalloc::allocate_unique_ptr<test_type>(alloc3, 1, nullptr, 10);
      //   //alloc3.construct(sptr.get(), 10);
      //   //alloc3.destroy(sptr.get());
      //   
      //   //auto sptr = std::unique_ptr<double[]>(new double[size]);
      //   //test_lol<double, memalloc::mempool_allocator<double> > test(size);
      //   //test_lol<double> test(size);
      //   //deleted_unique_ptr<double, decltype(alloc)> ptr(size);
      //   //ptr = alloc.allocate(size);
      //   //alloc.deallocate(ptr, size);
      //   //ptr = new double[size];
      //   //delete[] ptr;
      //   t.stop();
      //}
      std::cout << " Average time : " << t.average_time<std::milli>() << " ms" << std::endl;
   }


   return 0;
}
