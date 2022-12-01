There are a lot of issues with the singleton pattern. I'm totally aware of that. But the singleton pattern is an ideal use case for a variable, which has only to be initialized in a thread-safe way. From that point on you can use it without synchronization. So in this post, I discuss different ways to initialize a singleton in a multithreading environment. You get the performance numbers and can reason about your uses cases for the thread-safe initialization of a variable.

There are a lot of different ways to initialize a singleton in C++11 in a thread-safe way. From a birds-eye, you can have guarantees from the C++ runtime, locks or atomics. I'm totally curious about the performance implications.

My strategy
-----------

I use as a reference point for my performance measurement a singleton object which I sequential access 40 million times. The first access will initialize the object. In contrast, the access from the multithreading program will be done by 4 threads. Here I'm only interested in the performance. The program will run on two real PCs. My Linux PC has four, my Windows PC has two cores. I compile the program with maximum and without optimization. For the translation of the program with maximum optimization, I have to use a volatile variable in the static method getInstance. If not the compiler will optimize away my access to the singleton and my program becomes too fast.

I have three questions in my mind:

1.  How is the relative performance of the different singleton implementations?
2.  Is there a significant difference between Linux (gcc) and Windows (cl.exe)?
3.  What's the difference between the optimized and non-optimized versions?

Finally, I collect all numbers in a table. The numbers are in seconds.

The reference values
--------------------

### The both compilers

The command line gives you the details of the compiler Here are the gcc and the cl.exe.

