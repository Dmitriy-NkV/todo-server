#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "database.hpp"

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

TEST_F(TestDatabaseFixture, CreateTask)
{
  database::Task task;
  task.set_title("Title");
  task.set_description("Descrtiption");
  task.set_status("In progress");

  int task_id = db_->create_task(task);
  EXPECT_GT(task_id, 0);

  auto result_task = db_->get_task_by_id(task_id);
  ASSERT_TRUE(result_task);
  EXPECT_EQ(result_task->get_id().value(), task_id);
  EXPECT_EQ(result_task->get_title().value(), "Title");
  EXPECT_EQ(result_task->get_description().value(), "Description");
  EXPECT_EQ(result_task->get_status().value(), "In progress");
}
