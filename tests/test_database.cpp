#include "test_utils.hpp"

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

TEST_F(TestDatabaseFixture, GetNonExistentTask)
{
  auto result = db_->get_task_by_id(1000000);
  EXPECT_FALSE(result);
}


TEST_F(TestDatabaseFixture, DeleteTask)
{
  database::Task task;
  task.set_title("Title");
  task.set_description("Descrtiption");
  task.set_status("In progress");
  int task_id = db_->create_task(task);

  ASSERT_NO_THROW(db_->delete_task(task_id));

  auto result = db_->get_task_by_id(task_id);
  EXPECT_FALSE(result);
}

TEST_F(TestDatabaseFixture, DeleteNonExistentTask)
{
  ASSERT_THROW(db_->delete_task(1000000), std::runtime_error);
}
