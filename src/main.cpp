#include "server.hpp"
#include <iostream>
#include <cstdlib>

int main()
{
  try
  {
    std::string db_host = std::getenv("DB_HOST") ? std::getenv("DB_HOST") : "localhost";
    std::string db_port = std::getenv("DB_PORT") ? std::getenv("DB_PORT") : "5432";
    std::string db_name = std::getenv("DB_NAME") ? std::getenv("DB_NAME") : "todoapp";
    std::string db_user = std::getenv("DB_USER") ? std::getenv("DB_USER") : "postgres";
    std::string db_password = std::getenv("DB_PASSWORD") ? std::getenv("DB_PASSWORD") : "admin";

    std::string connection_string = "host=" + db_host +
      " port=" + db_port +
      " dbname=" + db_name +
      " user=" + db_user +
      " password=" + db_password;

    auto db = std::make_shared< database::Database >(connection_string);

    db->initialize_database();

    server::Server server("0.0.0.0", 8000, 1, db);
    server.start();
    std::string line;
    while (std::getline(std::cin, line))
    {
      if (line == "exit")
      {
        break;
      }
    }

    server.stop();
  }
  catch (const std::exception& e)
  {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
  }

  return 0;
}
