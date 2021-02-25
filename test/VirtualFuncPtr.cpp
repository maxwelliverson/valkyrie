//
// Created by Maxwell on 2020-12-17.
//

#include <iostream>
#include <span>


class Base{
public:
  virtual ~Base() = default;
  virtual int f() { return 0; }
  virtual int g() { return -1; }
};
class DerivedA : public Base{
public:
  int f() override { return 1; }
  int g() override { return -2; }
};
class DerivedB : public virtual Base{
public:
  int f() override { return 2; }
  int g() override { return -3; }
};
class DerivedC : public virtual Base{
public:
  int f() override { return 3; }
  int g() override { return -4; }
};
class DerivedD : public DerivedB, public DerivedC{
public:
  int f() override { return 4; }
  int g() override { return -5; }
};


template <typename PFN>
void printFunctionPtr(PFN pfn) noexcept {
  std::cout << std::hex;
  auto asBytes = std::as_bytes(std::span(&pfn, 1));
  for (auto byte : asBytes)
    std::cout << "0x" << (unsigned)byte << " ";
}




int main(){
  
  Base base;
  DerivedA a;
  DerivedB b;
  DerivedC c;
  DerivedD d;
  
  using PFN_base = int(Base::*)();
  using PFN_a = int(DerivedA::*)();
  using PFN_b = int(DerivedB::*)();
  using PFN_c = int(DerivedC::*)();
  using PFN_d = int(DerivedD::*)();

  PFN_base pfnBase = &Base::f;
  PFN_a    pfnA    = &DerivedA::f;
  PFN_b    pfnB    = &DerivedB::f;
  PFN_c    pfnC    = &DerivedC::f;
  PFN_d    pfnD    = &DerivedD::f;
  //PFN_base pfnDerivedA = &DerivedA::f;

#define print_value(val) std::cout << #val ": "; printFunctionPtr(val); std::cout << "\n\n"


  print_value(pfnBase);
  print_value(pfnA);
  print_value(pfnB);
  print_value(pfnC);
  print_value(pfnD);


  std::cout << "\n" << std::endl;



  pfnBase = &Base::g;
  pfnA    = &DerivedA::g;
  pfnB    = &DerivedB::g;
  pfnC    = &DerivedC::g;
  pfnD    = &DerivedD::g;


  print_value(pfnBase);
  print_value(pfnA);
  print_value(pfnB);
  print_value(pfnC);
  print_value(pfnD);


  std::cout << std::endl;



  /*std::cout << "Base: " << (base.*pfnBase)() << std::endl;
  std::cout << "A: "    << (a.*pfnBase)() << std::endl;
  std::cout << "B: "    << (b.*pfnBase)() << std::endl;
  std::cout << "C: "    << (c.*pfnBase)() << std::endl;
  std::cout << "D: "    << (d.*pfnBase)() << std::endl;


  std::cout << "\n\nAptr: " << (a.*pfnA)() << std::endl;
  std::cout << "Bptr: " << (b.*pfnB)() << std::endl;
  std::cout << "Cptr: " << (c.*pfnC)() << std::endl;
  std::cout << "Dptr: " << (d.*pfnD)() << std::endl;


  std::cout << "\n\nD->B: " << (d.*pfnB)() << std::endl;
  std::cout << "D->C: " << (d.*pfnC)() << std::endl;*/
}