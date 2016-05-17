#include "multiswitch.hpp"

#include <hayai/hayai.hpp>

#include <iostream>
#include <assert.h>

BENCHMARK(MultiswitchBenchmarks, Multiswitch, 10, 1000000)
{
  using namespace multiswitch;
  int x = 1, y = 3;
  std::string s2 = "case";
  int result;
  auto b = _switch<int, std::string>()
          <= _case(2, "b")    ([&result]  {result += 0;})
          <= _case(1, _)      ([&result]  {result += 1;})
          <= _case(1, "case") ([&result]  {result += 2;})
          <= _case(_, _)      ([&result]  {result += 3;});

  b(2, "b");
  b(x, s2);
  b(8, "sxssxsxs");
  assert(result == 5);
}

int doComparision(int i, const std::string& str)
{
  if(i == 2 and str == "bb")
    return 0;
  else if(i == 1)
    return 1;
  else if(i == 1 and str == "case")
    return 2;
  return 3;
}

BENCHMARK(MultiswitchBenchmarks, if_else, 10, 1000000)
{
  int x = 1, y = 3;
  std::string s2 = "case";

  int result = doComparision(2, "b") + doComparision(x, s2) + doComparision(8, "sxssxsxs");
  assert(result == 5);
}

int main()
{
  hayai::ConsoleOutputter consoleOutputter;
  hayai::Benchmarker::AddOutputter(consoleOutputter);
  hayai::Benchmarker::RunAllTests();


  std::cout << "\n....... Tests ....... \n\n";

  using namespace multiswitch;
  int x = 1, y = 3;
  auto a = _switch<int, int>()
          <= _case(1,0)([]{std::cout << "Hello, World 1-0!" << std::endl;})
          <= _case(1,1)([]{std::cout << "Hello, World 1-1" << std::endl;})
          <= _case(1,3)([]{std::cout << "Bingo 1-3" << std::endl;});

  a(x,y);

  std::string s2 = "case";
  auto b = _switch<int, std::string>()
          <= _case(2, "b")([]{std::cout << "2-b" << std::endl;})
          <= _case(1, _)([]{std::cout << "1-_" << std::endl;})
          <= _case(1, "case")([]{std::cout << "1-case" << std::endl;})
          <= _case(_, _)([]{std::cout << "_-_" << std::endl;});

  b(2, "b");
  b(x, s2);
  b(8, "sxssxsxs");


  return 0;
}