![](https://www.modernescpp.com/images/blog/Threads/Singleton/gcc.png)
![](https://www.modernescpp.com/images/blog/Threads/Singleton/cl_exe.PNG)

### The reference code

At first, the single-threaded case. Of course without synchronization.

```c++
// singletonSingleThreaded.cpp

#include <chrono>
#include <iostream>

constexpr auto tenMill= 10000000;

class MySingleton{
public:
  static MySingleton& getInstance(){
    static MySingleton instance;
    // volatile int dummy{};
    return instance;
  }
private:
  MySingleton()= default;
  ~MySingleton()= default;
  MySingleton(const MySingleton&)= delete;
  MySingleton& operator=(const MySingleton&)= delete;

};

int main(){
    
  constexpr auto fourtyMill= 4* tenMill;

  auto begin= std::chrono::system_clock::now();

  for ( size_t i= 0; i <= fourtyMill; ++i){
       MySingleton::getInstance();
  }

  auto end= std::chrono::system_clock::now() - begin;

  std::cout << std::chrono::duration<double>(end).count() << std::endl;

}
```

I use in the reference implementation the so-called Meyers Singleton. The elegance of this implementation is that the singleton object instance in line 11 is a static variable with a block scope. Therefore, instance will exactly be initialized, when the static method `getInstance` (line 10 - 14) will be executed the first time. In line 14 the volatile variable dummy is commented out. When I translate the program with maximum optimization that has to change. So the call `MySingleton::getInstance()` will not be optimized away. 

Now the raw numbers on Linux and Windows.

#### Without optimization

![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletonSingleThreaded.png)
![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletonSingleThreaded_win.png)

#### Maximum Optimization

 ![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletonSingleThreaded_opt.png)
![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletonSingleThreaded_win_opt.png)

## Meyers Singleton

The beauty of the Meyers Singleton in C++11 is that it's automatically thread-safe. That is guaranteed by the standard: [Static variables with block scope.](https://www.modernescpp.com/index.php/thread-safe-initialization-of-data) The Meyers Singleton is a static variable with block scope, so we are done. It's still left to rewrite the program for four threads.



```c++
// singletonMeyers.cpp

#include <chrono>
#include <iostream>
#include <future>

constexpr auto tenMill= 10000000;

class MySingleton{
public:
  static MySingleton& getInstance(){
    static MySingleton instance;
    // volatile int dummy{};
    return instance;
  }
private:
  MySingleton()= default;
  ~MySingleton()= default;
  MySingleton(const MySingleton&)= delete;
  MySingleton& operator=(const MySingleton&)= delete;

};

std::chrono::duration<double> getTime(){

  auto begin= std::chrono::system_clock::now();
  for ( size_t i= 0; i <= tenMill; ++i){
      MySingleton::getInstance();
  }
  return std::chrono::system_clock::now() - begin;

};

int main() {
auto fut1= std::async(std::launch::async,getTime);
auto fut2= std::async(std::launch::async,getTime);
auto fut3= std::async(std::launch::async,getTime);
auto fut4= std::async(std::launch::async,getTime);

auto total= fut1.get() + fut2.get() + fut3.get() + fut4.get();

std::cout << total.count() << std::endl;
}
```

I use the singleton object in the function `getTime` (line 24 - 32). The function is executed by the four [promise](https://www.modernescpp.com/index.php/asynchronous-function-calls) in line 36 - 39. The results of the associate [futures](https://www.modernescpp.com/index.php/asynchronous-function-calls) are summed up in line 41. That's all. Only the execution time is missing.

#### Without optimization

 ![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletonMeyers.png)
![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletonMeyers_win.png)

#### Maximum optimization

![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletonMeyers_opt.png)
![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletonMeyers_win_opt.png)

The function `std::call_once`
---------------------------------------------------------

You can use the function [std::call_once](https://www.modernescpp.com/index.php/thread-safe-initialization-of-data) to register a callable which will be executed exactly once. The flag std::call_once in the following implementation guarantees that the singleton will be thread-safe initialized.

```c++
// singletonCallOnce.cpp

#include <chrono>
#include <iostream>
#include <future>
#include <mutex>
#include <thread>

constexpr auto tenMill= 10000000;

class MySingleton{
public:
  static MySingleton& getInstance(){
    std::call_once(initInstanceFlag, &MySingleton::initSingleton);
    // volatile int dummy{};
    return *instance;
  }
private:
  MySingleton()= default;
  ~MySingleton()= default;
  MySingleton(const MySingleton&)= delete;
  MySingleton& operator=(const MySingleton&)= delete;

  static MySingleton* instance;
  static std::once_flag initInstanceFlag;

  static void initSingleton(){
    instance= new MySingleton;
  }
};

MySingleton* MySingleton::instance= nullptr;
std::once_flag MySingleton::initInstanceFlag;

std::chrono::duration<double> getTime(){

  auto begin= std::chrono::system_clock::now();
  for ( size_t i= 0; i <= tenMill; ++i){
      MySingleton::getInstance();
  }
  return std::chrono::system_clock::now() - begin;

};

int main() {
auto fut1= std::async(std::launch::async,getTime);
auto fut2= std::async(std::launch::async,getTime);
auto fut3= std::async(std::launch::async,getTime);
auto fut4= std::async(std::launch::async,getTime);

auto total= fut1.get() + fut2.get() + fut3.get() + fut4.get();

std::cout << total.count() << std::endl;
}
```

Here are the numbers.

#### Without optimization

![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletoCallOnce.png)
![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletoCallOnce_win.png)

#### Maximum optimization

![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletoCallOnce_opt.png)
![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletoCallOnce_win_opt.png)

Of course, the most obvious way is it protects the singleton with a lock.

Lock
----

The mutex wrapped in a [lock](https://www.modernescpp.com/index.php/prefer-locks-to-mutexes) guarantees that the singleton will be thread-safe initialized.

```c++
// singletonLock.cpp

#include <chrono>
#include <iostream>
#include <future>
#include <mutex>

constexpr auto tenMill= 10000000;

std::mutex myMutex;

class MySingleton{
public:
  static MySingleton& getInstance(){
    std::lock_guard<std::mutex> myLock(myMutex);
    if ( !instance ){
        instance= new MySingleton();
    }
    // volatile int dummy{};
    return *instance;
  }
private:
  MySingleton()= default;
  ~MySingleton()= default;
  MySingleton(const MySingleton&)= delete;
  MySingleton& operator=(const MySingleton&)= delete;

  static MySingleton* instance;
};


MySingleton* MySingleton::instance= nullptr;

std::chrono::duration<double> getTime(){

  auto begin= std::chrono::system_clock::now();
  for ( size_t i= 0; i <= tenMill; ++i){
       MySingleton::getInstance();
  }
  return std::chrono::system_clock::now() - begin;

};

int main(){
auto fut1= std::async(std::launch::async,getTime);
auto fut2= std::async(std::launch::async,getTime);
auto fut3= std::async(std::launch::async,getTime);
auto fut4= std::async(std::launch::async,getTime);

auto total= fut1.get() + fut2.get() + fut3.get() + fut4.get();

std::cout << total.count() << std::endl;
}
```
How fast is the classical thread-safe implementation of the singleton pattern?

#### Without optimization

 ![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletonLock.png)
![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletonLock_win.png)

#### Maximum optimization

![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletonLock_opt.png)
![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletonLock_win_opt.png)

Not so fast. Atomics should make the difference.

Atomic variables
----------------

With atomic variables, my job becomes extremely challenging. Now I have to use the [C++ memory model](https://www.modernescpp.com/index.php/c-memory-model). I base my implementation on the well-known [double-checked locking pattern.](https://www.modernescpp.com/index.php/thread-safe-initialization-of-data)

### Sequential consistency

The handle to the singleton is atomic. Because I didn't specify the C++ memory model the default applies: [Sequential consistency.](https://www.modernescpp.com/index.php/sequential-consistency)

```c++
// singletonAcquireRelease.cpp

#include <atomic>
#include <iostream>
#include <future>
#include <mutex>
#include <thread>

constexpr auto tenMill= 10000000;

class MySingleton{
public:
  static MySingleton* getInstance(){
    MySingleton* sin= instance.load();
    if ( !sin ){
      std::lock_guard<std::mutex> myLock(myMutex);
      sin= instance.load();
      if( !sin ){
        sin= new MySingleton();
        instance.store(sin);
      }
    }   
    // volatile int dummy{};
    return sin;
  }
private:
  MySingleton()= default;
  ~MySingleton()= default;
  MySingleton(const MySingleton&)= delete;
  MySingleton& operator=(const MySingleton&)= delete;

  static std::atomic<MySingleton*> instance;
  static std::mutex myMutex;
};


std::atomic<MySingleton*> MySingleton::instance;
std::mutex MySingleton::myMutex;

std::chrono::duration<double> getTime(){

  auto begin= std::chrono::system_clock::now();
  for ( size_t i= 0; i <= tenMill; ++i){
       MySingleton::getInstance();
  }
  return std::chrono::system_clock::now() - begin;

};

int main(){
auto fut1= std::async(std::launch::async,getTime);
auto fut2= std::async(std::launch::async,getTime);
auto fut3= std::async(std::launch::async,getTime);
auto fut4= std::async(std::launch::async,getTime);

auto total= fut1.get() + fut2.get() + fut3.get() + fut4.get();

std::cout << total.count() << std::endl;
}
```

Now I'm curious.

#### Without optimization

 ![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletonSequentialConsistency.png)
![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletonSequentialConsistency_win.png)

#### Maximum optimization

![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletonSequentialConsistency_opt.png)
![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletonSequentialConsistency_win_opt.png)

But we can do better. There is an additional optimization possibility.

### Acquire-release Semantic

The reading of the singleton (line 14) is an acquire operation, the writing a release operation (line 20). Because both operations take place on the same atomic I don't need sequential consistency. The C++ standard guarantees that an acquire operation synchronizes with a release operation on the same atomic. These conditions hold in this case therefore I can weaken the C++ memory model in line 14 and 20. [Acquire-release semantic](https://www.modernescpp.com/index.php/acquire-release-semantic) is sufficient.

```c++
// singletonAcquireRelease.cpp

#include <atomic>
#include <iostream>
#include <future>
#include <mutex>
#include <thread>

constexpr auto tenMill= 10000000;

class MySingleton{
public:
  static MySingleton* getInstance(){
    MySingleton* sin= instance.load(std::memory_order_acquire);
    if ( !sin ){
      std::lock_guard<std::mutex> myLock(myMutex);
      sin= instance.load(std::memory_order_relaxed);
      if( !sin ){
        sin= new MySingleton();
        instance.store(sin,std::memory_order_release);
      }
    }   
    // volatile int dummy{};
    return sin;
  }
private:
  MySingleton()= default;
  ~MySingleton()= default;
  MySingleton(const MySingleton&)= delete;
  MySingleton& operator=(const MySingleton&)= delete;

  static std::atomic<MySingleton*> instance;
  static std::mutex myMutex;
};


std::atomic<MySingleton*> MySingleton::instance;
std::mutex MySingleton::myMutex;

std::chrono::duration<double> getTime(){

  auto begin= std::chrono::system_clock::now();
  for ( size_t i= 0; i <= tenMill; ++i){
       MySingleton::getInstance();
  }
  return std::chrono::system_clock::now() - begin;

};

int main(){
auto fut1= std::async(std::launch::async,getTime);
auto fut2= std::async(std::launch::async,getTime);
auto fut3= std::async(std::launch::async,getTime);
auto fut4= std::async(std::launch::async,getTime);

auto total= fut1.get() + fut2.get() + fut3.get() + fut4.get();

std::cout << total.count() << std::endl;
}
```

The acquire-release semantic has a similar performance as the sequential consistency. That's not surprising, because on x86 both memory models are very similar. We would get totally different numbers on an ARMv7 or PowerPC architecture. You can read the details on Jeff Preshings blog [Preshing on Programming](http://preshing.com/).

#### Without optimization

 ![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletonAcquireRelease.png)
![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletonAcquireRelease_win.png)

#### Maximum optimization

![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletonAcquireRelease_opt.png)
![](https://www.modernescpp.com/images/blog/Threads/Singleton/singletonAcquireRelease_win_opt.png)
.

If I forget an import variant of the thread-safe singleton pattern, please let me know and send me the code. I will measure it and add the numbers to the comparison.

All numbers at one glance
-------------------------

Don't take the numbers too seriously. I executed each program only once and the executable is optimized for four cores on my two core windows PC. But the numbers give a clear indication. The Meyers Singleton is the easiest to get and the fastest one. In particular, the lock-based implementation is by far the slowest one. The numbers are independent of the used platform.

But the numbers show more. Optimization counts. This statement holds not totally true for the std::lock_guard based implementation of the singleton pattern.

![](https://www.modernescpp.com/images/blog/Threads/Singleton/comparisonSingletonEng.png)
