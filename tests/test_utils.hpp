#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <boost/beast/version.hpp>
#include "database.hpp"
#include "server.hpp"

namespace tests
{
  class TestDatabaseFixture: public ::testing::Test
  {
  protected:
    void SetUp() override
    {
      std::string db_host = "localhost";
      std::string db_port =  "5432";
      std::string db_name = "dbtest";
      std::string db_user = "postgres";
      std::string db_password = "admin";

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
    TestServerFixture():
      server_host_("127.0.0.1"),
      server_port_(9000),
      threads_num_(2)
    {
      server_ = std::make_unique< server::Server >(server_host_, server_port_, threads_num_);

    }

    void SetUp() override
    {
      TestDatabaseFixture::SetUp();

      server_host_ = "127.0.0.1";
      server_port_ = 9000;
      threads_num_ = 2;

      server_ = std::make_unique< server::Server >(server_host_, server_port_, threads_num_);

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
      port_(port)
    {}

    http::response< http::string_body > request(http::verb method, const std::string& target, const nlohmann::json& body = {})
    {
      http::request< http::string_body > req(method, target, 11);
      req.set(http::field::host, host_);
      req.set(http::field::user_agent, "Client");

      if (!body.empty())
      {
        req.set(http::field::content_type, "application/json");
        req.body() = body.dump();
        req.prepare_payload();
      }

      return connect(req);
    }

  private:
    std::string host_;
    unsigned short port_;

    http::response< http::string_body > connect(const http::request< http::string_body >& req)
    {
      net::io_context ioc;
      tcp::resolver resolver(ioc);
      beast::tcp_stream stream(ioc);

      auto const results = resolver.resolve(host_, std::to_string(port_));
      stream.connect(results);
      http::write(stream, req);

      beast::flat_buffer buffer;
      http::response< http::string_body > res;
      http::read(stream, buffer, res);
      beast::error_code ec;
      stream.socket().shutdown(tcp::socket::shutdown_both, ec);

      if (ec && ec != beast::errc::not_connected)
      {
        throw beast::system_error(ec);
      }

      return res;
    }
  };
}
