//
// Created by Maxwell on 2020-11-15.
//

#ifndef VALKYRIE_ENGINE_STREAM_HPP
#define VALKYRIE_ENGINE_STREAM_HPP

#include <valkyrie/Graphics/API/Queue.hpp>

namespace valkyrie::Graphics::Engine{
  class Stream{
    struct {
      API::GraphicsQueue lowPriority;
      API::GraphicsQueue averagePriority;
      API::GraphicsQueue highPriority;
    } graphics, compute;
    API::TransferQueue transfer;
    API::SparseBindingQueue sparseBinding;
  public:

  };
}

#endif//VALKYRIE_ENGINE_STREAM_HPP
