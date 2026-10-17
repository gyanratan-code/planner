#pragma once

#include <string>

struct task{
  int id;
  int group_id;
  std::string name;
  std::string description;
  bool completed;
};

struct task_group{
  int id;
  std::string name;
};