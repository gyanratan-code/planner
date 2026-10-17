#include "parser.h"
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <system_error>
#include <charconv>

namespace{
  const std::unordered_map<std::string,CMD> command_map = {
    {"create", CMD::create},
    {"exit", CMD::exit},
    {"remove", CMD::remove},
    {"home", CMD::home},
    {"help", CMD::help},
    {"open", CMD::open},
    {"show", CMD::show},
    {"update", CMD::update}
  };
  bool valid_id(std::string_view arg){
    int id = -1;
    const char* end = arg.data() + arg.size();
    const int max_id = (1<<30) - 1;
    auto [parse_end, error_code] = std::from_chars(arg.data(), end, id);
    return (error_code == std::errc{} && parse_end == end && id > 0 && id<=max_id);
  }
}

CMD resolve_command(std::string_view command){
  std::string cmd = std::string(command);
	auto it = command_map.find(cmd);
	if(it == command_map.end()){
		return CMD::unknown;
	}
	return it->second;
}

parsed_command parse(std::string_view input){
  parsed_command output;
  std::string current = "";
  bool quoted_input = false;
  for(auto& ch : input){
    // handle quoting character
    if(ch == '"'){
      if(quoted_input){
        quoted_input = false;
        if(!current.empty()){
          output.args.push_back(current);
          current = "";
        }
      }else{
        quoted_input = true;
      }
      continue;
    }
    auto u_ch = static_cast<unsigned char>(ch);
    if(isspace(u_ch)){
      if(!current.empty() && !quoted_input){
        output.args.push_back(current);
        current = "";
      }
      if(quoted_input && !current.empty() && current.back() != ' '){
        current.push_back(' ');
      }
      continue;
    }
    current += ch;
  }
  if(quoted_input){
    throw std::runtime_error("Quoting mismatch.");
  }
  if(!current.empty()){
    output.args.push_back(current);
    current = "";
  }
  if(output.args.size() >=1 ){
    output.command = output.args.front();
  }
  return output;
}

void validate_args(CMD command, const std::vector<std::string>& args){
  switch (command){
    case CMD::create:{

      break;
    }
    case CMD::exit:{
      if(args.size() != 1){
        throw std::runtime_error("No argument required.");
      }
      break;
    }
    case CMD::remove:{
      if(args.size() != 2){
        throw std::runtime_error("Expected: remove <id>.");
      }
      if(!valid_id(args[1])){
        throw std::runtime_error("Invalid ID: " + args[1]);
      }
      break;
    }
    case CMD::home:{
      if(args.size() != 1){
        throw std::runtime_error("No argument required.");
      }
      break;
    }
    case CMD::help:{
      if(args.size() != 1){
        throw std::runtime_error("No argument required.");
      }
      break;
    }
    case CMD::open:{
      if(args.size() != 2){
        throw std::runtime_error("Expected: open <id>.");
      }
      if(!valid_id(args[1])){
        throw std::runtime_error("Invalid ID: " + args[1]);
      }
      break;
    }
    case CMD::show:{
      if(args.size() != 1){
        throw std::runtime_error("No argument required.");
      }
      break;
    }
    case CMD::update:{
      if(args.size() != 4){
        throw std::runtime_error("Expected: update <name|description|status> <task_id> <value>");
      }
      std::string operation = args[1];
      if(!(operation == "status" || operation == "name" || operation == "description")){
        throw std::runtime_error("Expected: one of the <name|description|status>");
      }
      std::string task_id = args[2];
      if(!valid_id(task_id)){
        throw std::runtime_error("Invalid ID: " + task_id);
      }
      std::string updated_value = args[3];
      if(args[1] == "status" && !(updated_value == "true" || updated_value == "false")){
        throw std::runtime_error("Invalid updated state: " + args[3] + ".");
      }
      break;
    }
    case CMD::unknown:
    default:{
      throw std::runtime_error("Invalid command: " + args[0]);
    }
  }
}