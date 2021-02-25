//
// Created by Maxwell on 2021-01-29.
//

#include <valkyrie/Core/Config.hpp>
#include <valkyrie/Core/Utility/Version.hpp>
#include <valkyrie/Core/Utility/Shapes.hpp>
#include <valkyrie/Core/Utility/Interval.hpp>
#include <valkyrie/Core/Utility/StringView.hpp>

#include <cuda.h>
#include <curand_discrete2.h>
#include <curand_globals.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>



#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <span>
#include <memory_resource>

template <typename T>
struct MultiPointerTraits;



template <typename T, typename ...MemTypes>
class MultiPointer{

};


template <typename T, typename Alloc = std::allocator<T>>
class TempArray{

  using alloc_traits = std::allocator_traits<Alloc>;

public:

  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = T;
  using pointer = typename alloc_traits::pointer;
  using reference = decltype(*std::declval<pointer>());
  using const_pointer = typename alloc_traits::const_pointer;
  using const_reference = decltype(*std::declval<const_pointer>());

  using iterator = pointer;
  using const_iterator = const_pointer;
  using sentinel = iterator;
  using const_sentinel = const_iterator;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using reverse_sentinel = reverse_iterator;
  using const_reverse_sentinel = const_reverse_iterator;

  TempArray() = default;
  TempArray(const TempArray&) = delete;
  TempArray(TempArray&& other) noexcept : allocator_(std::move(other.allocator_)), ptr_(other.ptr_), size_(other.size_){
    other.ptr_ = nullptr;
    other.size_ = 0; // not strictly necessary?
  }

  explicit TempArray(size_t length_) : TempArray(length_, {}){}
  TempArray(size_t length_, const Alloc& allocator) : allocator_(allocator), ptr_(alloc_traits::allocate(allocator_, length_)), size_(length_){
    if constexpr (!std::is_trivially_default_constructible_v<T>) {
      for (size_t i = 0; i < length_; ++i) {
        alloc_traits::construct(allocator_, ptr_ + i);
      }
    }
  }

  ~TempArray() {
    if (ptr_) {
      if constexpr (!std::is_trivially_destructible_v<T>) {
        for (size_t i = 0; i < size_; ++i)
          std::allocator_traits<Alloc>::destroy(allocator_, ptr_ + i);
      }
      std::allocator_traits<Alloc>::deallocate(allocator_, ptr_, size_);
    }
  }



  reference operator[](size_type offset) const noexcept {
    VK_assert(offset < size_);
    return ptr_[offset];
  }


  size_type size() const noexcept {
    return size_;
  }
  pointer data() const noexcept {
    return ptr_;
  }


  iterator  begin() const noexcept {
    return ptr_;
  }
  const_iterator cbegin() const noexcept {
    return ptr_;
  }

  sentinel end() const noexcept {
    return ptr_ + size_;
  }
  const_sentinel cend() const noexcept {
    return end();
  }

  reverse_iterator  rbegin() const noexcept {
    return reverse_iterator(end());
  }
  const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(cend());
  }

  reverse_sentinel rend() const noexcept {
    return reverse_sentinel(begin());
  }
  const_reverse_sentinel crend() const noexcept {
    return const_reverse_sentinel(cbegin());
  }






  operator std::span<T>() const noexcept {
    return { ptr_, size_ };
  }


private:
  [[no_unique_address]] Alloc allocator_{};
  pointer                     ptr_ = nullptr;
  size_type                   size_ = 0;
};

namespace Detail{
  class AnyPointerProxy{
  public:
    template <typename T>
    operator T*() const noexcept;
  };
}

template <typename A, typename T = void>
concept Allocator = requires(A& a, size_t n){
  { a.allocate(n) } -> std::convertible_to<T*>;
} && (std::is_void_v<T> || requires(A& a, T* p, size_t n){ { a.deallocate(p, n) }; });

// I like this name lmao
template <typename S, typename T = void>
concept Spanish = requires(S&& s){
  { std::span(std::forward<S>(s)) } -> std::convertible_to<std::span<T>>;
} || (std::is_void_v<T> && requires(S&& s){ std::span(std::forward<S>(s)); });


/*[[noreturn]] inline static void cuTerminate(CUresult result, const char* func, const char* file, int line) noexcept {
  const char* errorName, *errorString;
  if (cuGetErrorName(result, &errorName) | cuGetErrorString(result, &errorString))
    std::cerr << "CUDA Driver Internal Error";
  else
    std::cerr << errorName << ": " << errorString;
  std::cerr << "\n\n\t* * * *\n\nOrigin:\n\n\tLocation: " << file << ":" << line  << "\n\tFunction: " << func << "\n\n\t* * * *\n\nTerminating Program..." << std::endl;
  std::terminate();
}*/
[[noreturn]] inline static void cuAssertionFailure(CUresult result, const char* apiCall, const char* func, const char* file, int line) noexcept {
  const char* errorName, *errorString;
  std::cerr << "Assertion Failed: ";
  if (cuGetErrorName(result, &errorName) | cuGetErrorString(result, &errorString))
    std::cerr << apiCall << " caused a CUDA Driver Internal Error.";
  else
    std::cerr << apiCall << " returned " << errorName << " (" << errorString << ").";
  std::cerr << "\n\n\t* * * *\n\nOrigin:\n\nLocation: " << file << ":" << line  << "\n\tScope: " << func << "\n\n\t* * * *\n\nTerminating Program..." << std::endl;
  std::terminate();
}

#define PP_CU_impl_TRY_CALL(...) (__VA_ARGS__) ) throw cu::cuda_driver_error(CU_callResult_)
#define PP_CU_impl_CALL(...) (__VA_ARGS__) ) cuTerminate(CU_callResult_, VK_function_name, VK_filename, VK_line_number)
#if !defined(NDEBUG)

#define PP_CU_impl_ASSERT_CALL_redirect_2(fun, ...) if (auto CU_assertResult = fun(__VA_ARGS__)) cuAssertionFailure(CU_assertResult, #fun, VK_function_name, VK_filename, VK_line_number)
#define PP_CU_impl_ASSERT_CALL PP_CU_impl_ASSERT_CALL_macro()
#define PP_CU_impl_ASSERT_CALL_macro() PP_CU_impl_ASSERT_CALL_redirect
#define PP_CU_impl_ASSERT_CALL_redirect(...) PP_CU_impl_ASSERT_CALL_redirect_2(__VA_ARGS__)
#define PP_CU_impl_ASSERT_CALL_WRAP_ARGS_redirect(...) (__VA_ARGS__) )
#define PP_CU_impl_ASSERT_CALL_WRAP_ARGS PP_CU_impl_ASSERT_CALL_WRAP_ARGS_redirect
#define CU_assert_call(fun) PP_CU_impl_ASSERT_CALL(fun, PP_CU_impl_ASSERT_CALL_WRAP_ARGS
#else
#define PP_CU_impl_ASSERT_CALL(...) (__VA_ARGS__)
#define CU_assert_call(fun) cu##fun PP_CU_impl_ASSERT_CALL
#endif

#define CU_call(fun) if (auto CU_callResult_ = cu##fun PP_CU_impl_CALL
#define CU_try_call(fun) if (auto CU_callResult_ = cu##fun PP_CU_impl_TRY_CALL

#define CU_getErrorName(result, str) PP_CU_impl_ASSERT_CALL(cuGetErrorName, result, str)
#define CU_getErrorString(result, str) PP_CU_impl_ASSERT_CALL(cuGetErrorString, result, str)

#define CU_graphCreate(graph) PP_CU_impl_ASSERT_CALL(cuGraphCreate, graph, 0)
#define CU_graphDestroy(graph) PP_CU_impl_ASSERT_CALL(cuGraphDestroy, graph)
#define CU_graphNodeGetType(graphNode, type) PP_CU_impl_ASSERT_CALL(cuGraphNodeGetType, graphNode, type)
#define CU_graphNodeRemove(graphNode) PP_CU_impl_ASSERT_CALL(cuGraphDestroyNode, graphNode)
#define CU_graphExecDestroy(graphExec) PP_CU_impl_ASSERT_CALL(cuGraphExecDestroy, graphExec)
#define CU_graphGetNodes(graph, nodes, nodeCount) PP_CU_impl_ASSERT_CALL(cuGraphGetNodes, graph, nodes, nodeCount)
#define CU_graphGetEdges(graph, fromNodes, toNodes, edgeCount) PP_CU_impl_ASSERT_CALL(cuGraphGetEdges, graph, fromNodes, toNodes, edgeCount)
#define CU_graphGetRootNodes(graph, rootNodes, rootNodeCount) PP_CU_impl_ASSERT_CALL(cuGraphGetRootNodes, graph, rootNodes, rootNodeCount)
#define CU_graphFindInClone(resultNode, originalNode, clonedGraph) PP_CU_impl_ASSERT_CALL(cuGraphNodeFindInClone, resultNode, originalNode, clonedGraph)

