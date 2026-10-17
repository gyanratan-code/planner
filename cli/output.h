#pragma once
#include <vector>
#include "../task/task_manager.h"

void print_task_group(const std::vector<task_group>& );

void print_task(const std::vector<task>& );

void print_error(std::string);

void print(std::string);
