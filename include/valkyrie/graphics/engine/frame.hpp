//
// Created by maxwe on 2021-05-08.
//

#ifndef VALKYRIE_GRAPHICS_ENGINE_FRAME_HPP
#define VALKYRIE_GRAPHICS_ENGINE_FRAME_HPP

#include <valkyrie/traits.hpp>

namespace valkyrie::graphics::engine{

  struct surface{
    VkSurfaceKHR handle;
  };

  struct frame{
    u32             index;

    VkImage         image;
    VkImageView     imageView;
    VkCommandBuffer commandBuffer;

    VkSemaphore     semaphore;
  };

  struct swapchain{

    u32               frameCount;
    frame*            frames;

    frame*            currentFrame;

    surface*          pSurface;

    VkSwapchainKHR    handle;

    VkFormat          imageFormat;
    VkColorSpaceKHR   imageColorspace;
    VkImageUsageFlags imageUsage;
    VkPresentModeKHR  presentMode;
    extent2d<>        imageExtent;
    bool              isOutdated;
  };


}

#endif  //VALKYRIE_GRAPHICS_ENGINE_FRAME_HPP
