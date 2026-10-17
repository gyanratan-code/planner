#pragma once
#include <string>
#include <vector>

// Data structure of parser
struct parsed_command{
  std::string command;
  std::vector<std::string> args;
};

// Command mapping to enum
enum class CMD{
	create,
	exit,
	show,
  open,
	remove,
	home,
	help,
  update,
	unknown
};

CMD resolve_command(std::string_view);

parsed_command parse(std::string_view);

void validate_args(CMD command, const std::vector<std::string>&);
