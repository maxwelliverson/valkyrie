//
// Created by Maxwell on 2020-11-09.
//

#include <valkyrie/Core/ADT/Arrays.hpp>
#include <valkyrie/Core/Utility/Uuid.hpp>

#include <random>

using namespace valkyrie;

Core::Uuid Core::Uuid::generate() noexcept {
  std::mt19937 rnd{std::random_device{}()};
  u32 bytes[4];
  bytes[0] = rnd();
  bytes[1] = rnd() ^ (rnd() >> 9);
  rnd.discard(3);
  bytes[2] = rnd();
  rnd.discard(7);
  bytes[3] = rnd();
  return Uuid(reinterpret_cast<byte(&)[16]>(bytes));
}