#define CU_graphAddMemsetNode(nodeResult, graph, pDeps, depsCount, memsetParams, ctx) PP_CU_impl_ASSERT_CALL(cuGraphAddMemsetNode, nodeResult, graph, pDeps, depsCount, memsetParams, ctx)




#define CU_ctxDestroy(ctx) PP_CU_impl_ASSERT_CALL(cuCtxDestroy, ctx)
#define CU_deviceGet(resultDev, ordinal)             PP_CU_impl_ASSERT_CALL(cuDeviceGet,               resultDev, ordinal)
#define CU_devicePrimaryCtxRetain(resultCtx, device) PP_CU_impl_ASSERT_CALL(cuDevicePrimaryCtxRetain,  resultCtx, device)
#define CU_devicePrimaryCtxRelease(device)           PP_CU_impl_ASSERT_CALL(cuDevicePrimaryCtxRelease, device)
#define CU_devicePrimaryCtxReset(device)             PP_CU_impl_ASSERT_CALL(cuDevicePrimaryCtxReset,   device)
#define CU_devicePrimaryCtxGetState(device, flagsResult, activeResult)   PP_CU_impl_ASSERT_CALL(cuDevicePrimaryCtxGetState, device, flagsResult, activeResult)
#define CU_devicePrimaryCtxSetFlags(device, flags)   PP_CU_impl_ASSERT_CALL(cuDevicePrimaryCtxSetFlags, device, flags)

#define CU_ctxResetPersistingL2Cache()               PP_CU_impl_ASSERT_CALL(cuCtxResetPersistingL2Cache)
#define CU_ctxSynchronize()                          PP_CU_impl_ASSERT_CALL(cuCtxSynchronize)

#define CU_ctxGetApiVersion(context, versionResult)  PP_CU_impl_ASSERT_CALL(cuCtxGetApiVersion, context, versionResult)
#define CU_ctxGetDevice(resultDevice)                PP_CU_impl_ASSERT_CALL(cuCtxGetDevice, resultDevice)
#define CU_ctxGetFlags(resultFlags)                  PP_CU_impl_ASSERT_CALL(cuCtxGetFlags, resultFlags)

#define CU_ctxSetCacheConfig(config)                     PP_CU_impl_ASSERT_CALL(cuCtxSetCacheConfig, config)
#define CU_ctxSetSharedMemConfig(config)                 PP_CU_impl_ASSERT_CALL(cuCtxSetSharedMemConfig, config)
#define CU_ctxSetLimit(limitKind, limitValue)            PP_CU_impl_ASSERT_CALL(cuCtxSetLimit, limitKind, limitValue)
#define CU_ctxGetCacheConfig(configResult)               PP_CU_impl_ASSERT_CALL(cuCtxGetCacheConfig, configResult)
#define CU_ctxGetSharedMemConfig(configResult)           PP_CU_impl_ASSERT_CALL(cuCtxGetSharedMemConfig, configResult)
#define CU_ctxGetLimit(limitResult, limitKind)           PP_CU_impl_ASSERT_CALL(cuCtxGetLimit, limitResult, limitKind)

#define CU_ctxPushCurrent(context)                       PP_CU_impl_ASSERT_CALL(cuCtxPushCurrent, context)
#define CU_ctxPopCurrent(contextResult)                  PP_CU_impl_ASSERT_CALL(cuCtxPopCurrent,  contextResult)
#define CU_ctxGetCurrent(contextResult)                  PP_CU_impl_ASSERT_CALL(cuCtxGetCurrent,  contextResult)
#define CU_ctxSetCurrent(context)                        PP_CU_impl_ASSERT_CALL(cuCtxSetCurrent,  context)
#define CU_ctxGetStreamPriorityRange(loResult, hiResult) PP_CU_impl_ASSERT_CALL(cuCtxGetStreamPriorityRange, loResult, hiResult)




namespace cu {

  using
  valkyrie::Core::Box2D,
  valkyrie::Core::Box3D,
  valkyrie::Core::Extent2D,
  valkyrie::Core::Extent3D,
  valkyrie::Core::Position2D,
  valkyrie::Core::Position3D,
  valkyrie::Core::Version,
  valkyrie::Core::Interval;

  class cuda_driver_error : public std::runtime_error {
    CUresult result_;

    inline static const char *getErrorName(CUresult result) noexcept {
      const char *pErrorName;
      CU_getErrorName(result, &pErrorName);
      return pErrorName;
    }

  public:
    explicit cuda_driver_error(CUresult result) noexcept : std::runtime_error(getErrorName(result)), result_(result) {}

    CUresult errorCode() const noexcept {
      return result_;
    }
    const char *what() const noexcept override {
      const char *pErrorString;
      CU_getErrorString(result_, &pErrorString);
      return pErrorString;
    }
  };

  template<typename Derived, typename Wrapped>
  class Handle {
  protected:
    Wrapped handle_{};

  public:

    using wrapped_handle_type = Derived;
    using native_handle_type = Wrapped;


    Handle() = default;
    Handle(Wrapped handle) noexcept : handle_(handle) {}

    Wrapped getNativeHandle() const noexcept {
      return handle_;
    }
    void setNativeHandle(Wrapped handle) noexcept {
      handle_ = handle;
    }

    bool hasOwnership() const noexcept {
      return true;
    }


    explicit operator bool() const noexcept {
      return bool(handle_);
    }

    friend bool operator==(Derived A, Derived B) noexcept {
      return A.handle_ == B.handle_;
    }
    friend std::strong_ordering operator<=>(Derived A, Derived B) noexcept {
      return A.handle_ <=> B.handle_;
    }
  };



  template <typename H>
  concept WrappedHandle =
      requires{
    typename H::native_handle_type;
    typename H::wrapped_handle_type;
  } &&
      requires(H& h, typename H::native_handle_type nh){
    h.setNativeHandle(nh);
    nh = h.getNativeHandle();
  } &&
      std::derived_from<H, Handle<typename H::wrapped_handle_type, typename H::native_handle_type>>;


  // Dumb workaround to deal with the fact that constrained template parameters generally can't be
  // declared with types that are incomplete at the point of specialization declaration, even if the
  // template is not instantiated at that point. (eg. in function signature)
  // This isn't a huge issue in most scenarios, but there is one fairly substantial consequence,
  // that being that within a class declaration, constrained templates *cannot* be specialized on that class.
  // This means, for example, that any kind of instances of any kind of template used to wrap objects cannot
  // be used as parameter or return value of any member functions.

  /*
   * Something rotten in the state of the standard.
   *
   * As they currently stand in the C++ language, concepts are incredibly useful for use in any kind of programming
   * involving templates, and it generally makes sense to use them as liberally as possible. They're quite robust,
   * and almost entirely logically sound. There is, however, a problem with their specification.
   *
   * One of the axioms upon which the C++ type system is built is the fact that type definitions are invariant.
   * Template instantiation is generally understood to be a functional operation, and this is such an important
   * and fundamental fact that it dictates much of the design of C++ compilers. The problem is that it's not.
   *
   * The standard acknowledges a couple edge cases whereby the results of instantiation of a template might differ
   * depending on the location in the source code.
   *
   *
   *
   * Without this axiom, almost all of the benefits conferred by static typing would be lost (what use is it
   * knowing the type of a function parameter at compile time if said type could mean something entirely different
   * depending on where in the source code that function is called?) Most of the power afforded by templates comes
   * from knowing that type definitions, and thus properties, are invariant (instantiation would no longer be a purely functional operation)
   *
   *
   * Eg.
   *
   *   Good:
   *
   *     template <typename T>
   *     class ObjectRef{
   *     public:
   *       using wrapped_type = typename T::object_type;
   *
   *       ...
   *     };
   *
   *     struct AType{};
   *
   *     class Object{
   *     public:
   *
   *       using object_type = AType;
   *
   *       ObjectRef<Object> getRef();
   *     };
   *
   *
   *
   * In this example, ObjectRef requires that its template parameter declares a nested type named "object_type".
   * Compilation will fail otherwise. In this scenario
   *
   *
   *   Bad:
   *
   *     template <typename T>
   *     concept ObjectLike = requires{ typename T::wrapped_type; } && ...;
   *
   *     template <ObjectLike T>
   *     class ObjectRef{
   *     public:
   *       using wrapped_type = typename T::object_type;
   *
   *       ...
   *     };
   *
   *     struct AType{};
   *
   *     class Object{
   *     public:
   *
   *       using object_type = AType;
   *
   *       ObjectRef<Object> getRef();
   *     };
   *
   *
   *
   * */


