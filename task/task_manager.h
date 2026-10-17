#pragma once

#include <vector>
#include <string>
#include <string_view>

#include "task.h"
#include "../db/db.h"

class task_manager{
  private:
    Database& db;

  public:
    explicit task_manager(Database&);
    // setup
    void setup();
    // Create
    int create_group(std::string_view);
    int create_task(int, std::string_view, std::string_view);
    // Updation and deletion
    bool delete_group(int);
    bool delete_task(int);
    bool update_task_status(int ,bool);
    bool update_task_name(int, std::string_view);
    bool update_task_description(int , std::string_view);
    // Read
    std::string group_name(int);
    std::vector<task> list_tasks(int);
    std::vector<task_group> list_groups();
};
