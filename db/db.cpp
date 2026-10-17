#include "db.h"
#include <sqlite3.h>

// Implementation of db.h

Database::Database(std::string_view db_name){
  open(db_name);
}

Database::~Database(){
  close();
}

bool Database::is_open() const{
  return (db != nullptr);
}

void Database::open(std::string_view db_name_str){
  if(db != nullptr){
    return;
  }
  std::string db_name = std::string(db_name_str);
  int status = sqlite3_open(db_name.c_str(), &db);
  if(status != SQLITE_OK){
    std::string error = db ? sqlite3_errmsg(db) : sqlite3_errstr(status);
    if(db != nullptr){
      sqlite3_close_v2(db);
      db = nullptr;
    }
    std::string error_msg = "Failed to open database: " + db_name + ". " + error;
    throw database_error(error_msg, status);
  }
  // Now enable foreign key support
  try{
    execute("PRAGMA foreign_keys = ON;");
  }catch(...){
    sqlite3_close_v2(db);
    db = nullptr;
    throw;
  }
}

int Database::close(){
  if(db == nullptr){
    return SQLITE_OK;
  }
  int status = sqlite3_close(db);
  if(status == SQLITE_OK){
    db = nullptr;
  }
  return status;
}

query_result Database::execute(std::string_view query_str, const std::vector<cell>& args){
  query_result table;
  if(db == nullptr){
    throw database_error("No database selected.", SQLITE_MISUSE);
  }

  std::string query = std::string(query_str);
  std::string error_msg = "";
  sqlite3_stmt* result = nullptr; 
  const int max_byte = -1;

  // Run the sqlite_prepare_v2 function
  int status = sqlite3_prepare_v2(db, query.c_str(), max_byte, &result, nullptr);
  if(status != SQLITE_OK){
    error_msg = std::string(sqlite3_errmsg(db));
    sqlite3_finalize(result);
    throw database_error("Preparation failed: " + error_msg, status);
  }
  if(sqlite3_bind_parameter_count(result) != static_cast<int>(args.size())){
    sqlite3_finalize(result);
    throw database_error("Argument count mismatch.", SQLITE_MISUSE);
  }

  // Binding args
  for(size_t index = 0; index < args.size(); ++index){
    const cell& c = args[index];
    int binding_status = SQLITE_OK;
    if(is_type<std::string>(c)){
      binding_status = sqlite3_bind_text(result, index+1, std::get<std::string>(c).c_str(), -1, SQLITE_TRANSIENT);
    }else if(is_type<int>(c)){
      binding_status = sqlite3_bind_int(result, index+1, std::get<int>(c));
    }else if(is_type<double>(c)){
      binding_status = sqlite3_bind_double(result, index+1, std::get<double>(c));
    }else if(is_type<std::monostate>(c)){
      binding_status = sqlite3_bind_null(result, index+1);
    }
    if(binding_status != SQLITE_OK){
      error_msg = sqlite3_errmsg(db);
      sqlite3_finalize(result);
      throw database_error("Binding failed: " + error_msg, binding_status);
    }
  }

  int column_cnt = sqlite3_column_count(result);
  for(int col = 0; col < column_cnt; ++ col){
    const char* column_name = sqlite3_column_name(result, col);
    if(column_name == nullptr){
      sqlite3_finalize(result);
      throw(database_error("Failed to fetch column name.", SQLITE_NOMEM));
    }
    table.columns.push_back(std::string(column_name ? column_name : ""));
  }

  int step_status = sqlite3_step(result);
  while(step_status == SQLITE_ROW){
    // Processing Row
    std::vector<cell> row;
    for(int col = 0; col < column_cnt; ++col){
      int data_type = sqlite3_column_type(result, col);
      cell data;
      switch (data_type){
        case SQLITE_INTEGER:{
          data = sqlite3_column_int(result, col);
          break;
        }
        case SQLITE_TEXT:{
          const unsigned char* text = sqlite3_column_text(result, col);
          data = std::string(text ? reinterpret_cast<const char*>(text) : "");
          break;
        }
        case SQLITE_FLOAT:{
          data = sqlite3_column_double(result, col);
          break;
        }
        case SQLITE_NULL:{
          data = std::monostate{};
          break;
        }
        case SQLITE_BLOB:{
          // \to-do: Implement later
          break;
        }
        default:{
          error_msg = std::string(sqlite3_errmsg(db));
          sqlite3_finalize(result);
          throw database_error("Error in reading row: " + error_msg, SQLITE_MISMATCH);
        }
      }
      row.push_back(data);
    }
    table.tuples.push_back(row);
    step_status = sqlite3_step(result);
  }

  if(step_status != SQLITE_DONE){
    sqlite3_finalize(result);
    error_msg = std::string(sqlite3_errmsg(db));
    throw database_error("Partial row reading: " + error_msg, step_status);
  }

  int final_status = sqlite3_finalize(result);
  if(final_status != SQLITE_OK){
    error_msg = std::string(sqlite3_errmsg(db));
    throw database_error("Finalize failed: " + error_msg, final_status);
  }
  return table;
}

