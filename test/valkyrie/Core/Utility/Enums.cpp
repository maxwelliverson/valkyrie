//
// Created by Maxwell on 2020-11-17.
//

#include <valkyrie/Core/Utility/Enums.hpp>

#include <iostream>

using namespace valkyrie::Core;

int main(){
  std::cout << Custom::Enum<NonScopedEnum>::name << std::endl;
  std::cout << Custom::Enum<ScopedEnum>::name << std::endl;
  std::cout << Custom::Enum<NonScopedEnum>::scoped_name << std::endl;
  std::cout << Custom::Enum<ScopedEnum>::scoped_name << std::endl;
}