  template <typename T>
  concept Incomplete = !requires{ sizeof(T); };
  template <typename H>
  concept WrappedHandleFwd = WrappedHandle<H> || Incomplete<H>;

  template <WrappedHandleFwd H>
  class Ref : public H{


    static_assert(WrappedHandle<H>);

  public:
    using H::H;
    Ref() noexcept : H(nullptr){}
    Ref(const H& handle) noexcept : H(handle.getNativeHandle()){

    }
    ~Ref() {
      this->setNativeHandle(nullptr);
    }

    bool hasOwnership() const noexcept {
      return false;
    }

    operator H() const noexcept = delete; // No implicit slicing to the underlying type allowed >:(
  };

  class Context;
  class PrivateContext;
  class PrimaryContext;
  class ActiveContext;


  enum class StreamFlags{
    Default,
    NonBlocking
  };

  enum class ContextScheduling{
    Auto          = 0x00, /**< Automatic scheduling */
    Spin          = 0x01, /**< Set spin as default scheduling */
    Yield         = 0x02, /**< Set yield as default scheduling */
    BlockingSync  = 0x04, /**< Set blocking synchronization as default scheduling */
  };
  enum class CachePreference{
    None,
    Shared,
    L1,
    Equal
  };
  enum class SharedMemoryBankSize{
    Default,
    FourByte,
    EightByte
  };
  enum class ResourceKind{
    Host,
    Device,
    Unified,
    Array,
    TextureObject,
    SurfaceObject,
    Graphics,
    External
  };

  enum class AccessKind{
    ReadOnly = 0x1,
    WriteOnly,
    ReadWrite
  };

  enum class ResourceOpKind{
    Source,
    Destination
  };


  class ResourcePlaceholder{
  public:
    valkyrie::Core::StringView name() const noexcept;
    uint64_t                   id() const noexcept;
  };
  class Resource{
  public:

    virtual ~Resource() = default;

    virtual void             doFillMemsetParams(CUDA_MEMSET_NODE_PARAMS& memsetInfo, const Box3D<size_t>& areaDesc) const noexcept = 0;
    virtual void             doFillMemcpyParams(ResourceOpKind opKind, CUDA_MEMCPY3D& memcpyInfo, const Position3D<size_t>& offset, const Extent3D<size_t>& areaDesc) const noexcept = 0;
    
    virtual uint64_t         getID() const noexcept = 0;
    virtual size_t           getPitch() const noexcept = 0;
    virtual Extent3D<size_t> getExtents() const noexcept = 0;
  };

  class ResourceObject : public virtual Resource{};
  class Resource1D     : public virtual Resource{
    size_t length;
  };
  class Resource2D     : public virtual Resource{
    Extent2D<size_t> extent;
  };
  class Resource3D     : public virtual Resource{
    Extent3D<size_t> extent;
  };

  class Array     : public Handle<Array,     CUarray>,      public virtual Resource{
  public:
    ~Array() override {
      if (handle_)
        cuArrayDestroy(handle_);
    }
  };
  class Texture   : public Handle<Texture,   CUtexObject>,  public virtual Resource{};
  class Surface   : public Handle<Surface,   CUsurfObject>, public virtual Resource{};
  class RawMemory : public Handle<RawMemory, CUdeviceptr>,  public virtual Resource{};
  class ExternalResource : public virtual Resource{

  };
  class GraphicsResource : public virtual Resource{};




  class Array1D   : public Array,   public Resource1D { };
  class Array2D   : public Array,   public Resource2D { };
  class Array3D   : public Array,   public Resource3D { };

  class Texture1D : public Texture, public Resource1D { };
  class Texture2D : public Texture, public Resource2D { };
  class Texture3D : public Texture, public Resource3D { };

  class Surface1D : public Surface, public Resource1D { };
  class Surface2D : public Surface, public Resource2D { };
  class Surface3D : public Surface, public Resource3D { };

  class DevicePtr   : public Handle<DevicePtr, CUdeviceptr>{

  };
  class DeviceMemorySpan{
    DevicePtr memory;
    size_t    offset;
    size_t    length;
  };









  class Device         : public Handle<Device, CUdevice> {
    using Base_ = Handle<Device, CUdevice>;
  public:
    Device() : Device(0){}
    explicit Device(int ordinal) : Base_(){
      CU_try_call(DeviceGet)(&handle_, ordinal);
    }

    inline PrimaryContext primaryContext() const noexcept;
  };
  class Context        : public Handle<Context, CUcontext>{

  protected:
    using Handle<Context, CUcontext>::Handle;
  public:

    Version apiVersion() const noexcept {
      uint32_t verNum;
      CU_ctxGetApiVersion(handle_, &verNum);
      return Version{ verNum / 1000, (verNum % 1000) / 10 };
    }
  };

  namespace CurrentContext{
    inline static void                 push(const Ref<Context> ctx) noexcept {
      CU_ctxPushCurrent(ctx.getNativeHandle());
    }
    inline static Ref<Context>         pop() noexcept {
      CUcontext ctx;
      CU_ctxPopCurrent(&ctx);
      return Ref<Context>(ctx);
    }

    inline static void                 set(const Ref<Context> ctx) noexcept {
      CU_ctxSetCurrent(ctx.getNativeHandle());
    }
    inline static Ref<Context>         get() noexcept {
      CUcontext ctx;
      CU_ctxGetCurrent(&ctx);
      return Ref<Context>(ctx);
    }


    inline static size_t               getStackSize() noexcept {
      size_t result;
      CU_ctxGetLimit(&result, CU_LIMIT_STACK_SIZE);
      return result;
    }
    inline static void                 setStackSize(size_t size) noexcept {
      CU_ctxSetLimit(CU_LIMIT_STACK_SIZE, size);
    }
    inline static size_t               getMallocHeapSize() noexcept {
      size_t result;
      CU_ctxGetLimit(&result, CU_LIMIT_MALLOC_HEAP_SIZE);
      return result;
    }
    inline static void                 setMallocHeapSize(size_t size) noexcept {
      CU_ctxSetLimit(CU_LIMIT_MALLOC_HEAP_SIZE, size);
    }
    inline static size_t               getPrintfBufferSize() noexcept {
      size_t result;
      CU_ctxGetLimit(&result, CU_LIMIT_PRINTF_FIFO_SIZE);
      return result;
    }
    inline static void                 setPrintfBufferSize(size_t size) noexcept {
      CU_ctxSetLimit(CU_LIMIT_PRINTF_FIFO_SIZE, size);
    }
    inline static size_t               getMaxSyncDepth() noexcept {
      size_t result;
      CU_ctxGetLimit(&result, CU_LIMIT_DEV_RUNTIME_SYNC_DEPTH);
      return result;
    }
    inline static void                 setMaxSyncDepth(size_t size) noexcept {
      CU_ctxSetLimit(CU_LIMIT_DEV_RUNTIME_SYNC_DEPTH, size);
    }
    inline static size_t               getMaxPendingLaunchCount() noexcept {
      size_t result;
      CU_ctxGetLimit(&result, CU_LIMIT_DEV_RUNTIME_PENDING_LAUNCH_COUNT);
      return result;
    }
    inline static void                 setMaxPendingLaunchCount(size_t size) noexcept {
      CU_ctxSetLimit(CU_LIMIT_DEV_RUNTIME_PENDING_LAUNCH_COUNT, size);
    }
    inline static size_t               getMaxL2FetchGranularity() noexcept {
      size_t result;
      CU_ctxGetLimit(&result, CU_LIMIT_MAX_L2_FETCH_GRANULARITY);
      return result;
    }
    inline static void                 setMaxL2FetchGranularity(size_t size) noexcept {
      CU_ctxSetLimit(CU_LIMIT_MAX_L2_FETCH_GRANULARITY, size);
    }
    inline static size_t               getPersistingL2CacheSize() noexcept {
      size_t result;
      CU_ctxGetLimit(&result, CU_LIMIT_PERSISTING_L2_CACHE_SIZE);
      return result;
    }
    inline static void                 setPersistingL2CacheSize(size_t size) noexcept {
      CU_ctxSetLimit(CU_LIMIT_PERSISTING_L2_CACHE_SIZE, size);
    }

