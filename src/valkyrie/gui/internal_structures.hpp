//
// Created by Maxwell on 2020-11-30.
//

#ifndef VALKYRIE_GUI_INTERNAL_STRUCTURES_HPP
#define VALKYRIE_GUI_INTERNAL_STRUCTURES_HPP

#include <valkyrie/adt/arrays.hpp>
#include <valkyrie/gui/monitor.hpp>

#include <atomic>

#define WIN32_MEAN_AND_LEAN
#define NOMINMAX
#include <windows.h>
#include <highlevelmonitorconfigurationapi.h>
#include <physicalmonitorenumerationapi.h>

namespace valkyrie::GUI{
  namespace Internal{

    struct GraphicsMode{
      u32 bitsPerPixel;
      u32 pixelWidth;
      u32 pixelHeight;
      u32 displayFrequency;
      bool interlaced;
    };

    struct MonitorImpl{
      HMONITOR                         handle;
      HANDLE                           physicalMonitorHandle;
      GraphicsMode                     currentGraphicsMode;
      graphics::Position2D<i32>        position;
      std::string                      path;
      std::string                      name;
      DynamicArray<GraphicsMode> graphicsModes;
      std::atomic_uint32_t             refCount;
      bool                             primary;
      bool                             available;

      MonitorImpl() = default;
      MonitorImpl(const MonitorImpl& other)
          : handle(other.handle),
            physicalMonitorHandle(other.physicalMonitorHandle),
            currentGraphicsMode(other.currentGraphicsMode),
            position(other.position),
            path(other.path),
            name(other.name),
            graphicsModes(other.graphicsModes),
            refCount(),
            primary(other.primary),
            available(other.available)
      {
        refCount.store(other.refCount.load());
      }
    };

    void destroyRef(MonitorImpl* pImpl) noexcept {
      if (pImpl && !--pImpl->refCount) {
        if (!DestroyPhysicalMonitor(pImpl->physicalMonitorHandle))
          panic(sys::win32::getLastError());
        delete pImpl;
      }
    }

    struct WindowImpl{
      HWND                      handle;
      std::u8string             windowTitle;
      u32                       width;
      u32                       height;
      i32                       x;
      i32                       y;
      monitor monitor;
      std::atomic_uint32_t      refCount;
    };

    void destroyRef(WindowImpl* pImpl) noexcept {
      if (pImpl && !--pImpl->refCount) {
        if (!CloseWindow(pImpl->handle))
          panic(sys::win32::getLastError());
        delete pImpl;
      }
    }
  }
}

#endif//VALKYRIE_GUI_INTERNAL_STRUCTURES_HPP
