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

TEST_F(TestDatabaseFixture, GetAllTasks)
{
  database::Task task1;
  task1.set_title("Title 1");
  task1.set_status("In progress");
  db_->create_task(task1);

  database::Task task2;
  task1.set_title("Title 2");
  task1.set_status("Todo");
  db_->create_task(task2);

  auto tasks = db_->get_all_tasks();
  ASSERT_EQ(tasks.size(), 2);

  EXPECT_EQ(tasks[0].get_title().value(), "Title 2");
  EXPECT_EQ(tasks[1].get_title().value(), "Title 1");
}

TEST_F(TestDatabaseFixture, UpdateTask)
{
  database::Task task;
  task.set_title("Title");
  task.set_description("Descrtiption");
  task.set_status("In progress");
  int id = db_->create_task(task);

  database::Task updated_task;
  updated_task.set_id(id);
  updated_task.set_title("New Title");
  updated_task.set_description("New Descrtiption");
  updated_task.set_status("Completed");
  db_->update_task(updated_task);

  auto result_task = db_->get_task_by_id(id);
  ASSERT_TRUE(result_task);
  EXPECT_EQ(result_task->get_id(), id);
  EXPECT_EQ(result_task->get_title(), "New Title");
  EXPECT_EQ(result_task->get_description(), "New Description");
  EXPECT_EQ(result_task->get_status(), "Completed");
}

TEST_F(TestDatabaseFixture, UpdateEmptyTask)
{
  database::Task task;
  task.set_title("Title");
  task.set_description("Descrtiption");
  task.set_status("In progress");
  int id = db_->create_task(task);

  database::Task updated_task;
  updated_task.set_id(id);
  ASSERT_THROW(db_->update_task(updated_task), std::invalid_argument);
}


TEST_F(TestDatabaseFixture, UpdateNonExistentTask)
{
  database::Task task;
  task.set_id(1000000);
  task.set_title("Title");
  task.set_description("Descrtiption");
  task.set_status("In progress");

  ASSERT_THROW(db_->update_task(task), std::runtime_error);
}