    inline static Interval<int32_t>    streamPriorityRange() noexcept {
      Interval<int32_t> result;
      CU_ctxGetStreamPriorityRange(&result.min, &result.max);
      return result;
    }


    inline static void                 setSharedMemoryBankSize(SharedMemoryBankSize bankSize) noexcept {
      CU_ctxSetSharedMemConfig((CUsharedconfig)(uint32_t)bankSize);
    }
    inline static SharedMemoryBankSize getSharedMemoryBankSize() noexcept {
      CUsharedconfig config;
      CU_ctxGetSharedMemConfig(&config);
      return static_cast<SharedMemoryBankSize>(static_cast<uint32_t>(config));
    }
    inline static void                 setCachePreference(CachePreference cachePref) noexcept{
      CU_ctxSetCacheConfig((CUfunc_cache)(uint32_t)cachePref);
    }
    inline static CachePreference      getCachePreference() noexcept {
      CUfunc_cache cacheConfig;
      CU_ctxGetCacheConfig(&cacheConfig);
      return CachePreference((uint32_t)cacheConfig);
    }

    inline static ContextScheduling    scheduling() noexcept {
      ContextScheduling sched;
      CU_ctxGetFlags((uint32_t*)&sched);
      return sched;
    }
    inline static Device               device() noexcept {
      CUdevice dev;
      CU_ctxGetDevice(&dev);
      return Device(dev);
    }
    inline static void                 sync() noexcept{
      CU_ctxSynchronize();
    }
    inline static void                 resetPersistingL2Cache() noexcept {
      CU_ctxResetPersistingL2Cache();
    }
  }

  class PrimaryContext   : public Context{
    friend class Device;
    Device device{};
    explicit PrimaryContext(Device device) noexcept : device(device){
      CU_devicePrimaryCtxRetain(&handle_, device.getNativeHandle());
    }
  public:
    PrimaryContext() = delete;
    ~PrimaryContext(){
      CU_devicePrimaryCtxRelease(this->device.getNativeHandle());
    }


    ContextScheduling getScheduling() const noexcept {
      uint32_t flagsResult;
      int32_t  activeResult; // Always true on any device with CUDA 4.0 or better capabilities.
      CU_devicePrimaryCtxGetState(device.getNativeHandle(), &flagsResult, &activeResult);
      return ContextScheduling(flagsResult);
    }
    void              setScheduling(ContextScheduling sched) const noexcept {
      CU_devicePrimaryCtxSetFlags(device.getNativeHandle(), (uint32_t)sched);
    }
    void reset() const noexcept {

    }
  };
  class PrivateContext   : public Context{
  public:
    explicit PrivateContext(Device device, ContextScheduling sched = ContextScheduling::Auto){
      cuCtxCreate(&handle_, (uint32_t)sched, device.getNativeHandle());
    }
    ~PrivateContext(){
      CU_ctxDestroy(handle_);
    }
  };
  class ActiveContext    : public Ref<Context>{
  public:

    ActiveContext(const Context& ctx) noexcept : Ref<Context>(ctx){
      CurrentContext::push(*this);
    }
    ActiveContext(const ActiveContext& other) noexcept : ActiveContext(static_cast<const Context&>(other)){}

    ActiveContext& operator=(const Context& ctx) noexcept {
      CurrentContext::set(ctx);
      this->handle_ = ctx.getNativeHandle();
      return *this;
    }
    ActiveContext& operator=(const ActiveContext& other) noexcept {
      *this = static_cast<const Context&>(other);
      return *this;
    }


    size_t getStackSize() const noexcept {
      VK_assert(CurrentContext::get() == *this);
      return CurrentContext::getStackSize();
    }
    void   setStackSize(size_t size) const noexcept {
      VK_assert(CurrentContext::get() == *this);
      CurrentContext::setStackSize(size);
    }
    size_t getMallocHeapSize() const noexcept {
      VK_assert(CurrentContext::get() == *this);
      return CurrentContext::getMallocHeapSize();
    }
    void   setMallocHeapSize(size_t size) const noexcept {
      VK_assert(CurrentContext::get() == *this);
      CurrentContext::setMallocHeapSize(size);
    }
    size_t getPrintfBufferSize() const noexcept {
      VK_assert(CurrentContext::get() == *this);
      return CurrentContext::getPrintfBufferSize();
    }
    void   setPrintfBufferSize(size_t size) const noexcept {
      VK_assert(CurrentContext::get() == *this);
      CurrentContext::setPrintfBufferSize(size);
    }
    size_t getMaxSyncDepth() const noexcept {
      VK_assert(CurrentContext::get() == *this);
      return CurrentContext::getMaxSyncDepth();
    }
    void   setMaxSyncDepth(size_t size) const noexcept {
      VK_assert(CurrentContext::get() == *this);
      CurrentContext::setMaxSyncDepth(size);
    }
    size_t getMaxPendingLaunchCount() const noexcept {
      VK_assert(CurrentContext::get() == *this);
      return CurrentContext::getMaxPendingLaunchCount();
    }
    void   setMaxPendingLaunchCount(size_t size) const noexcept {
      VK_assert(CurrentContext::get() == *this);
      CurrentContext::setMaxPendingLaunchCount(size);
    }
    size_t getMaxL2FetchGranularity() const noexcept {
      VK_assert(CurrentContext::get() == *this);
      return CurrentContext::getMaxL2FetchGranularity();
    }
    void   setMaxL2FetchGranularity(size_t size) const noexcept {
      VK_assert(CurrentContext::get() == *this);
      CurrentContext::setMaxL2FetchGranularity(size);
    }
    size_t getPersistingL2CacheSize() const noexcept {
      VK_assert(CurrentContext::get() == *this);
      return CurrentContext::getPersistingL2CacheSize();
    }
    void   setPersistingL2CacheSize(size_t size) const noexcept {
      VK_assert(CurrentContext::get() == *this);
      CurrentContext::setPersistingL2CacheSize(size);
    }

    Interval<int32_t> streamPriorityRange() const noexcept {
      VK_assert(CurrentContext::get() == *this);
      return CurrentContext::streamPriorityRange();
    }


    void setSharedMemoryBankSize(SharedMemoryBankSize bankSize) const noexcept {
      VK_assert(CurrentContext::get() == *this);
      CurrentContext::setSharedMemoryBankSize(bankSize);
    }
    SharedMemoryBankSize getSharedMemoryBankSize() const noexcept {
      VK_assert(CurrentContext::get() == *this);
      return CurrentContext::getSharedMemoryBankSize();
    }
    void setCachePreference(CachePreference cachePref) noexcept {
      VK_assert(CurrentContext::get() == *this);
      CurrentContext::setCachePreference(cachePref);
    }
    CachePreference getCachePreference() const noexcept {
      VK_assert(CurrentContext::get() == *this);
      return CurrentContext::getCachePreference();
    }

    ContextScheduling scheduling() const noexcept {
      VK_assert(CurrentContext::get() == *this);
      return CurrentContext::scheduling();
    }
    Device device() const noexcept {
      VK_assert(CurrentContext::get() == *this);
      return CurrentContext::device();
    }
    void sync() const noexcept {
      VK_assert(CurrentContext::get() == *this);
      CurrentContext::sync();
    }
    void resetPersistingL2Cache() const noexcept{
      VK_assert(CurrentContext::get() == *this);
      CurrentContext::resetPersistingL2Cache();
    }

    ~ActiveContext(){
      auto result = CurrentContext::pop();
      VK_assert(result == *this);
    }
  };


  inline PrimaryContext Device::primaryContext() const noexcept {
    return PrimaryContext(*this);
  }




  class Module : public Handle<Module, CUmodule> {};
  class Function : public Handle<Function, CUfunction> {};
  class Event : public Handle<Event, CUevent> {};
  class Stream : public Handle<Stream, CUstream> {
  public:
    Stream(){
      cuStreamCreate(&handle_, 0);
    }

    ~Stream(){
      cuStreamDestroy(handle_);
    }
  };
  class Semaphore : public Handle<Semaphore, CUexternalSemaphore> {
  public:
    Semaphore();
    Semaphore(CUexternalSemaphore handle);
    ~Semaphore();

