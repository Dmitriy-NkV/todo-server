#include "server.hpp"
#include <iostream>

int main()
{
  auto db = std::make_shared< database::Database >("host=localhost dbname=todoapp user=postgres password=admin");
  server::Server server("127.0.0.1", 8080, 1, db);

  server.start();
  while (std::cin)
  {
    continue;
  }

  return 0;
}
