#include <iostream>
#include <exception>

#include "app/app.h"

int main(){
  try{
    App app("planner.db");
    app.run();
  }catch(std::exception& e){
    print_error(e.what());
    return 1;
  }
  return 0;
}