//
// Created by maxwe on 2021-05-16.
//

#ifndef VALKYRIE_READ_STREAM_HPP
#define VALKYRIE_READ_STREAM_HPP

#include <valkyrie/traits.hpp>

namespace valkyrie{

  class read_stream{


  public:

  protected:
    virtual u64 do_read(byte* address, u64 readSize) noexcept = 0;
  };
}

#endif  //VALKYRIE_READ_STREAM_HPP