    virtual void doFillWaitParams(CUDA_EXTERNAL_SEMAPHORE_WAIT_PARAMS& waitParams) noexcept = 0;
    virtual void doFillSignalParams(CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS& signalParams) noexcept = 0;
  };
  class GraphNode : public Handle<GraphNode, CUgraphNode> {
  protected:
    explicit GraphNode(native_handle_type handle) noexcept : Handle<GraphNode, CUgraphNode>(handle){}
  public:
    GraphNode() = default;
    void remove() noexcept {
      CU_graphNodeRemove(handle_);
    }
    CUgraphNodeType getDynamicType() const noexcept {
      CUgraphNodeType nodeType;
      CU_graphNodeGetType(handle_, &nodeType);
      return nodeType;
    }
  };
  struct GraphEdge {
    GraphNode from;
    GraphNode to;


    GraphEdge* operator->() noexcept {
      return this;
    }

    friend bool operator==(GraphEdge A, GraphEdge B) noexcept {
      return A.from == B.from && A.to == B.to;
    }
  };

  void swap(GraphEdge& a, GraphEdge& b) noexcept {
    GraphEdge tmp = a;
    a = b;
    b = tmp;
  }

  namespace Detail{
    struct GraphEdgeRef{
      GraphNode& from;
      GraphNode& to;

      GraphEdgeRef() = delete;
      GraphEdgeRef(const GraphEdgeRef&) = delete;
      GraphEdgeRef(GraphEdgeRef&&) noexcept = delete;

      GraphEdgeRef(GraphNode* fromPtr, GraphNode* toPtr) noexcept : from(*fromPtr), to(*toPtr){}

      GraphEdgeRef& operator=(const GraphEdgeRef& other) {
        from = other.from;
        to   = other.to;
        return *this;
      }
      GraphEdgeRef& operator=(GraphEdgeRef&& other) noexcept {
        from = other.from;
        to   = other.to;
        return *this;
      }
      GraphEdgeRef& operator=(GraphEdge edge) noexcept {
        from = edge.from;
        to   = edge.to;
        return *this;
      }

      GraphEdgeRef* operator->() noexcept {
        return this;
      }

      friend bool operator==(const GraphEdgeRef& A, const GraphEdgeRef& B) noexcept {
        return A.from == B.from && A.to == B.to;
      }

      operator GraphEdge() const noexcept {
        return {
            .from = from,
            .to = to
        };
      }
    };
    void swap(GraphEdge& a, GraphEdgeRef& b) noexcept {
      GraphEdge tmp = a;
      a = b;
      b = tmp;
    }
    void swap(GraphEdgeRef& a, GraphEdge& b) noexcept {
      GraphEdge tmp = b;
      b = a;
      a = tmp;
    }
    void swap(GraphEdgeRef& a, GraphEdgeRef& b) noexcept {
      GraphEdge tmp = a;
      a = b;
      b = tmp;
    }

    class GraphEdgeIterator{

      friend class GraphEdgeConstIterator;

      GraphNode* from;
      size_t     edgeCount;

    public:
      GraphEdgeIterator(GraphNode* from, size_t edgeCount) noexcept : from(from), edgeCount(edgeCount){}

      using value_type = GraphEdge;
      using reference = GraphEdgeRef;
      using difference_type = ptrdiff_t;

      GraphEdgeRef operator*()  const noexcept {
        return { from, from + edgeCount };
      }
      GraphEdgeRef operator->() const noexcept {
        return **this;
      }
      GraphEdgeRef operator[](ptrdiff_t offset) const noexcept {
        return *(*this + offset);
      }

      GraphEdgeIterator& operator++() noexcept {
        ++from;
        return *this;
      }
      GraphEdgeIterator& operator--() noexcept {
        --from;
        return *this;
      }
      GraphEdgeIterator  operator++(int) noexcept {
        auto copy = *this;
        ++from;
        return copy;
      }
      GraphEdgeIterator  operator--(int) noexcept {
        auto copy = *this;
        --from;
        return copy;
      }

      GraphEdgeIterator& operator+=(ptrdiff_t offset) noexcept {
        from += offset;
        return *this;
      }
      GraphEdgeIterator& operator-=(ptrdiff_t offset) noexcept {
        from -= offset;
        return *this;
      }

      friend GraphEdgeIterator operator+(GraphEdgeIterator A, ptrdiff_t offset) noexcept {
        return { A.from + offset, A.edgeCount };
      }
      friend GraphEdgeIterator operator-(GraphEdgeIterator A, ptrdiff_t offset) noexcept {
        return { A.from - offset, A.edgeCount };
      }
      friend ptrdiff_t         operator-(GraphEdgeIterator A, GraphEdgeIterator B) noexcept {
        VK_assert(A.edgeCount == B.edgeCount);
        VK_assert(std::abs(A.from - B.from) < A.edgeCount);
      }

      friend bool operator==(GraphEdgeIterator A, GraphEdgeIterator B) noexcept {
        // Assume the sizes are the same. Comparing iterators from different arrays is UB.
        VK_assert(A.edgeCount == B.edgeCount);
        return A.from == B.from;
      }
      friend auto operator<=>(GraphEdgeIterator A, GraphEdgeIterator B) noexcept {
        VK_assert(A.edgeCount == B.edgeCount);
        return A.from <=> B.from;
      }
    };
    class GraphEdgeConstIterator{
      const GraphNode* from;
      size_t     edgeCount;

    public:
      GraphEdgeConstIterator(GraphEdgeIterator nonConstIterator) noexcept : from(nonConstIterator.from), edgeCount(nonConstIterator.edgeCount){}
      GraphEdgeConstIterator(const GraphNode* from, size_t edgeCount) noexcept : from(from), edgeCount(edgeCount){}

      using value_type = GraphEdge;
      using reference = GraphEdge;
      using difference_type = ptrdiff_t;


      GraphEdge operator*()  const noexcept {
        return { .from = *from, .to = from[edgeCount] };
      }
      GraphEdge operator[](ptrdiff_t offset) const noexcept {
        return *(*this + offset);
      }

      GraphEdgeConstIterator& operator++() noexcept {
        ++from;
        return *this;
      }
      GraphEdgeConstIterator& operator--() noexcept {
        --from;
        return *this;
      }
      GraphEdgeConstIterator  operator++(int) noexcept {
        auto copy = *this;
        ++from;
        return copy;
      }
      GraphEdgeConstIterator  operator--(int) noexcept {
        auto copy = *this;
        --from;
        return copy;
      }

      GraphEdgeConstIterator& operator+=(ptrdiff_t offset) noexcept {
        from += offset;
        return *this;
      }
      GraphEdgeConstIterator& operator-=(ptrdiff_t offset) noexcept {
        from -= offset;
        return *this;
      }

      friend GraphEdgeConstIterator operator+(GraphEdgeConstIterator A, ptrdiff_t offset) noexcept {
        return { A.from + offset, A.edgeCount };
      }
      friend GraphEdgeConstIterator operator-(GraphEdgeConstIterator A, ptrdiff_t offset) noexcept {
        return { A.from - offset, A.edgeCount };
      }
      friend ptrdiff_t         operator-(GraphEdgeConstIterator A, GraphEdgeConstIterator B) noexcept {
        VK_assert(A.edgeCount == B.edgeCount);
        VK_assert(std::abs(A.from - B.from) < A.edgeCount);
      }

      friend bool operator==(GraphEdgeConstIterator A, GraphEdgeConstIterator B) noexcept {
        // Assume the sizes are the same. Comparing iterators from different arrays is UB.
        VK_assert(A.edgeCount == B.edgeCount);
        return A.from == B.from;
      }
      friend auto operator<=>(GraphEdgeConstIterator A, GraphEdgeConstIterator B) noexcept {
        VK_assert(A.edgeCount == B.edgeCount);
        return A.from <=> B.from;
      }
    };
  }


  template <Allocator<GraphNode> Alloc = std::allocator<GraphNode>>
  class GraphEdgeList : public TempArray<GraphNode, Alloc>{
  protected:

    using Base_ = TempArray<GraphNode, Alloc>;

  public:



    using value_type = GraphEdge;
    using typename Base_::size_type;
    using typename Base_::difference_type;
    using iterator = Detail::GraphEdgeIterator;
    using const_iterator = Detail::GraphEdgeConstIterator;
    using sentinel = iterator;
    using const_sentinel = const_iterator;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_sentinel = reverse_iterator;
    using const_reverse_sentinel = const_reverse_iterator;

    using reference = typename iterator::reference;
    using const_reference = typename const_iterator::reference;


