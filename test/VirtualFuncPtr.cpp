//
// Created by Maxwell on 2020-12-17.
//

#include <iostream>


class Base{
public:
  virtual int f() { return 0; }
};
class DerivedA : public Base{
public:
  int f() override { return 1; }
};
class DerivedB : public virtual Base{
public:
  int f() override { return 2; }
};
class DerivedC : public virtual Base{
public:
  int f() override { return 3; }
};
class DerivedD : public DerivedB, public DerivedC{
public:
  int f() override { return 4; }
};





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

  std::cout << "Base: " << (base.*pfnBase)() << std::endl;
  std::cout << "A: "    << (a.*pfnBase)() << std::endl;
  std::cout << "B: "    << (b.*pfnBase)() << std::endl;
  std::cout << "C: "    << (c.*pfnBase)() << std::endl;
  std::cout << "D: "    << (d.*pfnBase)() << std::endl;


  std::cout << "\n\nAptr: " << (a.*pfnA)() << std::endl;
  std::cout << "Bptr: " << (b.*pfnB)() << std::endl;
  std::cout << "Cptr: " << (c.*pfnC)() << std::endl;
  std::cout << "Dptr: " << (d.*pfnD)() << std::endl;


  std::cout << "\n\nD->B: " << (d.*pfnB)() << std::endl;
  std::cout << "D->C: " << (d.*pfnC)() << std::endl;
}