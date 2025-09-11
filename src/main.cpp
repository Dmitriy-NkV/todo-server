#include "server.hpp"
#include <iostream>

int main()
{
  try
  {
    auto db = std::make_shared< database::Database >("host=localhost dbname=todoapp user=postgres password=admin");
    server::Server server("127.0.0.1", 8080, 1, db);
    server.start();
    while (std::cin)
    {
      continue;
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << '\n';
    return 1;
  }

  return 0;
}
