//
// Created by Maxwell on 2021-01-18.
//

#ifndef VALKYRIE_CORE_ADT_RANGE_REF_HPP
#define VALKYRIE_CORE_ADT_RANGE_REF_HPP


#include <type_traits>

namespace valkyrie::Core{


  template <typename T>
  class InputIteratorRef{};
  template <typename T>
  class ForwardIteratorRef : public InputIteratorRef<T>{};
  template <typename T>
  class BidirectionalIteratorRef : public ForwardIteratorRef<T>{};
  template <typename T>
  class RandomAccessIteratorRef : public BidirectionalIteratorRef<T>{};
  template <typename T>
  class ContiguousIteratorRef : public RandomAccessIteratorRef<T>{};



  namespace Detail{
    template <typename T>
    class InputIteratorRefInterface{};
    template <typename T>
    class ForwardIteratorRefInterface : public InputIteratorRefInterface<T>{};
    template <typename T>
    class BidirectionalIteratorRefInterface : public ForwardIteratorRefInterface<T>{};
    template <typename T>
    class RandomAccessIteratorRefInterface : public BidirectionalIteratorRefInterface<T>{};
    template <typename T>
    class ContiguousIteratorRefInterface : public RandomAccessIteratorRefInterface<T>{};

    template <typename T>
    using erased_range_ref = std::conditional_t<std::is_const_v<T>, const void*, void*>;



    template <typename T>
    class InputRangeRefInterface{};
    template <typename T>
    class ForwardRangeRefInterface           : public virtual InputRangeRefInterface<T>{};
    template <typename T>
    class BidirectionalRangeRefInterface     : public virtual ForwardRangeRefInterface<T>{};
    template <typename T>
    class RandomAccessRangeRefInterface      : public virtual BidirectionalRangeRefInterface<T>{};
    template <typename T>
    class ContiguousRangeRefInterface        : public virtual RandomAccessRangeRefInterface<T>{};


    template <typename T, typename Rng>
    class InputRangeRefInterfaceImpl         : public virtual InputRangeRefInterface<T>{};
    template <typename T, typename Rng>
    class ForwardRangeRefInterfaceImpl       : public virtual ForwardRangeRefInterface<T>,       public InputRangeRefInterfaceImpl<T, Rng>{};
    template <typename T, typename Rng>
    class BidirectionalRangeRefInterfaceImpl : public virtual BidirectionalRangeRefInterface<T>, public ForwardRangeRefInterfaceImpl<T, Rng>{};
    template <typename T, typename Rng>
    class RandomAccessRangeRefInterfaceImpl  : public virtual RandomAccessRangeRefInterface<T>,  public BidirectionalRangeRefInterfaceImpl<T, Rng>{};
    template <typename T, typename Rng>
    class ContiguousRangeRefInterfaceImpl    : public ContiguousRangeRefInterface<T>,            public RandomAccessRangeRefInterfaceImpl<T, Rng>{};
  }
  
  
  
  
  
  
  template <typename T>
  class InputRangeRef{
  public:
    using value_type = T;
  };
  template <typename T>
  class ForwardRangeRef{
  public:
    using value_type = T;
  };
  template <typename T>
  class BidirectionalRangeRef{
  public:
    using value_type = T;
  };
  template <typename T>
  class RandomAccessRangeRef{
  public:
    using value_type = T;
  };
  template <typename T>
  class ContiguousRangeRef{
  public:
    using value_type = T;


  private:
    using ref_t = Detail::erased_range_ref<T>;
    ref_t m_ref;
    const Detail::ContiguousRangeRefInterface<T>* m_pInterface;
  };



}

#endif //VALKYRIE_CORE_ADT_RANGE_REF_HPP
