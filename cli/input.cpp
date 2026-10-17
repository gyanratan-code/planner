#include "input.h"
#include <iostream>
#include <editline/readline.h>
#include <cstdlib>

std::string read_line(bool& eof, std::string& prompt){
  std::string full_prompt = prompt + "> ";
  char* line = readline(full_prompt.c_str());
  std::string input = "";
  if(line == nullptr){
    eof = true;
  }else{
    eof = false;
    input = std::string(line);
    free(line);
  }
  return input;
}