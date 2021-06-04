//
// Created by maxwe on 2021-04-15.
//

#include <iostream>
#include <sstream>
#include <bitset>
#include <string_view>
#include <regex>
#include <ranges>

#include <immintrin.h>


#include <valkyrie/preprocessor.hpp>


void printMode(uint32_t mode, std::stringstream& ss) {
  ss << R"str( "mode": {)str" << "\n\t\t\t\t" << R"str("type": ")str";
  switch (mode & 0x3) {
    case 0:
      ss << "u8";
      break;
    case 1:
      ss << "u16";
      break;
    case 2:
      ss << "i8";
      break;
    case 3:
      ss << "i16";
      break;
    default:
      __assume(false);
  }
  ss << "\",\n\t\t\t\t" << R"("compare": ")";
  switch (mode & 0xC) {
    case 0:
      ss << "equal any";
      break;
    case 4:
      ss << "ranges";
      break;
    case 8:
      ss << "equal each";
      break;
    case 12:
      ss << "equal ordered";
      break;
    default:
      __assume(false);
  }
  ss << "\",\n\t\t\t\t" << R"("polarity": ")";
  switch (mode & 0x30) {
    case 0:
      ss << "positive";
      break;
    case 16:
      ss << "negative";
      break;
    case 32:
      ss << "masked positive";
      break;
    case 48:
      ss << "masked negative";
      break;
    default:
      __assume(false);
  }
  ss << "\",\n\t\t\t\t" << R"("index": ")";
  if (mode & 0x40)
    ss << "most significant";
  else
    ss << "least significant";
  ss << "\",\n\t\t\t\t" << R"("mask": ")";
  if (mode & 0x40)
    ss << "unit";
  else
    ss << "bit";
  ss << "\"\n\t\t\t}";
}


union aligned_t{
  uint8_t buffer[16];
  __m128i vector;
};


std::ostream& operator<<(std::ostream& ss, const __m128i& bigNum) {
  ss << "0b" << *(const std::bitset<128>*)((const void*)&bigNum);
  return ss;
/*#define SWITCH_CASE_(val) case 0x##val : ss << #val; break;
#define SWITCH_CASE SWITCH_CASE_

  for (auto byte : aligned.buffer | std::views::reverse) {
    for (int i = 0; i < 8; i += 4) {
      switch ((byte >> i) & 0xF) {
        VK_foreach(SWITCH_CASE, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F)
        default:
        assert(false);
        __assume(false);
      }
    }
  }
  return ss;*/
}




template <int32_t Mode>
void compare_explicit_index(std::string_view strA, std::string_view strB, std::stringstream& ss) {
  __m128i aBuffer = {};
  __m128i bBuffer = {};

  const int aLength(std::min(strA.length(), 16ull));
  const int bLength(std::min(strB.length(), 16ull));

  std::memcpy(&aBuffer, strA.data(), aLength);
  std::memcpy(&bBuffer, strB.data(), bLength);


  const int result = _mm_cmpestri(aBuffer, aLength, bBuffer, bLength, Mode);

  ss << R"({ "op": "cmpestri", "result": )" << result /*<< R"(, "bits": ")" << *(std::bitset<32>*)&result;
  ss*/ << " }";
}
template <int32_t Mode>
void compare_implicit_index(std::string_view strA, std::string_view strB, std::stringstream& ss) {
  __m128i aBuffer = {};
  __m128i bBuffer = {};

  std::memcpy(&aBuffer, strA.data(), std::min(strA.length(), 16ull));
  std::memcpy(&bBuffer, strB.data(), std::min(strB.length(), 16ull));


  const int result = _mm_cmpistri(aBuffer, bBuffer, Mode);

  ss << R"({ "op": "cmpistri", "result": )" << result /*<< R"(, "bits": ")" << *(std::bitset<32>*)&result*/ << " }";
}
template <int32_t Mode>
void compare_explicit_mask(std::string_view strA, std::string_view strB, std::stringstream& ss) {
  __m128i aBuffer = {};
  __m128i bBuffer = {};

  const int aLength(std::min(strA.length(), 16ull));
  const int bLength(std::min(strB.length(), 16ull));

  std::memcpy(&aBuffer, strA.data(), aLength);
  std::memcpy(&bBuffer, strB.data(), bLength);


  const __m128i result = _mm_cmpestrm(aBuffer, aLength, bBuffer, bLength, Mode);

  ss << R"({ "op": "cmpestrm", "result": )" << result << " }";
}
template <int32_t Mode>
void compare_implicit_mask(std::string_view strA, std::string_view strB, std::stringstream& ss) {
  __m128i aBuffer = {};
  __m128i bBuffer = {};

  std::memcpy(&aBuffer, strA.data(), std::min(strA.length(), 16ull));
  std::memcpy(&bBuffer, strB.data(), std::min(strB.length(), 16ull));


  const __m128i result = _mm_cmpistrm(aBuffer, bBuffer, Mode);

  ss << R"({ "op": "cmpistrm", "result": )" << result << " }";
}


