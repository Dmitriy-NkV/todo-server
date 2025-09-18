#include "test_utils.hpp"

namespace tests
{
  TEST_F(TestServerFixture, GetEmptyTasks)
  {
    HttpClient client(server_host_, server_port_);
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
    HttpClient client(server_host_, server_port_);

    nlohmann::json create_json = {
      { "title", "Title" },
      { "description", "Description" },
      { "status", "Todo" }
    };

    http::response< http::string_body > create_response;
    ASSERT_NO_THROW(create_response = client.request(http::verb::post, "/task", create_json));

    ASSERT_EQ(create_response.result(), http::status::created);
    ASSERT_EQ(create_response[http::field::content_type], "application/json");

    auto create_json_response = nlohmann::json::parse(create_response.body());
    ASSERT_TRUE(create_json_response.contains("message") && !create_json_response["message"].is_null());

    int task_id = create_json_response["message"].get< int >();

    http::response< http::string_body > get_response;
    ASSERT_NO_THROW(get_response = client.request(http::verb::get, "/task" + std::to_string(task_id)));

    ASSERT_EQ(create_response.result(), http::status::ok);
    ASSERT_EQ(create_response[http::field::content_type], "application/json");

    auto get_json_response = nlohmann::json::parse(get_response.body());

    EXPECT_EQ(get_json_response["id"].get< int >(), task_id);
    EXPECT_EQ(get_json_response["title"].get< std::string >(), "Title");
  }

  TEST_F(TestServerFixture, UpdateAndGetTask)
  {
    HttpClient client(server_host_, server_port_);

    nlohmann::json create_json = {
      { "title", "Title" },
      { "description", "Description" },
      { "status", "Todo" }
    };

    http::response< http::string_body > create_response;
    ASSERT_NO_THROW(create_response = client.request(http::verb::post, "/task", create_json));

    ASSERT_EQ(create_response.result(), http::status::created);
    ASSERT_EQ(create_response[http::field::content_type], "application/json");

    auto create_json_response = nlohmann::json::parse(create_response.body());
    ASSERT_TRUE(create_json_response.contains("message") && !create_json_response["message"].is_null());

    int task_id = create_json_response["message"].get< int >();

    nlohmann::json update_json = {
      { "id", task_id },
      { "title", "New title" },
      { "description", "New description" },
      { "status", "In progress" }
    };

    http::response< http::string_body > update_response;
    ASSERT_NO_THROW(update_response = client.request(http::verb::put, "/task", create_json));

    ASSERT_EQ(update_response.result(), http::status::accepted);
    ASSERT_EQ(update_response[http::field::content_type], "application/json");

    http::response< http::string_body > get_response;
    ASSERT_NO_THROW(get_response = client.request(http::verb::get, "/task" + std::to_string(task_id)));

    ASSERT_EQ(get_response.result(), http::status::created);
    ASSERT_EQ(get_response[http::field::content_type], "application/json");

    auto get_json_response = nlohmann::json::parse(get_response.body());
    EXPECT_EQ(create_json_response["id"].get< int >(), task_id);
    EXPECT_EQ(create_json_response["title"].get< std::string >(), "New title");
  }

  TEST_F(TestServerFixture, DeleteTask)
  {
    HttpClient client(server_host_, server_port_);

    nlohmann::json create_json = {
      { "title", "Title" },
      { "description", "Description" },
      { "status", "Todo" }
    };

    http::response< http::string_body > create_response;
    ASSERT_NO_THROW(create_response = client.request(http::verb::post, "/task", create_json));

    ASSERT_EQ(create_response.result(), http::status::created);
    ASSERT_EQ(create_response[http::field::content_type], "application/json");

    auto create_json_response = nlohmann::json::parse(create_response.body());
    ASSERT_TRUE(create_json_response.contains("message") && !create_json_response["message"].is_null());

    int task_id = create_json_response["message"].get< int >();

    http::response< http::string_body > delete_response;
    ASSERT_NO_THROW(delete_response = client.request(http::verb::delete_, "/task" + std::to_string(task_id)));

    ASSERT_EQ(delete_response.result(), http::status::accepted);
    ASSERT_EQ(delete_response[http::field::content_type], "application/json");

    http::response< http::string_body > get_response;
    ASSERT_NO_THROW(get_response = client.request(http::verb::get, "/task" + std::to_string(task_id)));

    ASSERT_EQ(get_response.result(), http::status::ok);
    ASSERT_EQ(get_response[http::field::content_type], "application/json");

    auto get_json_response = nlohmann::json::parse(get_response.body());
    EXPECT_EQ(get_json_response["message"].get< std::string >(), "No task with current id");
  }

  TEST_F(TestServerFixture, InvalidJson)
  {
    HttpClient client(server_host_, server_port_);

    http::response< http::string_body > response;
    ASSERT_NO_THROW(response = client.request(http::verb::post, "/task", "invalid_json"));

    EXPECT_EQ(response.result(), http::status::bad_request);
  }

  TEST_F(TestServerFixture, NotFound)
  {
    HttpClient client(server_host_, server_port_);

    http::response< http::string_body > response;
    ASSERT_NO_THROW(response = client.request(http::verb::get, "/not found"));

    EXPECT_EQ(response.result(), http::status::not_found);
  }
}
