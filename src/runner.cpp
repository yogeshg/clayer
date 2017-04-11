#include "logconfig.h"
#include "logger.h"

void bar(){
  LOG(ERROR) << "this should be logged in bar()";
}

void foo(){
  LOG(DEBUG) << "this should be logged in foo()";
  bar();
}

int main() {
  LOG(INFO) << "this should be logged in the main function";
  foo(); 
  return 0;
}