    using Base_::Base_;
    explicit GraphEdgeList(size_type length)                : Base_(length * 2){}
    GraphEdgeList(size_type length, const Alloc& allocator) : Base_(length * 2, allocator){}






    reference front() const noexcept {
      return *begin();
    }
    reference back()  const noexcept {
      return *--end();
    }

    reference operator[](size_type offset) const noexcept {
      VK_assert(offset < size());
      return begin()[offset];
    }


    size_type size() const noexcept {
      return Base_::size() >> 1;
    }


    iterator        begin() const noexcept {
      return { this->data(), size() };
    }
    const_iterator cbegin() const noexcept {
      return begin();
    }

    sentinel        end() const noexcept {
      return { this->data() + size(), size() };
    }
    const_sentinel cend() const noexcept {
      return end();
    }

    reverse_iterator        rbegin() const noexcept {
      return reverse_iterator(end());
    }
    const_reverse_iterator crbegin() const noexcept {
      return const_reverse_iterator(cend());
    }

    reverse_sentinel        rend() const noexcept {
      return reverse_sentinel(begin());
    }
    const_reverse_sentinel crend() const noexcept {
      return const_reverse_sentinel(cbegin());
    }

    operator std::pair<std::span<GraphNode>, std::span<GraphNode>>() const noexcept {
      const auto size = this->size();
      return {{ this->data(), size }, { this->data() + size, size }};
    }
    explicit operator std::span<GraphNode>() const noexcept{
      return static_cast<const Base_&>(*this);
    }
  };


  using PFN_hostCallback = void(VK_if(VK_compiler_msvc(__stdcall))*)(void* pUserData);



  class Graph;
  using GraphRef = Ref<Graph>;

  class ChildGraphNode : public GraphNode{

    friend class Graph;
    ChildGraphNode(CUgraphNode handle) noexcept : GraphNode(handle){}

  public:

    ChildGraphNode() = default;

    inline static bool isTypeOf(GraphNode node) noexcept {
      return node.getDynamicType() == CU_GRAPH_NODE_TYPE_GRAPH;
    }
  };
  class EmptyNode : public GraphNode{

    friend class Graph;
    EmptyNode(CUgraphNode handle) noexcept : GraphNode(handle){}

  public:

    EmptyNode() = default;


    inline static bool isTypeOf(GraphNode node) noexcept {
      return node.getDynamicType() == CU_GRAPH_NODE_TYPE_EMPTY;
    }
  };





  class EventNode : public GraphNode{
  public:

    using GraphNode::GraphNode;

    inline static bool isTypeOf(GraphNode node) noexcept {
      auto dynamicType = node.getDynamicType();
      return dynamicType == CU_GRAPH_NODE_TYPE_EVENT_RECORD || dynamicType == CU_GRAPH_NODE_TYPE_WAIT_EVENT;
    }
  };
  class EventRecordNode : public EventNode{

    friend class Graph;
    EventRecordNode(CUgraphNode handle) noexcept : EventNode(handle){}

  public:

    inline static bool isTypeOf(GraphNode node) noexcept {
      return node.getDynamicType() == CU_GRAPH_NODE_TYPE_EVENT_RECORD;
    }
  };
  class EventWaitNode : public EventNode{

    friend class Graph;
    EventWaitNode(CUgraphNode handle) noexcept : EventNode(handle){}

  public:

    inline static bool isTypeOf(GraphNode node) noexcept {
      return node.getDynamicType() == CU_GRAPH_NODE_TYPE_WAIT_EVENT;
    }
  };

  class ExternalSemaphoreNode : public GraphNode{
  public:

    using GraphNode::GraphNode;

    inline static bool isTypeOf(GraphNode node) noexcept {
      auto dynamicType = node.getDynamicType();
      return dynamicType == CU_GRAPH_NODE_TYPE_EXT_SEMAS_WAIT || dynamicType == CU_GRAPH_NODE_TYPE_EXT_SEMAS_SIGNAL;
    }
  };
  class ExternalSemaphoreSignalNode : public ExternalSemaphoreNode{

    friend class Graph;
    ExternalSemaphoreSignalNode(CUgraphNode handle) noexcept : ExternalSemaphoreNode(handle){}

  public:
    inline static bool isTypeOf(GraphNode node) noexcept {
      return node.getDynamicType() == CU_GRAPH_NODE_TYPE_EXT_SEMAS_SIGNAL;
    }
  };
  class ExternalSemaphoreWaitNode : public ExternalSemaphoreNode{

    friend class Graph;
    ExternalSemaphoreWaitNode(CUgraphNode handle) noexcept : ExternalSemaphoreNode(handle){}

  public:
    inline static bool isTypeOf(GraphNode node) noexcept {
      return node.getDynamicType() == CU_GRAPH_NODE_TYPE_EXT_SEMAS_WAIT;
    }
  };

  class MemcpyNode : public GraphNode{

  public:

    explicit MemcpyNode(native_handle_type handle) noexcept : GraphNode(handle){}


    inline static bool isTypeOf(GraphNode node) noexcept {
      return node.getDynamicType() == CU_GRAPH_NODE_TYPE_MEMCPY;
    }
  };
  class MemsetNode : public GraphNode{

  public:
    explicit MemsetNode(native_handle_type handle) noexcept : GraphNode(handle){}

    inline static bool isTypeOf(GraphNode node) noexcept {
      return node.getDynamicType() == CU_GRAPH_NODE_TYPE_MEMSET;
    }
  };


  class KernelNode : public GraphNode{

    friend class Graph;
    explicit KernelNode(CUgraphNode handle) noexcept : GraphNode(handle){}

  public:

    KernelNode() = default;

    Ref<Function> getKernel() const noexcept;
    void          setKernel(Ref<Function> func) const noexcept;

    inline static bool isTypeOf(GraphNode node) noexcept {
      return node.getDynamicType() == CU_GRAPH_NODE_TYPE_KERNEL;
    }
  };
  class HostNode   : public GraphNode{

    friend class Graph;

    HostNode(CUgraphNode handle) noexcept : GraphNode(handle){}

  public:
    HostNode() = default;


    inline static bool isTypeOf(GraphNode node) noexcept {
      return node.getDynamicType() == CU_GRAPH_NODE_TYPE_HOST;
    }
  };

  template <std::derived_from<GraphNode> Node>
  inline static Node node_cast(GraphNode node) noexcept {
    Node nodeResult;
    if (Node::isTypeOf(node))
      nodeResult.setNativeHandle(node.getNativeHandle());
    return nodeResult;
  }





#define CU_calloca(type, count) (type*)alloca((count) * sizeof(type))




  class ExecutableGraph : public Handle<ExecutableGraph, CUgraphExec>{
  public:
    ~ExecutableGraph() {
      if (handle_)
        CU_graphExecDestroy(handle_);
    }
  };

  class Graph : public Handle<Graph, CUgraph>{
  public:

    using size_type = size_t;
    using node_type = GraphNode;


    Graph() {
      CU_try_call(GraphCreate)(&handle_, 0);
    }
    using Handle<Graph, CUgraph>::Handle;
    Graph(const Graph&) = delete;
    Graph(Graph&& other) noexcept : Handle(other.getNativeHandle()){
      other.setNativeHandle(nullptr);
    }

    ~Graph() {
      if (handle_)
        CU_graphDestroy(handle_);
    }

    Graph                       clone() const {
      CUgraph clonedHandle;
      CU_try_call(GraphClone)(&clonedHandle, handle_);
      return Graph(clonedHandle);
    }

    size_type                   size() const noexcept {
      size_type nodeCount;
      CU_graphGetNodes(handle_, nullptr, &nodeCount);
      return nodeCount;
    }

    template <std::derived_from<GraphNode> Node>
    Node                        findClonedNode(Node originalNode) const noexcept {
      Node clonedNode{originalNode};
      CUgraphNode resultNode;
      CU_graphFindInClone(&resultNode,originalNode.getNativeHandle(), handle_);
      clonedNode.setNativeHandle(resultNode);
      return clonedNode;
    }


