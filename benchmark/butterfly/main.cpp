#include <iostream>
#include <memory>
#include <functional>
#include <type_traits>
#include <cassert>
#include <random>

#include "../commandline/commandline.hpp"

#include "../../include/timer.hpp"
#include "../../../../libmda/util/type_info.h"

#define __gcc_header(x) #x
#define _gcc_header(x) __gcc_header(../../include/x.hpp)
#define gcc_header(x) _gcc_header(x)
#include gcc_header(ALLOCATOR)

int main(int argc, char* argv[])
{
   auto cc = commandline::parser()
      .option<int>("vec_size", "-vec")
      .option<int>("max_size", "-max")
      .option<int>("n", "-n")
      .option<int>("nrepeat", "-nrepeat")
      .parse(argc, argv)
      ;
   
   int vec_size = cc.get<int>("vec_size");
   int max_size = cc.get<int>("max_size");
   int n = cc.get<int>("n");
   int nrepeat = cc.get<int>("nrepeat");

   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_int_distribution<> dis(0, vec_size - 1);
   std::uniform_int_distribution<> dis_size(0, max_size - 1);

   memalloc::ALLOCATOR<double> alloc2;
   
   // print header
   std::cout << " Benchmark Butterfly : " << libmda::util::typeof<memalloc::ALLOCATOR<double> >() << std::endl;

   // loop over nrepeat
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
         if(lol[random] == nullptr)
         {
            lol_size[random] = random_size;
            lol[random] = alloc2.allocate(lol_size[random]);
            for(int j = 0; j < lol_size[random]; ++j)
            {
               lol[random][j] = 0x13371337;
            }
         }
         else
         {
            alloc2.deallocate(lol[random], lol_size[random]);
            lol[random] = nullptr;
            lol_size[random] = -1;
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

      // print timing
      std::cout << " Average time : " << t.average_time<std::milli>() << " ms" << std::endl;
   }


   return 0;
}
