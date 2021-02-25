//
// Created by Maxwell on 2021-02-07.
//

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <span>
#include <iostream>
#include <fstream>



#define CMD_LINE_BUFFER_LEN 1 << 14


int main(int argc, char* argv[]){
  char cmdLineBuffer[CMD_LINE_BUFFER_LEN];

  std::ofstream output{R"(C:\Users\Maxwell\clang_command.txt)"};

  output << argv[0] << " ";

  for (auto&& arg : std::span{argv + 1, static_cast<size_t>(argc)}) {
    output << arg << " ";
  }

  output << std::endl;

  return -69;
}