    size_type                   nodeCount() const noexcept {
      return this->size();
    }
    template <Spanish<GraphNode> S>
    std::span<GraphNode>        nodes(S&& nodeHandles) const noexcept {
      std::span nodeSpan{std::forward<S>(nodeHandles)};
      size_t nodeCount = nodeSpan.size();
      CU_graphGetNodes(handle_, (CUgraphNode*)nodeSpan.data(), &nodeCount);
      return nodeSpan.first(nodeCount);
    }
    template <Allocator<GraphNode> Alloc = std::allocator<GraphNode>>
    TempArray<GraphNode, Alloc> nodes(const Alloc& allocator = {}) const noexcept {
      size_t nodeCount;
      CU_graphGetNodes(handle_, nullptr, &nodeCount);
      if (nodeCount != 0) {
        TempArray<GraphNode, Alloc> nodeArray(nodeCount, allocator);
        CU_graphGetNodes(handle_, (CUgraphNode*)nodeArray.data(), &nodeCount);
        return std::move(nodeArray);
      }
      return {};
    }



    size_type                   rootNodeCount() const noexcept {
      size_type rootNodeCount_;
      CU_graphGetRootNodes(handle_, nullptr, &rootNodeCount_);
      return rootNodeCount_;
    }
    template <Spanish<GraphNode> S>
    std::span<GraphNode>        rootNodes(S&& nodeHandles) const noexcept {
      std::span nodeSpan{std::forward<S>(nodeHandles)};
      size_t nodeCount = nodeSpan.size();
      CU_graphGetRootNodes(handle_, (CUgraphNode*)nodeSpan.data(), &nodeCount);
      return nodeSpan.first(nodeCount);
    }
    template <Allocator<GraphNode> Alloc = std::allocator<GraphNode>>
    TempArray<GraphNode, Alloc> rootNodes(const Alloc& allocator = {}) const noexcept {
      size_t nodeCount;
      CU_graphGetRootNodes(handle_, nullptr, &nodeCount);
      if (nodeCount != 0) {
        TempArray<GraphNode, Alloc> nodeArray(nodeCount, allocator);
        CU_graphGetRootNodes(handle_, (CUgraphNode*)nodeArray.data(), &nodeCount);
        return std::move(nodeArray);
      }
      return {};
    }


    size_type edgeCount() const noexcept {
      size_type edgeCount_;
      CU_graphGetEdges(handle_, nullptr, nullptr, &edgeCount_);
      return edgeCount_;
    }
    template <Spanish<GraphEdge> S>
    std::span<GraphEdge> edges(S&& nodeHandles) const noexcept {
      std::span nodeSpan{std::forward<S>(nodeHandles)};
      size_t nodeCount = nodeSpan.size();
      CU_graphGetEdges(handle_, (CUgraphNode*)nodeSpan.data(), ((CUgraphNode*)nodeSpan.data()) + nodeCount, &nodeCount);
      rearrangeEdges({ &nodeSpan[0].from, nodeSpan.size() * 2 });
      return nodeSpan.first(nodeCount);
    }
    template <Allocator Alloc = std::allocator<GraphNode>>
    GraphEdgeList<typename std::allocator_traits<Alloc>::template rebind_alloc<GraphNode>> edges(const Alloc& allocator = {}) const noexcept {
      using alloc_type = typename std::allocator_traits<Alloc>::template rebind_alloc<GraphNode>;
      using ret_type = GraphEdgeList<alloc_type>;

      size_t edgeCount;
      CU_graphGetEdges(handle_, nullptr, nullptr, &edgeCount);
      if (edgeCount != 0) {
        ret_type edgeArray(edgeCount, static_cast<alloc_type>(allocator));
        CU_graphGetEdges(handle_, (CUgraphNode*)edgeArray.data(), ((CUgraphNode*)edgeArray.data()) + edgeCount, &edgeCount);
        return std::move(edgeArray);
      }
      return {};
    }


    /*template <typename T> requires(sizeof(T) <= 4 && std::is_trivially_copyable_v<T>)
    MemsetNode addMemsetNode(void* pArray, T value, size_t width, std::span<const GraphNode> dependencies = {}) {
      return this->addMemsetNode(pArray, value, width, CurrentContext::get(), dependencies);
    }
    template <typename T> requires(sizeof(T) <= 4 && std::is_trivially_copyable_v<T>)
    MemsetNode addMemsetNode(void* pArray, T value, size_t width, const ActiveContext context, std::span<GraphNode> dependencies = {}) {
      CUDA_MEMSET_NODE_PARAMS memsetNodeParams{
          .dst         = (CUdeviceptr)pArray,
          .pitch       = 0,
          .value       = (uint32_t)value,
          .elementSize = (uint32_t)sizeof(T),
          .width       = width,
          .height      = 1
      };

      CUgraphNode result;
      CU_try_call(GraphAddMemsetNode)(&result, handle_, (const CUgraphNode*)dependencies.data(), dependencies.size(), &memsetNodeParams, context.getNativeHandle());
      return MemsetNode(result);
    }

    template <typename T, typename I> requires(sizeof(T) <= 4 && std::is_trivially_copyable_v<T>)
    MemsetNode addMemsetNode(const Array2D<T>& array, std::type_identity_t<T> value, const Box2D<I>& box, std::span<GraphNode> dependencies = {}) noexcept {
      return this->addMemsetNode(array, value, box, CurrentContext::get(), dependencies);
    }
    template <typename T, typename I> requires(sizeof(T) <= 4 && std::is_trivially_copyable_v<T>)
    MemsetNode addMemsetNode(const Array2D<T>& array, std::type_identity_t<T> value, const Box2D<I>& box, ActiveContext context, std::span<GraphNode> dependencies = {}) noexcept{
      VK_assert(box.extents.width  + box.offsets.x <= array.width);
      VK_assert(box.extents.height + box.offsets.y <= array.height);

      CUDA_MEMSET_NODE_PARAMS memsetNodeParams{
          .dst         = ((CUdeviceptr)array.address) + (box.offsets.y * array.pitch) + box.offsets.x,
          .pitch       = array.pitch,
          .value       = (uint32_t)value,
          .elementSize = (uint32_t)sizeof(T),
          .width       = array.width,
          .height      = array.height
      };

      CUgraphNode result;
      CU_try_call(GraphAddMemsetNode)(&result, handle_, (const CUgraphNode*)dependencies.data(), dependencies.size(), &memsetNodeParams, context.getNativeHandle());
      return MemsetNode(result);
    }
*/

    MemcpyNode     addMemcpyNode(const Resource&            dst,
                                 const Position3D<size_t>&  dstOffset,
                                 const Resource&            src,
                                 const Position3D<size_t>&  srcOffset,
                                 const Extent3D<size_t>&    cpyRangeInBytes,
                                 Ref<Context>               ctx,
                                 std::span<const GraphNode> dependencies = {}) {
      CUDA_MEMCPY3D memcpyParams{
        .WidthInBytes = cpyRangeInBytes.width,
        .Height       = cpyRangeInBytes.height,
        .Depth        = cpyRangeInBytes.depth
      };
      dst.doFillMemcpyParams(ResourceOpKind::Destination, memcpyParams, dstOffset, cpyRangeInBytes);
      src.doFillMemcpyParams(ResourceOpKind::Source, memcpyParams, srcOffset, cpyRangeInBytes);

      CUgraphNode graphNode;
      auto addResult = cuGraphAddMemcpyNode(&graphNode, handle_, (const CUgraphNode*)dependencies.data(), dependencies.size(), &memcpyParams, ctx.getNativeHandle());
      if (addResult != CUDA_SUCCESS)
        throw cuda_driver_error(addResult);
      return MemcpyNode(graphNode);
    }
    MemsetNode     addMemsetNode(const Resource&            dst,
                                 const Position3D<size_t>&  dstOffset,
                                 uint8_t                    value,
                                 const Extent3D<size_t>&    cpyRangeInBytes,
                                 Ref<Context>               ctx,
                                 std::span<const GraphNode> dependencies = {}) {
      CUDA_MEMSET_NODE_PARAMS memsetParams{
        .value       = value,
        .elementSize = 1,
      };

      Box3D<size_t> box{
        .offsets = dstOffset,
        .extents = cpyRangeInBytes
      };

      dst.doFillMemsetParams(memsetParams, box);

      CUgraphNode graphNode;
      CU_try_call(GraphAddMemsetNode)(&graphNode, handle_, (const CUgraphNode*)dependencies.data(), dependencies.size(), &memsetParams, ctx.getNativeHandle());
      return MemsetNode(graphNode);
    }



