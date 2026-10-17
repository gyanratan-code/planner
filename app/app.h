#pragma once
#include "../db/db.h"
#include "../task/task.h"
#include "../task/task_manager.h"
#include "../cli/parser.h"
#include "../cli/output.h"
#include "../cli/input.h"

class App{
  private:
    Database db;
    task_manager manager;
    task_group cur_group;
    bool is_active = true;

    void process_command(const parsed_command& command);
  
  public:
    explicit App(std::string_view db_name);
    void run();
};