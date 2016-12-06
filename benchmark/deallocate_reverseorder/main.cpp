#include <iostream>
#include <memory>
#include <functional>
#include <type_traits>
#include <cassert>
#include <random>

#include "../commandline/commandline.hpp"

#include "../../timer.hpp"
#include "../../../../libmda/util/type_info.h"

#define __gcc_header(x) #x
#define _gcc_header(x) __gcc_header(../../x.hpp)
#define gcc_header(x) _gcc_header(x)
#include gcc_header(ALLOCATOR)

int main(int argc, char* argv[])
{
   auto cc = commandline::parser()
      .option<int>("max_size", "-max")
      .option<int>("n", "-n")
      .option<int>("nrepeat", "-nrepeat")
      .parse(argc, argv)
      ;
   
   int max_size = cc.get<int>("max_size");
   int n = cc.get<int>("n");
   int nrepeat = cc.get<int>("nrepeat");

   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_int_distribution<> dis_size(0, max_size - 1);

   memalloc::ALLOCATOR<double> alloc2;
   
   // print header
   std::cout << " Benchmark Butterfly : " << libmda::util::typeof<memalloc::ALLOCATOR<double> >() << std::endl;

   // loop over nrepeat
   for(int irepeat = 0; irepeat < nrepeat; ++irepeat)
   {
      std::vector<double*> vec(n, nullptr);
      std::vector<int> lol_size(n, -1);

      // start timer 
      timer t;
      t.start();
      for(std::size_t i = 0; i < vec.size(); ++i)
      {
         auto random_size = dis_size(gen);
         lol_size[i] = random_size;
         vec[i] = alloc2.allocate(lol_size[i]);
         for(int j = 0; j < lol_size[i]; ++j)
         {
               vec[i][j] = 0x13371337;
         }
      }

      for(std::size_t i = vec.size(); i > 0; --i)
      {
         alloc2.deallocate(vec[i-1], lol_size[i-1]);
      }

      // stop timer
      t.stop();

      // print timing
      std::cout << " Average time : " << t.average_time<std::milli>() << " ms" << std::endl;
   }


   return 0;
}