template <int32_t Mode>
void compare_each(std::string_view a, std::string_view b, std::stringstream& ss) {
  ss << "\t\t{\n\t\t\t";
  printMode(Mode, ss);
  ss << ",\n\t\t\t\"instructions\": [\n\t\t\t\t";
  compare_explicit_index<Mode>(a, b, ss);
  ss << ",\n\t\t\t\t";
  compare_implicit_index<Mode>(a, b, ss);
  ss << ",\n\t\t\t\t";
  compare_explicit_mask<Mode>(a, b, ss);
  ss << ",\n\t\t\t\t";
  compare_implicit_mask<Mode>(a, b, ss);
  ss << "\n\t\t\t]\n\t\t},\n";
}


VK_constant int mode_a = _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_MASKED_POSITIVE_POLARITY | _SIDD_BIT_MASK;
VK_constant int mode_b = _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_MASKED_POSITIVE_POLARITY | _SIDD_BIT_MASK;
VK_constant int mode_c = _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_MASKED_POSITIVE_POLARITY | _SIDD_BIT_MASK;
VK_constant int mode_d = _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ORDERED | _SIDD_MASKED_POSITIVE_POLARITY | _SIDD_BIT_MASK;
VK_constant int mode_e = _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY | _SIDD_MASKED_POSITIVE_POLARITY | _SIDD_UNIT_MASK;
VK_constant int mode_f = _SIDD_UBYTE_OPS | _SIDD_CMP_RANGES | _SIDD_MASKED_POSITIVE_POLARITY | _SIDD_UNIT_MASK;
VK_constant int mode_g = _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_MASKED_POSITIVE_POLARITY | _SIDD_UNIT_MASK;
VK_constant int mode_h = _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ORDERED | _SIDD_MASKED_POSITIVE_POLARITY | _SIDD_UNIT_MASK;

int main(){

  std::regex inputRegex;

  try {
    inputRegex = std::regex(R"reg(\s*\{\s*([^,]+),\s*([^}]+)\s*\})reg");
  } catch (std::regex_error& err) {
    std::cerr << err.what() << std::endl;
    return 1;
  }
  static constexpr std::string_view exit = "exit";
  static constexpr std::string_view clear = "clear";

  std::string totalInput;
  std::string inputString;


  while (true) {

    inputString.clear();
    std::smatch matches;

    std::string stringA;
    std::string stringB;
    std::string_view stringA_view;
    std::string_view stringB_view;

    std::cin >> inputString;

    if (inputString == exit) {
      break;
    } else if (inputString == clear) {
      totalInput.clear();
      continue;
    }

    totalInput += inputString;

    if (!std::regex_match(totalInput, matches, inputRegex)) {
      continue;
    }

    stringA = matches[1];
    stringB = matches[2];

    stringA_view = stringA;
    stringA_view = stringA_view.substr(0, 16);
    stringB_view = stringB;
    stringB_view = stringB_view.substr(0, 16);


    std::stringstream resultString;
    resultString <<
        "{\n\t\"stringA\": \"" << stringA_view <<
        "\",\n\t\"stringB\": \"" << stringB_view <<
        "\",\n\t\"results\": [\n";
#define compare_fn(i) compare_each<mode_##i>(stringA_view, stringB_view, resultString);
    VK_foreach(compare_fn, a, b, c, d, e, f, g, h)
#undef compare_fn
    resultString << "\b\b\n\t]\n}\n\n\n";
    std::cout << resultString.str() << std::endl;

    totalInput.clear();
  }




  return 0;
}

