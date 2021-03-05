//
// Created by Maxwell on 2020-11-09.
//

#include <valkyrie/ADT/arrays.hpp>
#include <valkyrie/utility/uuid.hpp>

#include <random>

using namespace valkyrie;

uuid uuid::generate() noexcept {
  std::mt19937 rnd{std::random_device{}()};
  u32 bytes[4];
  bytes[0] = rnd();
  bytes[1] = rnd() ^ (rnd() >> 9);
  rnd.discard(3);
  bytes[2] = rnd();
  rnd.discard(7);
  bytes[3] = rnd();
  return uuid(reinterpret_cast<byte(&)[16]>(bytes));
}