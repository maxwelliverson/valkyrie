//
// Created by Maxwell on 2020-11-13.
//

#include <valkyrie/ADT/arrays.hpp>

#include <iostream>

using namespace valkyrie;

int main(){
  auto* ptr = new int[5 * 4 * 5]{};

  ptr[92] = 420;
  ptr[54] = 69;

  ArrayView<int, 5, _, 5> arrayView{ptr, 4};
  Arrays::BasicArrayView<int, Arrays::Extents<5, _, 5>, Arrays::LayoutLeft> otherArrayView{ptr, 4};
  //Arrays::BasicArrayView<int, Arrays::Extents<5, _, 5>, Arrays::LayoutStrided<Arrays::Strides<1, 5, _>>> stridedView{ptr, Arrays::Extents<5, _, 5>{4}, Arrays::Strides<1, 5, _>{}};


  std::cout << "Right layout: " << arrayView(2, 2, 4) << std::endl;
  std::cout << "Left layout: " << otherArrayView(2, 2, 4) << std::endl;


  delete[] ptr;

  return 0;
}