    ChildGraphNode addChildGraphNode(const Graph& graphRef, std::span<const GraphNode> dependencies = {}) {
      CUgraphNode result;
      CU_try_call(GraphAddChildGraphNode)(&result, handle_, (const CUgraphNode*)dependencies.data(), dependencies.size(), graphRef.getNativeHandle());
      return ChildGraphNode(result);
    }
    EmptyNode      addEmptyNode(std::span<const GraphNode> dependencies = {}) noexcept {
      CUgraphNode result;
      CU_try_call(GraphAddEmptyNode)(&result, handle_, (const CUgraphNode*)dependencies.data(), dependencies.size());
      return EmptyNode(result);
    }
    HostNode       addHostNode(PFN_hostCallback pfnCallback, void* pUserData, std::span<const GraphNode> dependencies = {}) noexcept {

      CUDA_HOST_NODE_PARAMS nodeParams{
        .fn = pfnCallback,
        .userData = pUserData
      };

      CUgraphNode result;
      CU_try_call(GraphAddHostNode)(&result, handle_, (const CUgraphNode*)dependencies.data(), dependencies.size(), &nodeParams);
      return HostNode(result);
    }
    KernelNode     addKernelNode(Ref<Function> function,
                                 Extent3D<uint32_t> blockDims,
                                 Extent3D<uint32_t> gridDims,
                                 uint32_t sharedMemSize,
                                 void** ppParams,
                                 std::span<const GraphNode> dependencies = {}){
      CUDA_KERNEL_NODE_PARAMS nodeParams{
        .func = function.getNativeHandle(),
        .gridDimX = gridDims.width,
        .gridDimY = gridDims.height,
        .gridDimZ = gridDims.depth,
        .blockDimX = blockDims.width,
        .blockDimY = blockDims.height,
        .blockDimZ = blockDims.depth,
        .sharedMemBytes = sharedMemSize,
        .kernelParams = ppParams
      };

      CUgraphNode result;
      CU_try_call(GraphAddKernelNode)(&result, handle_, (const CUgraphNode*)dependencies.data(), dependencies.size(), &nodeParams);
      return KernelNode(result);
    }


    ExternalSemaphoreSignalNode addExternalSemaphoreSignalNode(std::span<Ref<Semaphore>> semaphore, std::span<const GraphNode> dependencies = {}) {

      static_assert(sizeof(CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS) == 144);

      auto* semaphoreHandles = CU_calloca(CUexternalSemaphore,                   semaphore.size());
      auto* signalParams =     CU_calloca(CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS, semaphore.size());

      for (auto i = 0; i < semaphore.size(); ++i) {
        semaphoreHandles[i] = semaphore[i].getNativeHandle();
        semaphore[i].doFillSignalParams(signalParams[i]);
      }

      CUexternalSemaphore* extSemArray;
      const CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS* paramsArray;
      unsigned int numExtSems;

      CUDA_EXT_SEM_SIGNAL_NODE_PARAMS nodeParams{
        .extSemArray = semaphoreHandles,
        .paramsArray = signalParams,
        .numExtSems  = (uint32_t)semaphore.size()
      };

      CUgraphNode result;
      CU_try_call(GraphAddExternalSemaphoresSignalNode)(&result, handle_, (const CUgraphNode*)dependencies.data(), dependencies.size(), &nodeParams);
      return ExternalSemaphoreSignalNode(result);
    }
    ExternalSemaphoreWaitNode   addExternalSemaphoreWaitNode(std::span<Ref<Semaphore>> semaphore, std::span<const GraphNode> dependencies = {})  {
      static_assert(sizeof(CUDA_EXTERNAL_SEMAPHORE_SIGNAL_PARAMS) == 144);

      auto* semaphoreHandles = CU_calloca(CUexternalSemaphore,                   semaphore.size());
      auto* waitParams       = CU_calloca(CUDA_EXTERNAL_SEMAPHORE_WAIT_PARAMS, semaphore.size());

      for (auto i = 0; i < semaphore.size(); ++i) {
        semaphoreHandles[i] = semaphore[i].getNativeHandle();
        semaphore[i].doFillWaitParams(waitParams[i]);
      }

      CUDA_EXT_SEM_WAIT_NODE_PARAMS nodeParams{
          .extSemArray = semaphoreHandles,
          .paramsArray = waitParams,
          .numExtSems  = (uint32_t)semaphore.size()
      };

      CUgraphNode result;
      CU_try_call(GraphAddExternalSemaphoresWaitNode)(&result, handle_, (const CUgraphNode*)dependencies.data(), dependencies.size(), &nodeParams);
      return ExternalSemaphoreWaitNode(result);
    }

    EventRecordNode addEventRecordNode(Ref<Event> event, std::span<GraphNode> dependencies = {}) {
      CUgraphNode result;
      CU_try_call(GraphAddEventRecordNode)(&result, handle_, (const CUgraphNode*)dependencies.data(), dependencies.size(), event.getNativeHandle());
      return EventRecordNode(result);
    }
    EventWaitNode   addEventWaitNode(Ref<Event> event, std::span<GraphNode> dependencies = {}) {
      CUgraphNode result;
      CU_try_call(GraphAddEventWaitNode)(&result, handle_, (const CUgraphNode*)dependencies.data(), dependencies.size(), event.getNativeHandle());
      return EventWaitNode(result);
    }






  private:

    static void rearrangeEdges(std::span<GraphNode> edges) noexcept {

      const auto edgeCount = edges.size();
      const auto n = edgeCount >> 1;

      auto* tmpArray = new GraphNode[edgeCount];
      std::memcpy(tmpArray, edges.data(), edgeCount);

      auto* const edgeData = edges.data();


      for (size_t i = 0; i < edgeCount; ++i) {
        edgeData[2 * i] = tmpArray[i];
        edgeData[(2 * i) + 1] = tmpArray[i + n];
      }

      delete[] tmpArray;
    }
  };


  class DeviceAllocation : public Handle<DeviceAllocation, CUmemGenericAllocationHandle>{

  };

  







  /*void getGraphNodes(){
    Graph graph{};
    GraphNode nodeStorage[16];
    std::pmr::monotonic_buffer_resource monotonicBuffer{1024};
    std::pmr::polymorphic_allocator<GraphNode> nodeAllocator{&monotonicBuffer};
    auto nodes = graph.nodes();
    auto autoNodes = graph.nodes(nodeStorage);
    auto allocatedNodes = graph.nodes(nodeAllocator);

  }*/
}
//__global__ void kernel(){}






class CudaRandomGenerator{
  CUmemGenericAllocationHandle allocationHandle;
  CUdeviceptr memoryBuffer;
  CUmodule    kernelModule;
  CUfunction  kernelFunction;
  CUresult    lastResult;
  size_t      bufferLength;


  std::atomic_uint32_t readOffset;
  std::atomic_uint32_t writeOffset;


  explicit CudaRandomGenerator(size_t bufferSize)
      : allocationHandle(),
        memoryBuffer(),
        kernelModule(),
        kernelFunction(),
        bufferLength(bufferSize){


    SECURITY_ATTRIBUTES securityAttributes{
        .nLength = sizeof(SECURITY_ATTRIBUTES),
        .lpSecurityDescriptor = nullptr,
        .bInheritHandle = true
    };



    CUmemAllocationProp allocationProperties{
        .type = CU_MEM_ALLOCATION_TYPE_PINNED,
        .requestedHandleTypes = CU_MEM_HANDLE_TYPE_WIN32,
        .location = {
            .type = CU_MEM_LOCATION_TYPE_DEVICE,
            .id   = 0
        },
        .win32HandleMetaData = &securityAttributes
    };

    CUmemAccessDesc accessDesc{
        .location = allocationProperties.location,
        .flags    = CU_MEM_ACCESS_FLAGS_PROT_READWRITE
    };

    cuMemCreate(&allocationHandle, bufferSize, &allocationProperties, 0);

    size_t alignment;
    cuMemGetAllocationGranularity(&alignment, &allocationProperties, CU_MEM_ALLOC_GRANULARITY_RECOMMENDED);
    cuMemAddressReserve(&memoryBuffer, bufferSize * 2, alignment, 0, 0);
    cuMemMap(memoryBuffer, bufferSize, 0, allocationHandle, 0);
    cuMemMap(memoryBuffer + bufferSize, bufferSize, 0, allocationHandle, 0);
    cuMemSetAccess(memoryBuffer, bufferSize * 2, &accessDesc, 1);
  }

  ~CudaRandomGenerator(){
    cuMemUnmap(memoryBuffer, bufferLength * 2);
    cuMemAddressFree(memoryBuffer, bufferLength * 2);
    cuMemRelease(allocationHandle);
  }



};


int main() {
  cuInit(0);

  cu::Device device{};
  cu::PrivateContext ctx{device};
  cu::ActiveContext activeContext{ctx};
  cu::Graph graph{};
}