
#include "task_manager.h"

namespace sql_query {
  constexpr std::string_view create_task_groups = R"(
    CREATE TABLE IF NOT EXISTS task_groups (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      name TEXT NOT NULL UNIQUE
    );
  )";

  constexpr std::string_view create_tasks = R"(
    CREATE TABLE IF NOT EXISTS tasks (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      group_id INTEGER NOT NULL,
      task TEXT NOT NULL,
      description TEXT NOT NULL,
      status INTEGER NOT NULL DEFAULT 0
        CHECK (status IN (0, 1)),
      FOREIGN KEY (group_id)
        REFERENCES task_groups(id)
        ON DELETE CASCADE
    );
  )";

  constexpr std::string_view create_group = R"(
    INSERT INTO task_groups (name)
    VALUES (?1)
    RETURNING id;
  )";

  constexpr std::string_view create_task = R"(
    INSERT INTO tasks (group_id, task, description, status)
    VALUES (?1, ?2, ?3, 0)
    RETURNING id;
  )";

  constexpr std::string_view delete_group = R"(
    DELETE FROM task_groups
    WHERE id = ?1
    RETURNING id;
  )";

  constexpr std::string_view delete_task = R"(
    DELETE FROM tasks
    WHERE id = ?1
    RETURNING id;
  )";

  constexpr std::string_view update_task_status = R"(
    UPDATE tasks
    SET status = ?2
    WHERE id = ?1
    RETURNING id;
  )";

  constexpr std::string_view update_task_name = R"(
    UPDATE tasks
    SET task = ?2
    WHERE id = ?1
    RETURNING id;
  )";

  constexpr std::string_view update_task_description = R"(
    UPDATE tasks
    SET description = ?2
    WHERE id = ?1
    RETURNING id;
  )";

  constexpr std::string_view list_tasks = R"(
    SELECT id, group_id, task, description, status
    FROM tasks
    WHERE group_id = ?1
    ORDER BY id;
  )";

  constexpr std::string_view list_groups = R"(
    SELECT id, name
    FROM task_groups
    ORDER BY name;
  )";

  constexpr std::string_view get_group_name = R"(
    SELECT name
    FROM task_groups
    WHERE id = ?1;
  )";
}

task_manager::task_manager(Database& database) : db(database){
}

void task_manager::setup(){
  db.execute(sql_query::create_tasks);
  db.execute(sql_query::create_task_groups);
}

int task_manager::create_group(std::string_view name){
  query_result response = db.execute(sql_query::create_group, {std::string(name)});
  return (response.tuples.empty() ? -1 : std::get<int>(response.tuples[0][0]));
}

int task_manager::create_task(int group_id, std::string_view name, std::string_view description){
  query_result response = db.execute(sql_query::create_task, {group_id, std::string(name), std::string(description)});
  return (response.tuples.empty() ? -1 : std::get<int>(response.tuples[0][0]));
}

bool task_manager::delete_group(int group_id){
  query_result response = db.execute(sql_query::delete_group, {group_id});
  return !response.tuples.empty();
}

bool task_manager::delete_task(int task_id){
  query_result response = db.execute(sql_query::delete_task, {task_id});
  return !response.tuples.empty();
}

bool task_manager::update_task_status(int task_id, bool completed){
  query_result response = db.execute(sql_query::update_task_status, {task_id, completed ? 1 : 0});
  return !response.tuples.empty();
}

bool task_manager::update_task_name(int task_id, std::string_view name){
  query_result response = db.execute(sql_query::update_task_name, {task_id});
  return !response.tuples.empty();
}

bool task_manager::update_task_description(int task_id, std::string_view description){
  query_result response = db.execute(sql_query::update_task_description, {task_id, std::string(description)});
  return !response.tuples.empty();
}

std::string task_manager::group_name(int group_id){
  query_result response = db.execute(sql_query::get_group_name, {group_id});
  // Handle case of zero rows
  if(response.tuples.empty()){
    throw std::runtime_error("Invalid group id: " + std::to_string(group_id));
  }
  return std::get<std::string>(response.tuples[0][0]);
}

std::vector<task> task_manager::list_tasks(int group_id){
  std::vector<task> task_list;
  query_result response = db.execute(sql_query::list_tasks, {group_id});
  // store the things in task format
  const auto& rows = response.tuples;
  for(size_t row_ind = 0; row_ind < rows.size(); ++row_ind){
    task row;
    row.id = std::get<int>(rows[row_ind][0]);
    row.group_id = std::get<int>(rows[row_ind][1]);
    row.name = std::get<std::string>(rows[row_ind][2]);
    row.description = std::get<std::string>(rows[row_ind][3]);
    row.completed = std::get<int>(rows[row_ind][4]);
    task_list.push_back(row);
  }
  return task_list;
}

std::vector<task_group> task_manager::list_groups(){
  std::vector<task_group> task_group_list;
  query_result response = db.execute(sql_query::list_groups, {});
  // store the things in task_group format
  const auto& rows = response.tuples;
  for(size_t row_ind = 0; row_ind < rows.size(); ++row_ind){
    task_group row;
    row.id = std::get<int>(rows[row_ind][0]);
    row.name = std::get<std::string>(rows[row_ind][1]);
    task_group_list.push_back(row);
  }
  return task_group_list;
}