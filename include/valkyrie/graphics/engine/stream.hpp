//
// Created by Maxwell on 2020-11-15.
//

#ifndef VALKYRIE_ENGINE_STREAM_HPP
#define VALKYRIE_ENGINE_STREAM_HPP

#include <valkyrie/graphics/api/queue.hpp>

namespace valkyrie::graphics::engine{
  class stream{

    enum { low_priority, average_priority, high_priority };

    api::graphics_queue graphics[3];
    api::compute_queue  compute[3];
    api::transfer_queue transfer;
    api::sparse_binding_queue sparseBinding;
  public:

  };
}

#endif//VALKYRIE_ENGINE_STREAM_HPP
