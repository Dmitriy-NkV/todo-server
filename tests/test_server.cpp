#include "test_utils.hpp"

namespace tests
{
  TEST_F(TestServerFixture, GetEmptyTasks)
  {
    HttpClient client("127.0.0.1", 9000);
    http::response<boost::beast::http::string_body> response;
    ASSERT_NO_THROW(response = client.request(http::verb::get, "/tasks"));

    ASSERT_EQ(response.result(), http::status::ok);
    ASSERT_EQ(response[http::field::content_type], "application/json");

    auto json = nlohmann::json::parse(response.body());
    EXPECT_TRUE(json.is_array());
    EXPECT_TRUE(json.empty());
  }
}
