#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <istream>
#include <sstream>
#include <vector>
#include <iterator>

#include "commandline/commandline.hpp"

int main(int argc, char* argv[])
{
   auto cc = commandline::parser()
      .option<std::string>("benchmarkfile", "-f")
      .parse(argc, argv)
      ;

   auto filename = cc.get<std::string>("benchmarkfile");

   std::ifstream ifs(filename);
   std::string str;
   std::string cxx = "g++";
   std::string cxxflags = "-Wall -O3 -std=c++14";
   std::string libs = "../type_info.cpp ../../src/mempool.cpp";

   bool cleanup = true;
   
   bool success = true;
   std::cout << "*================================================================*" << std::endl;
   std::cout << "* START BENCHMARK " << std::endl;
   std::cout << "*================================================================*" << std::endl << std::endl;
   while(std::getline(ifs, str))
   {
      // if line starts with '!' we skip the test
      if(str.empty() || str[0] == '!') continue;
      
      // write test header
      std::cout << "******************************************************************" << std::endl;
      std::cout << "* " << str << std::endl;
      std::cout << "******************************************************************" << std::endl;
      bool partial_success = true;
      int return_value;
      
      std::istringstream iss{str};
      std::vector<std::string> tokens;
      std::string temp_str;
      while(std::getline(iss, temp_str, ';'))
      {
         tokens.emplace_back(temp_str);
      }

      auto dir = tokens[0];
      auto allocator = tokens[1];
      auto preprocessor = tokens[2];
      auto commandline_options = tokens[3];
      
      // change dir
      return_value = chdir(dir.c_str());
      return_value = std::system("pwd");

      // compile benchmark test
      std::string cc = cxx + " " + cxxflags + " -o main " + "-DALLOCATOR=" + allocator + " " + preprocessor + " main.cpp " + libs;
      std::cout << " CC: " << cc << std::endl;
      return_value = std::system(cc.c_str());
      if(return_value)
      {
         std::cout << " compilation failed : " << str << std::endl;
         partial_success = false;
      }

      // run benchmark
      std::string run_command = "./main " + commandline_options;
      std::cout << run_command << std::endl;
      std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
      return_value = std::system(run_command.c_str());
      if(return_value)
      {
         std::cout << " Benchmark run failed : " << str << std::endl;
         partial_success = false;
      }
      std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

      // output
      std::cout << "******************************************************************" << std::endl;
      std::cout << "* " << str << " " << (partial_success ? " ! SUCCESS ! " : " ! FAILED ! ") << std::endl;
      std::cout << "******************************************************************" << std::endl << std::endl;
      
      // clean-up
      if(partial_success && cleanup)
      {
         auto rm_command = std::string("rm main");
         return_value = std::system(rm_command.c_str());
      }
      

      // change back to 'main' dir
      return_value = chdir("..");
      success = success && partial_success;
      //return_value = std::system("pwd");
   }
   
   std::cout << "*================================================================*" << std::endl;
   if(success)
      std::cout << "* SUCCESS ! " << std::endl;
   else
      std::cout << "* FAILED ! " << std::endl;
   std::cout << "*================================================================*" << std::endl;

   return 0;
}
