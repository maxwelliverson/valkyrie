//
// Created by Maxwell on 2020-11-19.
//

#ifndef VALKYRIE_POSIX_ERROR_HPP
#define VALKYRIE_POSIX_ERROR_HPP

#include <valkyrie/Core/Error/Status.hpp>

namespace valkyrie::Core::System{
  namespace Detail{
    class PosixStatusDomain : public StatusDomain{

    };
  }

  using PosixStatus = StatusCode<Detail::PosixStatusDomain>;
  using PosixError  = ErrorStatus<Detail::PosixStatusDomain>;
}


#endif//VALKYRIE_POSIX_ERROR_HPP
