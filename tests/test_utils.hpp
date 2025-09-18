#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "database.hpp"
#include "server.hpp"

namespace tests
{
  class TestDatabaseFixture: public ::testing::Test
  {
  protected:
    void SetUp() override
    {
      std::string db_host = std::getenv("DB_HOST") ? std::getenv("DB_HOST") : "localhost";
      std::string db_port = std::getenv("DB_PORT") ? std::getenv("DB_PORT") : "5432";
      std::string db_name = std::getenv("DB_NAME") ? std::getenv("DB_NAME") : "dbtest";
      std::string db_user = std::getenv("DB_USER") ? std::getenv("DB_USER") : "postgres";
      std::string db_password = std::getenv("DB_PASSWORD") ? std::getenv("DB_PASSWORD") : "admin";

      connection_string_ = "host=" + db_host +
        " port=" + db_port +
        " dbname=" + db_name +
        " user=" + db_user +
        " password=" + db_password;

      db_ = std::make_shared< database::Database >(connection_string_);

      try
      {
        setup_test_database();
      }
      catch(const std::exception& e)
      {
        GTEST_SKIP() << "Can't connect to database " << e.what();
      }
    }

    void TearDown() override
    {
      cleanup_test_database();
    }

    void setup_test_database()
    {
      pqxx::connection connection(connection_string_);
      pqxx::work txn(connection);

      txn.exec("DROP TABLE IF EXISTS tasks CASCADE");
      txn.commit();

      db_->initialize_database();
    }

    void cleanup_test_database()
    {
      pqxx::connection connection(connection_string_);
      pqxx::work txn(connection);

      txn.exec("DROP TABLE IF EXISTS tasks CASCADE");
      txn.commit();
    }

    std::string connection_string_;
    std::shared_ptr< database::Database > db_;
  };

  class TestServerFixture: public TestDatabaseFixture
  {
  protected:
    void SetUp() override
    {
      TestDatabaseFixture::SetUp();

      server_host_ = "127.0.0.1";
      server_port_ = 9000;
      threads_num_ = 2;

      server_ = std::make_unique< server::Server >(server_host_, server_port_, threads_num_, db_);

      server_->start();

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void TearDown() override
    {
      server_->stop();
      TestDatabaseFixture::TearDown();
    }

    std::string make_url(const std::string& target) const
    {
      return "http://" + server_host_ + ':' + std::to_string(server_port_) + target;
    }

    std::string server_host_;
    unsigned short server_port_;
    size_t threads_num_;
    std::unique_ptr< server::Server > server_;
  };

  class HttpClient
  {
  public:
    HttpClient(const std::string& host, unsigned short port):
      host_(host),
      port_(port),
      ioc_(),
      stream_(ioc_),
      connected_(false)
    {}

    ~HttpClient()
    {
      disconnect();
    }

    http::response< http::string_body > request(http::verb method, const std::string& target, const nlohmann::json& body = {})
    {
      if (!connected_)
      {
        connect();
      }

      http::request<http::string_body> req(method, target, 11);
      req.set(http::field::host, host_);
      req.set(http::field::user_agent, "Client");
      req.set(http::field::connection, "keep-alive");

      if (!body.empty())
      {
        req.set(http::field::content_type, "application/json");
        req.body() = body.dump();
        req.prepare_payload();
      }

      stream_.expires_after(std::chrono::seconds(5));
      http::write(stream_, req);

      beast::flat_buffer buffer;
      http::response< http::string_body > res;

      stream_.expires_after(std::chrono::seconds(5));
      http::read(stream_, buffer, res);

      if (res[http::field::connection] == "close")
      {
        disconnect();
      }
      return res;
    }

  private:
    std::string host_;
    unsigned short port_;
    net::io_context ioc_;
    beast::tcp_stream stream_;
    bool connected_;

    void connect()
    {
      tcp::resolver resolver(ioc_);
      auto const results = resolver.resolve(host_, std::to_string(port_));

      stream_.expires_after(std::chrono::seconds(5));
      stream_.connect(results);

      connected_ = true;
    }

    void disconnect()
    {
      if (connected_)
      {
        beast::error_code ec;
        stream_.socket().shutdown(tcp::socket::shutdown_both, ec);
        connected_ = false;
      }
    }
  };
}
