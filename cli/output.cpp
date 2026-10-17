#include <format>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "output.h"

// Implementation of output.h

// Truncate text that exceeds the column width.
std::string fit(std::string_view text, std::size_t width){
  if(text.size() <= width){
    return std::string(text);
  }
  if(width <= 3){
    return std::string(text.substr(0, width));
  }
  return std::string(text.substr(0, width - 3)) + "...";
}

// Print a horizontal table separator.
void print_separator(std::initializer_list<std::size_t> widths){
  for(const auto width : widths){
    std::cout << '+' << std::string(width + 2, '-');
  }
  std::cout << "+\n";
}

// Print one aligned row.
void print_row(std::initializer_list<std::string> cells, std::initializer_list<std::size_t> widths){
  auto cell = cells.begin();
  auto width = widths.begin();
  std::cout << '|';
  for(; cell != cells.end() && width != widths.end(); ++cell, ++width){
    std::cout<<" "<<std::format("{:<{}}", fit(*cell, *width), static_cast<int>(*width))<< " |";
  }
  std::cout << '\n';
}

// Print all task groups.
void print_task_group(const std::vector<task_group>& groups){
  const std::initializer_list<std::size_t> widths{6, 30};
  if(groups.empty()){
    std::cout << "No task groups found.\n";
    return;
  }

  std::cout << "\nTask Groups\n";
  print_separator(widths);
  print_row({"ID", "Group Name"}, widths);
  print_separator(widths);

  for(const auto& group : groups){
    print_row({
      std::to_string(group.id),
      group.name
    }, widths);
  }
  print_separator(widths);
}

// Print all tasks.
void print_task(const std::vector<task>& tasks){
  const std::initializer_list<std::size_t> widths{6, 10, 9, 16, 30};

  if(tasks.empty()){
    std::cout << "No tasks found.\n";
    return;
  }

  std::cout << "\nTasks\n";
  print_separator(widths);
  print_row({"ID", "Status", "Group ID", "Task", "Description"}, widths);
  print_separator(widths);

  for(const auto& item : tasks){
    print_row({
      std::to_string(item.id),
      item.completed ? "Complete" : "Pending",
      std::to_string(item.group_id),
      item.name,
      item.description
    }, widths);
  }
  print_separator(widths);
}

void print_error(std::string error){
  std::cout<<"[Error]: " << error << "\n";
}

void print(std::string info){
  std::cout << info << "\n";
}