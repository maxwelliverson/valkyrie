//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_LOGGER_HPP
#define VALKYRIE_LOGGER_HPP

#include <valkyrie/Core/IO/Messages.hpp>

namespace valkyrie::Core{
  class Logger{};

  struct Log{
    const static Logger verbose;
    const static Logger info;
    const static Logger warning;
    const static Logger error;
  };
}

#endif//VALKYRIE_LOGGER_HPP
