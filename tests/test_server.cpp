#include "test_utils.hpp"

namespace tests
{
  TEST_F(TestServerFixture, GetEmptyTasks)
  {
    HttpClient client("127.0.0.1", 9000);
    http::response< http::string_body > response;
    ASSERT_NO_THROW(response = client.request(http::verb::get, "/tasks"));

    ASSERT_EQ(response.result(), http::status::ok);
    ASSERT_EQ(response[http::field::content_type], "application/json");

    auto json = nlohmann::json::parse(response.body());
    EXPECT_TRUE(json.is_array());
    EXPECT_TRUE(json.empty());
  }

  TEST_F(TestServerFixture, CreateAndGetTask)
  {
    HttpClient client("127.0.0.1", 9000);

    nlohmann::json create_json = {
      { "title", "Title" },
      { "description", "Description" },
      { "status", "Todo" }
    };

    http::response< http::string_body > create_response;
    ASSERT_NO_THROW(create_response = client.request(http::verb::post, "/task", create_json));

    ASSERT_EQ(create_response.result(), http::status::ok);
    ASSERT_EQ(create_response[http::field::content_type], "application/json");

    auto create_json_response = nlohmann::json::parse(create_response.body());
    ASSERT_TRUE(create_json_response.contains("message") && !create_json_response["message"].is_null());

    int task_id = create_json_response["message"].get< int >();

    http::response< http::string_body > get_response;
    ASSERT_NO_THROW(get_response = client.request(http::verb::get, "/task" + std::to_string(task_id)));

    auto get_json_response = nlohmann::json::parse(get_response.body());
    ASSERT_TRUE(create_json_response.contains("id") && !create_json_response["id"].is_null());

    EXPECT_EQ(create_json_response["message"].get< int >(), task_id);
    EXPECT_EQ(create_json_response["message"].get< std::string >(), "Title");
  }
}
