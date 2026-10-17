#include "app.h"
#include <iostream>

App::App(std::string_view db_name) : db(db_name), manager(db){
}

namespace{
  constexpr char home_help[] = R"(
  Available commands:
  show                List task groups
  create <group_name> Create a task group
  open <group_id>     Open a task group
  remove <group_id>   Remove a task group
  help                Show available commands
  exit                Exit the planner
  )";

 constexpr char group_help[] = R"(
  Available commands:
  show                        List tasks
  create <name> <description> Create a task
  update <field> <id> <value> Update a task
  remove <task_id>            Remove a task
  home                        Return to task groups
  clear                       Clear the terminal
  help                        Show available commands
  exit                        Exit the planner
  )";
}

void App::run(){
  cur_group.id = -1;
  cur_group.name = "";
  manager.setup();
  do{
    try{
      // print the current directory
      // std::cout<<cur_group.name<<">";
      bool is_eof = false;
      std::string input = read_line(is_eof, cur_group.name);
      if(input.empty()){
        if(is_eof){
          is_active = false;
          break;
        }
        continue;
      }
      parsed_command parsed_input = parse(input);
      validate_args(resolve_command(parsed_input.command), parsed_input.args);
      process_command(parsed_input);
    } catch(const std::exception& e){
      std::cout<<e.what()<<"\n";
    }
  } while(is_active);
}

void App::process_command(const parsed_command& cmd){
  auto& args = cmd.args;
  switch (resolve_command(cmd.command)){
    case CMD::create:{
      int id = -1;
      if(cur_group.name.empty()){
        if(args.size() != 2){
          throw std::runtime_error("Expected create <group_name>.");
        }
        id = manager.create_group(args[1]);
      }else{
        if(args.size() != 3){
          throw std::runtime_error("Expected create <name> <description>.");
        }
        id = manager.create_task(cur_group.id ,args[1], args[2]);
      }
      print(args[1] + " is created with id: " + std::to_string(id));
      break;
    }
    case CMD::exit:{
      std::cout<<"Happy planning!\n";
      is_active = false;
      break;
    }
    case CMD::remove:{
      int id = stoi(args[1]);
      bool success = false;
      if(cur_group.name.empty()){
        success = manager.delete_group(id);
      }else{
        success = manager.delete_task(id);
      }
      if(!success){
        print_error("Removal failed.");
      }
      break;
    }
    case CMD::home:{
      cur_group.id = -1;
      cur_group.name = "";
      break;
    }
    case CMD::help:{
      print(cur_group.name.empty() ? home_help : group_help);
      break;
    }
    case CMD::open:{
      int id = stoi(args[1]);
      std::string group_name = manager.group_name(id);
      cur_group.id = id;
      cur_group.name = group_name;
      break;
    }
    case CMD::show:{
      if(cur_group.name.empty()){
        std::vector<task_group> groups = manager.list_groups();
        print_task_group(groups);
      }else{
        std::vector<task> tasks = manager.list_tasks(cur_group.id);
        print_task(tasks);
      }
      break;
    }
    case CMD::update:{
      if(cur_group.name.empty()){
        throw std::runtime_error("Invalid command: update.");
      }
      int task_id = stoi(args[2]);
      std::string updated_value = args[3];
      std::string operation = args[1];
      if(operation == "status"){
        manager.update_task_status(task_id, updated_value == "true" ? 1 : 0);
      }else if(operation == "name"){
        manager.update_task_name(task_id, updated_value);
      }else if(operation == "description"){
        manager.update_task_description(task_id, updated_value);
      }
      break;
    }
    case CMD::unknown:
    default:{
      std::cout<<"Unreachable state!\n";
    }
  }
}