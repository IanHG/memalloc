#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <chrono>

class timer
{
   public:
      timer() {};

      void start()
      {
         if(!running_)
         {
            start_ = std::chrono::steady_clock::now();
            running_ = true;
         }
      }

      void stop()
      {
         if(running_)
         {
            stop_ = std::chrono::steady_clock::now();
            diff_ = stop_ - start_;
            accu_ += diff_;
            ++n_;
            running_ = false;
         }
      }

      template<class T>
      double average_time()
      {
         return std::chrono::duration<double, T>(accu_).count();
      }

   private:
      bool running_ = false;
      int n_ = 0;
      std::chrono::time_point<std::chrono::steady_clock> start_;
      std::chrono::time_point<std::chrono::steady_clock> stop_;
      std::chrono::duration<double> diff_ = std::chrono::duration<double>{0.0};
      std::chrono::duration<double> accu_ = std::chrono::duration<double>{0.0};
};

#endif /* TIMER_H_INCLUDED */
