#pragma once
#include <string_view>
#include <variant>
#include <vector>
#include <string>
#include <stdexcept>

using cell = std::variant<std::monostate ,int, double, std::string>;
struct query_result{
  std::vector<std::string> columns;
  std::vector<std::vector<cell>> tuples;
};

// Database 
struct sqlite3;
class Database{
  private:
    sqlite3* db = nullptr;
    void open(std::string_view);
    template<typename T>
    inline bool is_type(const cell& c) const noexcept{
      return std::holds_alternative<T>(c);
    }
  
  public:
    Database() = delete;
    explicit Database(std::string_view);
    ~Database() noexcept;

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    
    bool is_open() const;
    query_result execute(std::string_view, const std::vector<cell>& = {});
    int close();
};

class database_error : public std::runtime_error{
  private:
    int error_code;
  
  public:
    database_error(const std::string& message, int code)
      : std::runtime_error(message), error_code(code) {}
    
    int code() const noexcept{
      return error_code;
    }
};