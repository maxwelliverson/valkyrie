//
// Created by maxwe on 2021-05-06.
//

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