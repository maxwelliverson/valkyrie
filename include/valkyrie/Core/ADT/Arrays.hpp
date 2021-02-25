//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_CORE_ADT_ARRAYS_HPP
#define VALKYRIE_CORE_ADT_ARRAYS_HPP

#include <valkyrie/Core/ADT/ArrayRef.hpp>
#include <valkyrie/Core/Utility/Iterator.hpp>
#include <valkyrie/Core/Utility/Interval.hpp>
#include <valkyrie/Math/Calculus/Polynomial.hpp>


#include <memory>
#include <vector>
#include <array>
#include <algorithm>


namespace valkyrie::Core{


  namespace Arrays{

    template <typename T>
    struct Stride;

    template <typename T, auto ...Val>
    class BasicIndex{


    private:
      T value_;
    };


    template <typename IndexType, /*Core::Detail::ExtentLike<IndexType>*/typename Ext>
    inline constexpr static auto normalizeExtent(Ext extent) noexcept {
      if constexpr (std::same_as<Ext, DynamicExtentType>)
        return extent;
      else
        return static_cast<IndexType>(extent);
    }

    namespace Detail{

      template <typename T, auto...>
      using discard_value_t = T;

      inline constexpr static enum class CopyConstructTag{} copyConstruct{};


      template <typename T, auto ...Val>
      class IndexStorage{

      };













      template <typename IndexType, /*Core::Detail::StrictExtent<IndexType> */auto ...Ext>
      struct ExtentStorage;
      template <typename IndexType>
      struct ExtentStorage<IndexType>{
        constexpr ExtentStorage() noexcept = default;
        constexpr ExtentStorage(CopyConstructTag, const ExtentStorage&) noexcept{}
      };
      template <typename IndexType, IndexType Head, /*Core::Detail::StrictExtent<IndexType>*/ auto ...Tail>
      struct ExtentStorage<IndexType, Head, Tail...> : public ExtentStorage<IndexType, Tail...>{

        constexpr ExtentStorage() noexcept = default;
        constexpr ExtentStorage(const ExtentStorage&) noexcept = default;
        constexpr ExtentStorage(ExtentStorage&&) noexcept = default;



        template </*Core::Detail::StrictExtent<IndexType>*/ auto ...OtherTail>
        inline constexpr explicit ExtentStorage(CopyConstructTag, const ExtentStorage<IndexType, Head, OtherTail...>& Other) noexcept
            //requires(ConstructibleFrom<ExtentStorage<IndexType, Tail...>, CopyConstructTag, const ExtentStorage<IndexType, OtherTail...>&>)
            : ExtentStorage<IndexType, Tail...>(copyConstruct, Other.base()){}
        template </*Core::Detail::StrictExtent<IndexType>*/ auto ...OtherTail>
        inline constexpr explicit ExtentStorage(CopyConstructTag, const ExtentStorage<IndexType, dynamicExtent, OtherTail...>& Other) noexcept
            //requires(ConstructibleFrom<ExtentStorage<IndexType, Tail...>, CopyConstructTag, const ExtentStorage<IndexType, OtherTail...>&>)
            : ExtentStorage<IndexType, Tail...>(copyConstruct, Other.base()){
          VK_constexpr_assert(Other.extent() == Head);
        }

        template <std::convertible_to<IndexType> ...Index>
        inline constexpr ExtentStorage(Index&& ...values_) noexcept
            : ExtentStorage<IndexType, Tail...>(std::forward<Index>(values_)...){}

        inline constexpr param_t<IndexType> extent() const noexcept {
          return Head;
        }
        inline constexpr const ExtentStorage<IndexType, Tail...>& base() const noexcept {
          return *this;
        }
      };
      template <typename IndexType, /*Core::Detail::StrictExtent<IndexType>*/ auto ...Tail>
      struct ExtentStorage<IndexType, dynamicExtent, Tail...> : public ExtentStorage<IndexType, Tail...>{

        constexpr ExtentStorage() noexcept = default;
        constexpr ExtentStorage(const ExtentStorage&) noexcept = default;
        constexpr ExtentStorage(ExtentStorage&&) noexcept = default;

        template </*Core::Detail::StrictExtent<IndexType>*/ auto OtherHead, /*Core::Detail::StrictExtent<IndexType>*/ auto ...OtherTail>
        inline constexpr ExtentStorage(CopyConstructTag, const ExtentStorage<IndexType, OtherHead, OtherTail...>& Other) noexcept
            requires(ConstructibleFrom<ExtentStorage<IndexType, Tail...>, const ExtentStorage<IndexType, OtherTail...>&>)
            : ExtentStorage<IndexType, Tail...>(copyConstruct, Other.base()),
              thisIndex_(Other.extent()){}
        template <std::convertible_to<IndexType> I, std::convertible_to<IndexType> ...Index>
        inline constexpr ExtentStorage(I&& thisValue_, Index&& ...values_) noexcept
            : ExtentStorage<IndexType, Tail...>(std::forward<Index>(values_)...), thisIndex_(std::forward<I>(thisValue_)){}

        inline constexpr param_t<IndexType> extent() const noexcept {
          return thisIndex_;
        }
        inline constexpr const ExtentStorage<IndexType, Tail...>& base() const noexcept {
          return *this;
        }
        IndexType thisIndex_ = 0;
      };


      template <typename IndexType>
      struct ExtentStorage<Stride<IndexType>>{
        constexpr ExtentStorage() noexcept = default;
        constexpr ExtentStorage(CopyConstructTag, const ExtentStorage&) noexcept{}
      };
      template <typename IndexType, IndexType Head, /*Core::Detail::StrictExtent<IndexType>*/ auto ...Tail>
      struct ExtentStorage<Stride<IndexType>, Head, Tail...> : public ExtentStorage<Stride<IndexType>, Tail...>{

        constexpr ExtentStorage() noexcept = default;
        constexpr ExtentStorage(const ExtentStorage&) noexcept = default;
        constexpr ExtentStorage(ExtentStorage&&) noexcept = default;



        template </*Core::Detail::StrictExtent<IndexType>*/ auto ...OtherTail>
        inline constexpr explicit ExtentStorage(CopyConstructTag, const ExtentStorage<Stride<IndexType>, Head, OtherTail...>& Other) noexcept
        //requires(ConstructibleFrom<ExtentStorage<IndexType, Tail...>, CopyConstructTag, const ExtentStorage<IndexType, OtherTail...>&>)
            : ExtentStorage<Stride<IndexType>, Tail...>(copyConstruct, Other.base()){}
        template </*Core::Detail::StrictExtent<IndexType>*/ auto ...OtherTail>
        inline constexpr explicit ExtentStorage(CopyConstructTag, const ExtentStorage<Stride<IndexType>, dynamicExtent, OtherTail...>& Other) noexcept
        //requires(ConstructibleFrom<ExtentStorage<IndexType, Tail...>, CopyConstructTag, const ExtentStorage<IndexType, OtherTail...>&>)
            : ExtentStorage<Stride<IndexType>, Tail...>(copyConstruct, Other.base()){
          VK_constexpr_assert(Other.extent() == Head);
        }

        template <std::convertible_to<IndexType> ...Index>
        inline constexpr ExtentStorage(Index&& ...values_) noexcept
            : ExtentStorage<Stride<IndexType>, Tail...>(std::forward<Index>(values_)...){}

        inline constexpr param_t<IndexType> extent() const noexcept {
          return Head;
        }
        inline constexpr const ExtentStorage<Stride<IndexType>, Tail...>& base() const noexcept {
          return *this;
        }
      };
      template <typename IndexType, /*Core::Detail::StrictExtent<IndexType>*/ auto ...Tail>
      struct ExtentStorage<Stride<IndexType>, dynamicExtent, Tail...> : public ExtentStorage<Stride<IndexType>, Tail...>{

        constexpr ExtentStorage() noexcept = default;
        constexpr ExtentStorage(const ExtentStorage&) noexcept = default;
        constexpr ExtentStorage(ExtentStorage&&) noexcept = default;

        template </*Core::Detail::StrictExtent<IndexType>*/ auto OtherHead, /*Core::Detail::StrictExtent<IndexType>*/ auto ...OtherTail>
        inline constexpr ExtentStorage(CopyConstructTag, const ExtentStorage<Stride<IndexType>, OtherHead, OtherTail...>& Other) noexcept
        requires(ConstructibleFrom<ExtentStorage<Stride<IndexType>, Tail...>, const ExtentStorage<Stride<IndexType>, OtherTail...>&>)
            : ExtentStorage<Stride<IndexType>, Tail...>(copyConstruct, Other.base()),
              thisIndex_(Other.extent()){}
        template <std::convertible_to<IndexType> I, std::convertible_to<IndexType> ...Index>
        inline constexpr ExtentStorage(I&& thisValue_, Index&& ...values_) noexcept
            : ExtentStorage<Stride<IndexType>, Tail...>(std::forward<Index>(values_)...), thisIndex_(std::forward<I>(thisValue_)){}

        inline constexpr param_t<IndexType> extent() const noexcept {
          return thisIndex_;
        }
        inline constexpr const ExtentStorage<Stride<IndexType>, Tail...>& base() const noexcept {
          return *this;
        }
        IndexType thisIndex_ = 0;
      };




      template <size_t pos, typename IndexType, /*Core::Detail::StrictExtent<IndexType>*/ auto Head, /*Core::Detail::StrictExtent<IndexType>*/ auto ...Tail>
      inline constexpr decltype(auto) index(const ExtentStorage<IndexType, Head, Tail...>& Storage_) noexcept {
        if constexpr (!pos)
          return Storage_.extent();
        else
          return index<pos - 1>(Storage_.base());
      }

      template <typename IndexType>
      inline constexpr IndexType index(size_t pos, const ExtentStorage<IndexType>& Storage_) noexcept{
        VK_constexpr_assert(pos == 0xBAD'ACCE55);
        VK_unreachable;
        //return {};
      }
      template <typename IndexType, /*Core::Detail::StrictExtent<IndexType>*/ auto Head, /*Core::Detail::StrictExtent<IndexType>*/ auto ...Tail>
      inline constexpr decltype(auto) index(size_t pos, const ExtentStorage<IndexType, Head, Tail...>& Storage_) noexcept {
        if (!pos) return Storage_.extent();
        return index(pos - 1, Storage_.base());
      }

      template </*Core::Detail::StrictExtent*/ auto Head, /*Core::Detail::StrictExtent*/ auto ...Tail>
      inline constexpr bool isStaticExtent(size_t pos) noexcept {
        if (!pos) return !std::same_as<decltype(Head), DynamicExtentType>;
        if constexpr (sizeof...(Tail) > 0)
          return isStaticExtent<Tail...>(pos - 1);
        else {
          VK_constexpr_assert(pos == 0xBAD'ACCE55);
          VK_unreachable;
          return false;
        }
      }


      template <typename IndexType>
      inline constexpr bool isEqual(const ExtentStorage<IndexType>& A, const ExtentStorage<IndexType>& B) noexcept{
        return true;
      }

      template <typename IndexType, /*Core::Detail::StrictExtent<IndexType>*/ auto ...ExtA, /*Core::Detail::StrictExtent<IndexType>*/ auto ...ExtB>
      requires(sizeof...(ExtA) == sizeof...(ExtB))
      inline constexpr bool isEqual(const ExtentStorage<IndexType, ExtA...>& A, const ExtentStorage<IndexType, ExtB...>& B) noexcept {
        return A.extent() == B.extent() && isEqual(A.base(), B.base());
      }
      template <typename IndexType, /*Core::Detail::StrictExtent<IndexType>*/ auto ...ExtA, /*Core::Detail::StrictExtent<IndexType>*/ auto ...ExtB>
      inline constexpr bool isEqual(const ExtentStorage<IndexType, ExtA...>& A, const ExtentStorage<IndexType, ExtB...>& B) noexcept{
        return false;
      }
    }

    template <typename IndexType, auto ...Ext>
    class BasicExtents;

    template <typename IndexType, Core::Detail::ExtentLike<IndexType> auto ...Ext>
    class BasicExtents<IndexType, Ext...> : Detail::ExtentStorage<IndexType, normalizeExtent<IndexType>(Ext)...>{
      using Storage_ = Detail::ExtentStorage<IndexType, normalizeExtent<IndexType>(Ext)...>;
      using param_type = param_t<IndexType>;
    public:
      using index_type = IndexType;

      constexpr BasicExtents() noexcept = default;
      constexpr BasicExtents(const BasicExtents&) noexcept = default;
      constexpr BasicExtents(BasicExtents&&) noexcept = default;

      constexpr BasicExtents& operator=(const BasicExtents&) noexcept = default;
      constexpr BasicExtents& operator=(BasicExtents&&) noexcept = default;


      VK_nodiscard static constexpr size_t rank() noexcept { return sizeof...(Ext); }
      VK_nodiscard static constexpr size_t rankDynamic() noexcept { return (size_t(std::same_as<decltype(Ext), DynamicExtentType>) + ...); }
      VK_nodiscard static constexpr bool isStaticExtent(size_t pos) noexcept {
        return Detail::isStaticExtent<Ext...>(pos);
      }
      VK_nodiscard static constexpr index_type totalStaticExtent() noexcept requires(rankDynamic() == 0){
        return (static_cast<index_type>(Ext) * ...);
      }
      template <size_t pos> requires(pos < rank())
      VK_nodiscard inline constexpr param_type extent() const noexcept {
        return Detail::index<pos>(*this);
      }
      VK_nodiscard inline constexpr param_type extent(size_t pos) const noexcept {
        VK_constexpr_assert(pos < rank());
        return Detail::index(pos, *this);
      }

      template</*Core::Detail::ExtentLike<IndexType>*/ auto ...OtherExtents>
      constexpr BasicExtents(const BasicExtents<IndexType, OtherExtents...>& Other) noexcept
      requires(!std::same_as<BasicExtents, BasicExtents<IndexType, OtherExtents...>> &&
          ConstructibleFrom<Storage_, const typename BasicExtents<IndexType, OtherExtents...>::Storage_&>)
          : Storage_{Detail::copyConstruct, Other}{}

      template<std::convertible_to<index_type>... IndexType_>
      constexpr BasicExtents(IndexType_&&... indices) noexcept requires(sizeof...(IndexType_) == rankDynamic())
          : Storage_(std::forward<IndexType_>(indices)...){}

      constexpr explicit BasicExtents(Span<index_type, rankDynamic()> indices) noexcept
          : BasicExtents(indices, std::make_index_sequence<rankDynamic()>{}){}

      template</*Core::Detail::ExtentLike<IndexType>*/ auto ...OtherExtents>
      requires(ConstructibleFrom<BasicExtents, const BasicExtents<IndexType, OtherExtents...>&>)
      constexpr BasicExtents& operator=(const BasicExtents<IndexType, OtherExtents...>& Other) noexcept{
        this->~Extents();
        new(this) BasicExtents(Other);
        return *this;
      }

      template <typename Ind, /*Core::Detail::ExtentLike<Ind>*/ auto ...ExtA, /*Core::Detail::ExtentLike<Ind>*/ auto ...ExtB>
      friend constexpr bool operator==(const BasicExtents<Ind, ExtA...>&, const BasicExtents<Ind, ExtB...>&) noexcept;

    private:
      template<size_t ...I>
      constexpr BasicExtents(Span<index_type, rankDynamic()> indices, std::index_sequence<I...>) noexcept
          : Storage_{indices[I]...}{}
    };

    template <typename IndexType, Core::Detail::ExtentLike<IndexType> auto ...Ext>
    class BasicExtents<Stride<IndexType>, Ext...> : Detail::ExtentStorage<Stride<IndexType>, normalizeExtent<IndexType>(Ext)...>{
      using Storage_ = Detail::ExtentStorage<Stride<IndexType>, normalizeExtent<IndexType>(Ext)...>;
      using param_type = param_t<IndexType>;
    public:
      using index_type = IndexType;

      constexpr BasicExtents() noexcept = default;
      constexpr BasicExtents(const BasicExtents&) noexcept = default;
      constexpr BasicExtents(BasicExtents&&) noexcept = default;

      constexpr BasicExtents& operator=(const BasicExtents&) noexcept = default;
      constexpr BasicExtents& operator=(BasicExtents&&) noexcept = default;


      VK_nodiscard static constexpr size_t rank() noexcept { return sizeof...(Ext); }
      VK_nodiscard static constexpr size_t rankDynamic() noexcept { return (size_t(std::same_as<decltype(Ext), DynamicExtentType>) + ...); }
      VK_nodiscard static constexpr bool isStaticStride(size_t pos) noexcept {
        return Detail::isStaticExtent<Ext...>(pos);
      }
      VK_nodiscard static constexpr index_type totalStaticStride() noexcept requires(rankDynamic() == 0){
        return (static_cast<index_type>(Ext) * ...);
      }
      template <size_t pos> requires(pos < rank())
      VK_nodiscard inline constexpr param_type stride() const noexcept {
        return Detail::index<pos>(*this);
      }
      VK_nodiscard inline constexpr param_type stride(size_t pos) const noexcept {
        VK_constexpr_assert(pos < rank());
        return Detail::index(pos, *this);
      }

      template</*Core::Detail::ExtentLike<IndexType> */auto ...OtherExtents>
      constexpr BasicExtents(const BasicExtents<Stride<IndexType>, OtherExtents...>& Other) noexcept
      requires(!std::same_as<BasicExtents, BasicExtents<Stride<IndexType>, OtherExtents...>> &&
               ConstructibleFrom<Storage_, const typename BasicExtents<Stride<IndexType>, OtherExtents...>::Storage_&>)
          : Storage_{Detail::copyConstruct, Other}{}

      template<std::convertible_to<index_type>... IndexType_>
      constexpr BasicExtents(IndexType_&&... indices) noexcept requires(sizeof...(IndexType_) == rankDynamic())
          : Storage_(std::forward<IndexType_>(indices)...){}

      constexpr explicit BasicExtents(Span<index_type, rankDynamic()> indices) noexcept
          : BasicExtents(indices, std::make_index_sequence<rankDynamic()>{}){}

      template</*Core::Detail::ExtentLike<IndexType>*/ auto ...OtherExtents>
      requires(ConstructibleFrom<BasicExtents, const BasicExtents<Stride<IndexType>, OtherExtents...>&>)
      constexpr BasicExtents& operator=(const BasicExtents<Stride<IndexType>, OtherExtents...>& Other) noexcept{
        this->~Extents();
        new(this) BasicExtents(Other);
        return *this;
      }

      template <typename Ind, /*Core::Detail::ExtentLike<Ind>*/ auto ...ExtA, /*Core::Detail::ExtentLike<Ind> */auto ...ExtB>
      friend constexpr bool operator==(const BasicExtents<Stride<Ind>, ExtA...>&, const BasicExtents<Stride<Ind>, ExtB...>&) noexcept;

    private:
      template<size_t ...I>
      constexpr BasicExtents(Span<index_type, rankDynamic()> indices, std::index_sequence<I...>) noexcept
          : Storage_{indices[I]...}{}
    };

    template <typename IndexType, auto ...Srd>
    using BasicStrides = BasicExtents<Stride<IndexType>, Srd...>;






    template <typename Ind, /*Core::Detail::ExtentLike<Ind> */auto ...ExtA, /*Core::Detail::ExtentLike<Ind>*/ auto ...ExtB>
    constexpr bool operator==(const BasicExtents<Ind, ExtA...>& A, const BasicExtents<Ind, ExtB...>& B) noexcept {
      return Detail::isEqual(A, B);
    }
    template <typename Ind, /*Core::Detail::ExtentLike<Ind>*/ auto ...ExtA, /*Core::Detail::ExtentLike<Ind> */auto ...ExtB>
    constexpr bool operator==(const BasicExtents<Stride<Ind>, ExtA...>& A, const BasicExtents<Stride<Ind>, ExtB...>& B) noexcept {
      return Detail::isEqual(A, B);
    }

    template <Core::Detail::ExtentLike auto ...Ext>
    using Extents = BasicExtents<ptrdiff_t, Ext...>;
    template <auto ...Ext>
    using Strides = BasicStrides<ptrdiff_t, Ext...>;



    struct LayoutLeft{
      template <typename E>
      class mapping : protected E{

        using param_type = param_t<typename E::index_type>;


        inline constexpr static std::make_index_sequence<E::rank()> indexSequence{};

      public:
        using index_type = typename E::index_type;
        using extents_type = E;

        template <typename OtherE>
        using remap = mapping<OtherE>;

        constexpr mapping() noexcept = default;
        constexpr mapping(const mapping &) noexcept = default;
        constexpr mapping(const extents_type & Ext) noexcept : E(Ext){}
        template <typename OtherE>
        VK_gpu_inline constexpr mapping(const mapping<OtherE>&) noexcept;

        constexpr mapping & operator=(const mapping &) noexcept = default;
        template <typename OtherE>
        VK_gpu_inline constexpr mapping & operator=(const mapping<OtherE>&) noexcept;

        VK_nodiscard VK_gpu_inline constexpr param_t<extents_type> extents() const noexcept { return *this; }

        VK_nodiscard VK_gpu_inline constexpr index_type requiredSpanSize() const noexcept {
          return this->getReqSpanSize_(indexSequence);
        }

        template <typename... I>
        VK_nodiscard VK_gpu_inline constexpr index_type operator()(I... i) const noexcept{
          std::array<index_type, E::rank()> indexArray{ i ... };
          return this->template getLeftHornerIndex_<0>(indexArray, 0);
          //return this->getIndex_(indexSequence, i...);
        }

        VK_nodiscard VK_gpu_inline static constexpr bool isAlwaysUnique() noexcept { return true; }
        VK_nodiscard VK_gpu_inline static constexpr bool isAlwaysContiguous() noexcept { return true; }
        VK_nodiscard VK_gpu_inline static constexpr bool isAlwaysStrided() noexcept { return true; }

        VK_nodiscard VK_gpu_inline constexpr bool isUnique() const noexcept { return true; }
        VK_nodiscard VK_gpu_inline constexpr bool isContiguous() const noexcept { return true; }
        VK_nodiscard VK_gpu_inline constexpr bool isStrided() const noexcept { return true; }

        VK_nodiscard VK_gpu_inline constexpr index_type stride(size_t r) const noexcept {
          index_type result = 1;
          for (u32 i = 0; i < r; ++i)
            result *= this->extent(r);
          return result;
        }

        template <typename OtherE>
        VK_nodiscard VK_gpu_inline constexpr bool operator==(const mapping<OtherE>& otherMapping) const noexcept {
          return extents() == otherMapping.extents();
        }

      private:

        template <size_t N>
        VK_inline constexpr index_type getLeftHornerIndex_(const std::array<index_type, E::rank()>& indices, index_type currentValue) const noexcept {
          if constexpr (N == E::rank() - 1)
            return indices[E::rank() - 1] + currentValue;
          else
            return getLeftHornerIndex_<N + 1>(indices, indices[N] + extents().extent(N) * currentValue);
        }


        template <typename ...V, size_t ...I>
        VK_nodiscard VK_gpu_inline constexpr index_type getIndex_([[maybe_unused]] std::index_sequence<I...>, V... i) const noexcept {
          return VK_horner(right i, this->extent(I));
        }
        template <size_t ...I>
        VK_nodiscard VK_gpu_inline constexpr index_type getReqSpanSize_([[maybe_unused]] std::index_sequence<I...>) const noexcept {
          return (this->extent(I) * ...);
        }
      };
    };
    struct LayoutRight{
      template <typename E>
      class mapping : protected E {

        inline constexpr static std::make_index_sequence<E::rank()> indexSequence{};

      public:

        template <typename OtherE>
        using remap = mapping<OtherE>;

        using index_type = typename E::index_type;
        using extents_type = E;

        constexpr mapping() noexcept = default;
        constexpr mapping(const mapping &) noexcept = default;
        constexpr mapping(const extents_type & Ext) noexcept : E(Ext){}
        template <typename OtherE>
        VK_gpu_inline constexpr mapping(const mapping<OtherE>&) noexcept;

        constexpr mapping & operator=(const mapping &) noexcept = default;
        template <typename OtherE>
        VK_gpu_inline constexpr mapping & operator=(const mapping<OtherE>&) noexcept;

        VK_nodiscard VK_gpu_inline constexpr extents_type extents() const noexcept { return *this; }

        VK_nodiscard VK_gpu_inline constexpr index_type requiredSpanSize() const noexcept {
          return this->getReqSpanSize_(indexSequence);
        }

        template <typename... I>
        VK_nodiscard VK_gpu_inline constexpr index_type operator()(I... i) const noexcept{
          std::array<index_type, E::rank()> indexArray{ i ... };
          return getRightHornerIndex_<E::rank() - 1>(indexArray, 0);
          //return this->getIndex_(indexSequence, i...);
        }

        VK_nodiscard VK_gpu_inline static constexpr bool isAlwaysUnique() noexcept { return true; }
        VK_nodiscard VK_gpu_inline static constexpr bool isAlwaysContiguous() noexcept { return true; }
        VK_nodiscard VK_gpu_inline static constexpr bool isAlwaysStrided() noexcept { return true; }

        VK_nodiscard VK_gpu_inline constexpr bool isUnique() const noexcept { return true; }
        VK_nodiscard VK_gpu_inline constexpr bool isContiguous() const noexcept { return true; }
        VK_nodiscard VK_gpu_inline constexpr bool isStrided() const noexcept { return true; }

        VK_nodiscard VK_gpu_inline constexpr index_type stride(size_t r) const noexcept {
          index_type result = 1;
          for (; r < E::rank() - 1; ++r)
            result *= this->extent(r);
        }

        template <typename OtherE>
        VK_nodiscard VK_gpu_inline constexpr bool operator==(const mapping<OtherE>& otherMapping) const noexcept {
          return extents() == otherMapping.extents();
        }

      private:

        template <size_t N>
        VK_inline constexpr index_type getRightHornerIndex_(const std::array<index_type, E::rank()>& indices, index_type currentValue) const noexcept {
          if constexpr (N == 0)
            return indices[0] + currentValue;
          else
            return getRightHornerIndex_<N - 1>(indices, indices[N] + extents().extent(N) * currentValue);
        }



        template <typename ...V, size_t ...I>
        VK_nodiscard VK_gpu_inline constexpr index_type getIndex_([[maybe_unused]] std::index_sequence<I...>, V... i) const noexcept {
          return VK_horner(left i, this->extent(I));
        }
        template <size_t ...I>
        VK_nodiscard VK_gpu_inline constexpr index_type getReqSpanSize_([[maybe_unused]] std::index_sequence<I...>) const noexcept {
          return (this->extent(I) * ...);
        }
      };
    };
    template <typename S>
    struct LayoutStrided{
      template <typename E> requires(E::rank() == S::rank() && std::convertible_to<typename S::index_type, typename E::index_type>)
      class VK_empty_bases mapping : protected E, protected S{

        /*template <typename>
        friend class mapping;*/

        inline constexpr static std::make_index_sequence<E::rank()> indexSequence{};


      public:


        using index_type = typename E::index_type;
        using extents_type = E;
        using strides_type = S;
        using stride_type = typename S::index_type;

        template <typename OtherE>
        using remap = mapping<OtherE>;

        constexpr mapping() noexcept = default;
        constexpr mapping(const mapping&) noexcept = default;
        constexpr mapping(const extents_type& Extents, const strides_type& Strides) noexcept : E(Extents), S(Strides){}
        template <std::convertible_to<E> OtherE>
        VK_gpu_inline constexpr mapping(const mapping<OtherE>& other) noexcept : E(other.extents()), S(other.strides()){}

        constexpr mapping& operator=(const mapping &) noexcept = default;

        template <std::convertible_to<E> OtherE>
        VK_gpu_inline constexpr mapping& operator=(const mapping<OtherE>& other) noexcept{
          if (this != &other) {
            E::operator=(other.extents());
            S::operator=(other.strides());
          }
          return *this;
        }

        VK_nodiscard VK_gpu_inline constexpr extents_type extents() const noexcept { return *this; }
        VK_nodiscard VK_gpu_inline constexpr strides_type strides() const noexcept { return *this; }

        VK_nodiscard VK_gpu_inline constexpr index_type requiredSpanSize() const noexcept {
          /*index_type maxValue = 0;
          for (u32 r = 0; r < E::rank(); ++r) {
            index_type currentValue = E::extent(r) * S::stride(r);
            if (maxValue < currentValue)
              maxValue = currentValue;
          }
          return maxValue;*/
          return this->getReqSpanSize_(indexSequence);
        }

        template <std::convertible_to<index_type>... I> requires(sizeof...(I) == E::rank())
        VK_nodiscard VK_gpu_inline constexpr index_type operator()(I... i) const noexcept {
#if VK_debug_build
          std::array indices{i ...};
          for (u32 r = 0; r < E::rank(); ++r)
            VK_assert(indices[r] < E::extent(r));
#endif
          return this->getIndex_(indexSequence, i...);
        }

        VK_nodiscard VK_gpu_inline static constexpr bool isAlwaysUnique() noexcept { return true; }
        VK_nodiscard VK_gpu_inline static constexpr bool isAlwaysContiguous() noexcept { return E::rank() == 0; }
        VK_nodiscard VK_gpu_inline static constexpr bool isAlwaysStrided() noexcept { return true; }

        VK_nodiscard VK_gpu_inline constexpr bool isUnique() const noexcept { return true; }
        VK_nodiscard VK_gpu_inline constexpr bool isContiguous() const noexcept {
          return this->getIsContiguous_(indexSequence);
        }
        VK_nodiscard VK_gpu_inline constexpr bool isStrided() const noexcept { return true; }

        VK_nodiscard VK_gpu_inline constexpr stride_type stride(size_t r) const noexcept {
          return S::stride(r);
        }

        template <typename OtherE>
        VK_nodiscard VK_gpu_inline constexpr bool operator==(const mapping<OtherE>& other) const noexcept {
          return extents() == other.extents() && strides() == other.strides();
        }

      private:
        template <typename ...V, size_t ...I>
        VK_nodiscard VK_gpu_inline constexpr index_type getIndex_([[maybe_unused]] std::index_sequence<I...>, V... i) const noexcept {
          return ((i * stride(I)) + ...);
        }
        template <size_t ...I>
        VK_nodiscard VK_gpu_inline constexpr index_type getReqSpanSize_([[maybe_unused]] std::index_sequence<I...>) const noexcept {
          return std::max(std::initializer_list{ (E::extent(I) * S::stride(I)) ...  });
          //return (this->extent(I) * ...);
        }
        template <size_t ...I>
        VK_nodiscard VK_gpu_inline constexpr bool getIsContiguous_([[maybe_unused]] std::index_sequence<I...>) const noexcept{
          if constexpr (E::rank() == 0){
            return true;
          } else {
            std::array<std::pair<u32, std::pair<index_type, stride_type>>, E::rank()> values{ {(u32)I, { E::extent(I), stride(I) }} ... };
            std::sort(values.begin(), values.end(), [](auto&& A, auto&& B){ return A.second.second < B.second.second; });
            if (values[0].second.second != 1)
              return false;
            auto [lastExtent, lastStride] = values[0].second;
            for (auto pos = values.begin() + 1, end = values.end(); pos != end(); ++pos){
              auto&& [i, vals] = *pos;
              if (vals.second != lastExtent * lastStride)
                return false;
              lastExtent = vals.first;
              lastStride = vals.second;
            }
            return true;
          }
        }
      };
    };

    template <typename ElementType, typename ExtentsType>
    struct AccessorBasic{
      using index_type = typename ExtentsType::index_type;
      using offset_policy = AccessorBasic;
      using element_type = ElementType;
      using reference = element_type&;
      using pointer = element_type*;

      template <typename Ext>
      using rebind = AccessorBasic<ElementType, Ext>;

      VK_nodiscard VK_gpu_inline constexpr typename offset_policy::pointer offset(pointer p, ptrdiff_t i) const noexcept {
        return p + i;
      }
      VK_nodiscard VK_gpu_inline constexpr reference access(pointer p, ptrdiff_t i) const noexcept {
        return p[i];
      }
      VK_nodiscard VK_gpu_inline constexpr pointer decay(pointer p) const noexcept {
        return p;
      }
    };
    template <typename ElementType, typename ExtentsType, typename BaseAccessor = AccessorBasic<ElementType, ExtentsType>>
    struct AccessorStrided : BaseAccessor {
      using typename BaseAccessor::index_type;
      using typename BaseAccessor::offset_policy;
      using typename BaseAccessor::element_type;
      using typename BaseAccessor::reference;
      using typename BaseAccessor::pointer;

      template <typename Ext>
      using rebind = AccessorStrided<ElementType, Ext, typename BaseAccessor::template rebind<Ext>>;

      constexpr AccessorStrided() noexcept = default;

      VK_nodiscard VK_gpu_inline constexpr typename offset_policy::pointer offset(pointer p, param_t<index_type> i) const noexcept {
        return BaseAccessor::offset(p, i * stride);
      }
      VK_nodiscard VK_gpu_inline constexpr reference access(pointer p, param_t<index_type> i) const noexcept {
        return BaseAccessor::decay(offset(p, i));
      }

      index_type stride;
    };




    namespace Detail{

      template <typename M>
      concept IsMappingType = requires(const M& map){
        typename M::index_type;
        typename M::extents_type;
        typename M::template remap<Extents<4>>;
        { map.extents() } -> SameAs<typename M::extents_type>;
        { map.requiredSpanSize() } -> SameAs<typename M::index_type>;
        { map.isUnique() } -> std::convertible_to<bool>;
        { map.isContiguous() } -> std::convertible_to<bool>;
        { map.isStrided() } -> std::convertible_to<bool>;
        { M::isAlwaysUnique() } -> std::convertible_to<bool>;
        { M::isAlwaysContiguous() } -> std::convertible_to<bool>;
        { M::isAlwaysStrided() } -> std::convertible_to<bool>;
      };

      template <typename Ext>
      struct IsStatic;
      template <typename T, auto ...Ext>
      struct IsStatic<BasicExtents<T, Ext...>>{
        inline constexpr static bool value = !(SameAs<decltype(Ext), DynamicExtentType> || ...);
      };


      template <typename Ext, typename Layout>
      class ArrayViewStorageMapping : public Layout::template mapping<Ext>{
        using base_ = typename Layout::template mapping<Ext>;
        using mapping = base_;
      protected:
        template <typename ...Args>
        constexpr explicit ArrayViewStorageMapping(Args&& ...args) noexcept : base_{std::forward<Args>(args)...}{}

        inline constexpr mapping& getMapping() noexcept {
          return *this;
        }
        inline constexpr const mapping& getMapping() const noexcept {
          return *this;
        }
      };
      template <typename Ext, typename Layout, typename Accessor>
      class ArrayViewStorage : public Accessor, ArrayViewStorageMapping<Ext, Layout>{

      protected:
        template <typename ...Args> requires(std::default_initializable<Accessor> && ConstructibleFrom<typename Layout::template mapping<Ext>, Args...>)
        constexpr explicit ArrayViewStorage(Args&& ...args) noexcept
            : ArrayViewStorageMapping<Ext, Layout>(std::forward<Args>(args)...){}
        template <typename ...Args> requires(ConstructibleFrom<typename Layout::template mapping<Ext>, Args...>)
        constexpr explicit ArrayViewStorage(const Accessor& accessor, Args&& ...args) noexcept
            : Accessor(accessor), ArrayViewStorageMapping<Ext, Layout>(std::forward<Args>(args)...){}

        inline constexpr Accessor& getAccessor() noexcept { return *this; }
        inline constexpr const Accessor& getAccessor() const noexcept { return *this; }
      };
      template <typename Ext, typename Layout, typename Accessor> requires(std::is_empty_v<Accessor>)
      class ArrayViewStorage<Ext, Layout, Accessor> : public ArrayViewStorageMapping<Ext, Layout>{
        inline constexpr static Accessor accessor_{};
      protected:

        template <typename ...Args>
        constexpr explicit ArrayViewStorage(Args&& ...args) noexcept : ArrayViewStorageMapping<Ext, Layout>(std::forward<Args>(args)...){}

        inline constexpr const Accessor& getAccessor() const noexcept { return accessor_; }
      };
    }




    template <typename T,
              typename Ext,
              typename LayoutPolicy = LayoutRight,
              typename AccessorPolicy = AccessorBasic<T, Ext>>
    class BasicArrayView : public Detail::ArrayViewStorage<Ext, LayoutPolicy, AccessorPolicy>{

      using storage = Detail::ArrayViewStorage<Ext, LayoutPolicy, AccessorPolicy>;

      template <typename OtherT, typename OtherE, typename OtherLayout, typename OtherA>
      friend class BasicArrayView;

      inline constexpr static std::make_index_sequence<Ext::rank()> spanAccessor{};

    public:

      using extents_type = Ext;
      using layout_type = LayoutPolicy;
      using accessor_type = AccessorPolicy;
      using mapping_type = typename layout_type::template mapping<extents_type>;
      using element_type = typename accessor_type::element_type;
      using value_type = std::remove_cv_t<element_type>;
      using index_type = typename Ext::index_type;

      using pointer = typename accessor_type::pointer;
      using reference = typename accessor_type::reference;
      //using difference_type = typename pointer_traits<pointer>::difference_type;
      using difference_type = typename std::pointer_traits<pointer>::difference_type ;

      constexpr BasicArrayView() noexcept = default;
      constexpr BasicArrayView(const BasicArrayView&) noexcept = default;
      constexpr BasicArrayView(BasicArrayView&&) noexcept = default;

      [[nodiscard]] static constexpr u32 rank() noexcept { return extents_type::rank(); }
      [[nodiscard]] static constexpr u32 rankDynamic() noexcept { return extents_type::rankDynamic(); }
      [[nodiscard]] static constexpr index_type staticExtent(size_t r) noexcept { return extents_type::staticExtent(r); }

      template <typename ...Index> requires(sizeof...(Index) == rankDynamic())
      VK_gpu_inline explicit constexpr BasicArrayView(pointer ptr, Index&&... dynamicExtents) noexcept
          : storage(std::forward<Index>(dynamicExtents)...), ptr_{ptr}{}
      template <typename Index>
      VK_gpu_inline explicit constexpr BasicArrayView(param_t<pointer> ptr, Span<Index, rankDynamic()> dynamicExtents) noexcept
          : storage(dynamicExtents), ptr_{ptr}{}
      VK_gpu_inline constexpr BasicArrayView(pointer ptr, const mapping_type& mapping) noexcept
          : storage(mapping), ptr_{ptr}{}
      VK_gpu_inline constexpr BasicArrayView(pointer ptr, const mapping_type & mapping, const accessor_type & accessor) noexcept
          : storage(accessor, mapping), ptr_{ptr}{}
      template <typename OtherT, typename OtherE, typename OtherLayout, typename OtherA>
      VK_gpu_inline constexpr BasicArrayView(const BasicArrayView<OtherT, OtherE, OtherLayout, OtherA>& Other) noexcept
          : storage(Other.accessor(), Other.mapping()), ptr_{Other.ptr_}{}

      VK_gpu_inline constexpr BasicArrayView& operator=(const BasicArrayView&) noexcept = default;
      VK_gpu_inline constexpr BasicArrayView& operator=(BasicArrayView&&) noexcept = default;
      template <typename OtherT, typename OtherE, typename OtherLayout, typename OtherA>
      VK_gpu_inline constexpr BasicArrayView& operator=(const BasicArrayView<OtherT, OtherE, OtherLayout, OtherA>& Other) noexcept;


      VK_nodiscard VK_gpu_inline constexpr reference operator[](param_t<index_type> i) const noexcept requires(rank() == 1) {
        return (*this)(i);
      }
      template <std::convertible_to<index_type> ...Index> requires(sizeof...(Index) == rank())
      VK_nodiscard VK_gpu_inline constexpr reference operator()(Index... indices) const noexcept {
        return this->getAccessor().access(ptr_, this->getMapping()(indices...));
      }
      template <std::convertible_to<index_type> Index>
      VK_nodiscard VK_gpu_inline constexpr reference operator()(Span<Index, rank()> indices) const noexcept {
        return applySpanArgs_(indices, spanAccessor);
      }


      VK_nodiscard VK_gpu_inline constexpr param_t<accessor_type> accessor() const { return this->getAccessor(); }


      [[nodiscard]] static constexpr bool isAlwaysUnique() noexcept{ return mapping_type::isAlwaysUnique(); }
      [[nodiscard]] static constexpr bool isAlwaysContiguous() noexcept{ return mapping_type::isAlwaysContiguous(); }
      [[nodiscard]] static constexpr bool isAlwaysStrided() noexcept{ return mapping_type::isAlwaysStrided(); }

      VK_nodiscard VK_gpu_inline constexpr param_t<mapping_type> mapping() const noexcept { return this->getMapping(); }
      VK_nodiscard VK_gpu_inline constexpr bool isUnique() const noexcept { return this->getMapping().isUnique(); }
      VK_nodiscard VK_gpu_inline constexpr bool isContiguous() const noexcept { return this->getMapping().isContiguous(); }
      VK_nodiscard VK_gpu_inline constexpr bool isStrided() const noexcept { return this->getMapping().isStrided(); }
      VK_nodiscard VK_gpu_inline constexpr index_type stride(size_t r) const { return this->getMapping().stride(r); }

      VK_nodiscard VK_gpu_inline constexpr param_t<extents_type> extents() const noexcept { return this->getMapping().extents(); }
      VK_nodiscard VK_gpu_inline constexpr index_type extent(size_t N) const noexcept { return extents().extent(N); }
      VK_nodiscard VK_gpu_inline constexpr index_type size() const noexcept {
        if constexpr (isAlwaysUnique()) {
          return uniqueSize();
        }
        else {
          index_type ret = 1;
          for (u32 i = 0; i < rank(); ++i)
            ret *= extent(i);
          return ret;
        }
      }
      VK_nodiscard VK_gpu_inline constexpr index_type uniqueSize() const noexcept {
        return this->getMapping().requiredSpanSize();
      }

      VK_nodiscard VK_gpu_inline constexpr Span<element_type> span() const noexcept {
        return {ptr_, size()};
      }
      VK_nodiscard VK_gpu_inline constexpr param_t<pointer> data() const noexcept { return ptr_; }





    protected:
      pointer ptr_{};

    private:

      template <typename Index, size_t ...I>
      VK_nodiscard VK_gpu_inline constexpr auto applySpanArgs_(Span<Index, rank()> span, std::index_sequence<I...>) const noexcept {
        return this->getAccessor().access(ptr_, this->getMapping()(span[I]...));
      }
    };


    template <typename Alloc = std::allocator<byte>>
    struct DynamicStorage{
      template <typename T, typename E>
      class storage : allocator_traits<Alloc>::template rebind<T>{
        using Base = typename allocator_traits<Alloc>::template rebind<T>;

      public:

        using allocator_type = typename allocator_traits<Alloc>::template rebind<T>;
        using alloc_traits = allocator_traits<allocator_type>;

        using pointer = typename alloc_traits::pointer;
        using size_type = typename alloc_traits::size_type;

        inline constexpr static bool isResizable() noexcept { return !Detail::IsStatic<E>::value; }
        inline constexpr static bool isStable() noexcept { return !isResizable(); }
        inline constexpr static bool isUnique() noexcept { return true; }



        constexpr storage() = default;
        explicit storage(size_type initialSize) noexcept(alloc_traits::nothrow)
        : Base(), pData(alloc_traits::allocate(*this, initialSize)){
          VK_assert(pData != nullptr);
        }
        constexpr explicit storage(const Alloc& alloc) noexcept : Base(alloc){}
        storage(size_type initialSize, const Alloc& alloc) noexcept(alloc_traits::nothrow)
        : Base(alloc), pData(alloc_traits::allocate(*this, initialSize)){
          VK_assert(pData != nullptr);
        }

        template <template <typename> typename Mapping> requires(Detail::IsMappingType<Mapping<E>>)
        storage(const storage& other, const Mapping<E>& mapping) noexcept(alloc_traits::nothrow)
        : Base(other.getAllocator()), pData(alloc_traits::clone(*this, mapping.requiredSpanSize())){}
        storage(storage&& other) noexcept : pData(std::move(other.pData)){
          other.pData = nullptr;
        }

        ~storage(){
          if (pData != nullptr)
            alloc_traits::deallocate(*this, pData);
        }





        inline constexpr pointer getPointer() const noexcept {
          return pData;
        }
        inline constexpr param_t<allocator_type> getAllocator() const noexcept{
          return *this;
        }


      private:
        pointer pData = nullptr;
      };
    };
    template <size_t N>
    struct StaticStorage{
      template <typename T, typename E> requires(E::rankDynamic() > 0 || E::totalStaticExtent() <= N)
      class storage{
      public:
        using allocator_type = void;
      };
    };
    template <size_t InlineElements, typename Alloc = std::allocator<byte>>
    struct SmallOptimizedStorage{
    private:
      template <typename T>
      class inline_buffer{

      };





    public:
      template <typename T, typename E>
      class storage : allocator_traits<Alloc>::template rebind<T>{
      public:
        using allocator_type = typename allocator_traits<Alloc>::template rebind<T>;
        using alloc_traits = allocator_traits<allocator_type>;
      };
    };


    struct InlineStorage{
      template <typename T, typename E> requires(Detail::IsStatic<E>::value)
      class storage{
        inline constexpr static size_t RequiredStorage{E::totalStaticExtent()};

      public:



        using pointer = T*;
        using size_type = std::make_unsigned_t<typename E::index_type>;
        using allocator_type = void;

        constexpr storage() = default;
        constexpr explicit storage(size_type initialSize){
          VK_assert(initialSize == RequiredStorage);
        }
        constexpr storage(const storage& other) noexcept(std::is_nothrow_copy_constructible_v<T>) = default;
        template <Detail::IsMappingType M>
        constexpr storage(const storage& other, const M& mapping) noexcept(std::is_nothrow_copy_constructible_v<T>)
            : storage(other){}


        inline constexpr static bool isResizable() noexcept { return false; }
        inline constexpr static bool isStable() noexcept { return true; }
        inline constexpr static bool isUnique() noexcept { return true; }

        inline constexpr pointer getPointer() const noexcept {
          return storage_;
        }

      private:
        T storage_[RequiredStorage];
      };
    };

    /*struct UniqueOwnership{

    };
    struct LocalSharedOwnership{};
    struct SharedOwnership{};*/


    template <typename T,
              typename Ext,
              typename StoragePolicy = DynamicStorage<>,
              typename LayoutPolicy = LayoutRight,
              typename AccessorPolicy = AccessorBasic<T, Ext>>
    class BasicArray : public Detail::ArrayViewStorage<Ext, LayoutPolicy, AccessorPolicy>{
      using view_storage = Detail::ArrayViewStorage<Ext, LayoutPolicy, AccessorPolicy>;

      inline constexpr static std::make_index_sequence<Ext::rank()> spanAccessor{};


      inline constexpr const view_storage& getViewStorage() const noexcept {
        return *this;
      }


    public:

      using extents_type = Ext;
      using layout_type = LayoutPolicy;
      using storage_type = typename StoragePolicy::template storage<T, Ext>;
      using allocator_type = typename storage_type::allocator_type;
      using accessor_type = AccessorPolicy;
      using mapping_type = typename layout_type::template mapping<extents_type>;
      using element_type = typename accessor_type::element_type;
      using value_type = std::remove_cv_t<element_type>;
      using index_type = typename Ext::index_type;

      using view_type  = BasicArrayView<T, Ext, LayoutPolicy, AccessorPolicy>;

      using pointer = typename accessor_type::pointer;
      using reference = typename accessor_type::reference;
      using difference_type = typename pointer_traits<pointer>::difference_type;


      constexpr BasicArray() noexcept = default;
      constexpr BasicArray(const BasicArray& other) noexcept : view_storage(other.getViewStorage()), storage_(other.storage_, other.getMapping()){}
      constexpr BasicArray(BasicArray&&) noexcept = default;

      [[nodiscard]] static constexpr u32 rank() noexcept { return extents_type::rank(); }
      [[nodiscard]] static constexpr u32 rankDynamic() noexcept { return extents_type::rankDynamic(); }
      [[nodiscard]] static constexpr index_type staticExtent(size_t r) noexcept { return extents_type::staticExtent(r); }

      [[nodiscard]] static constexpr bool isResizable() noexcept { return rankDynamic() > 0 && storage_type::isResizable(); }
      [[nodiscard]] static constexpr bool isAllocated() noexcept { return NotSameAs<allocator_type, void>; /*TODO: Change to a static allocator type (see boost for example) */}

      [[nodiscard]] static constexpr bool isAlwaysUnique() noexcept{ return mapping_type::isAlwaysUnique(); }
      [[nodiscard]] static constexpr bool isAlwaysContiguous() noexcept{ return mapping_type::isAlwaysContiguous(); }
      [[nodiscard]] static constexpr bool isAlwaysStrided() noexcept{ return mapping_type::isAlwaysStrided(); }



      template <typename ...Index> requires(sizeof...(Index) == rankDynamic())
      VK_gpu_inline explicit constexpr BasicArray(Index&&... dynamicExtents) noexcept
          : view_storage(std::forward<Index>(dynamicExtents)...), storage_{this->uniqueSize()}{}
      template <typename Index>
      VK_gpu_inline explicit constexpr BasicArray(Span<Index, rankDynamic()> dynamicExtents) noexcept
          : view_storage(dynamicExtents), storage_{this->uniqueSize()}{}
      VK_gpu_inline constexpr BasicArray(const mapping_type& mapping) noexcept
          : view_storage(mapping), storage_{this->uniqueSize()}{}
      VK_gpu_inline constexpr BasicArray(const mapping_type & mapping, const accessor_type & accessor) noexcept
          : view_storage(accessor, mapping), storage_{this->uniqueSize()}{}
      /*template <typename OtherT, typename OtherE, typename OtherStorage, typename OtherLayout, typename OtherA>
      VK_gpu_inline constexpr BasicArray(const BasicArray<OtherT, OtherE, OtherStorage, OtherLayout, OtherA>& Other) noexcept
          : view_storage(Other.accessor(), Other.mapping()), ptr_{Other.ptr_}{}*/

      VK_gpu_inline constexpr BasicArray& operator=(const BasicArray&) noexcept = default;
      VK_gpu_inline constexpr BasicArray& operator=(BasicArray&&) noexcept = default;
      template <typename OtherT, typename OtherE, typename OtherLayout, typename OtherA>
      VK_gpu_inline constexpr BasicArray& operator=(const BasicArray<OtherT, OtherE, OtherLayout, OtherA>& Other) noexcept;


      VK_nodiscard VK_gpu_inline constexpr reference operator[](param_t<index_type> i) const noexcept requires(rank() == 1) {
        return (*this)(i);
      }
      template <std::convertible_to<index_type> ...Index> requires(sizeof...(Index) == rank())
      VK_nodiscard VK_gpu_inline constexpr reference operator()(Index... indices) const noexcept {
        return this->getAccessor().access(storage_.getPointer(), this->getMapping()(indices...));
      }
      template <std::convertible_to<index_type> Index>
      VK_nodiscard VK_gpu_inline constexpr reference operator()(Span<Index, rank()> indices) const noexcept {
        return applySpanArgs_(indices, spanAccessor);
      }


      VK_nodiscard VK_gpu_inline constexpr param_t<accessor_type> accessor() const { return this->getAccessor(); }

      VK_nodiscard VK_gpu_inline constexpr param_t<mapping_type> mapping() const noexcept { return this->getMapping(); }
      VK_nodiscard VK_gpu_inline constexpr bool isUnique() const noexcept { return this->getMapping().isUnique(); }
      VK_nodiscard VK_gpu_inline constexpr bool isContiguous() const noexcept { return this->getMapping().isContiguous(); }
      VK_nodiscard VK_gpu_inline constexpr bool isStrided() const noexcept { return this->getMapping().isStrided(); }
      VK_nodiscard VK_gpu_inline constexpr index_type stride(size_t r) const { return this->getMapping().stride(r); }

      VK_nodiscard VK_gpu_inline constexpr param_t<extents_type> extents() const noexcept { return this->getMapping().extents(); }
      VK_nodiscard VK_gpu_inline constexpr index_type extent(size_t N) const noexcept { return extents().extent(N); }
      VK_nodiscard VK_gpu_inline constexpr index_type size() const noexcept {
        if constexpr (isAlwaysUnique()) {
          return uniqueSize();
        }
        else {
          index_type ret = 1;
          for (u32 i = 0; i < rank(); ++i)
            ret *= extent(i);
          return ret;
        }
      }
      VK_nodiscard VK_gpu_inline constexpr index_type uniqueSize() const noexcept {
        return this->getMapping().requiredSpanSize();
      }

      VK_nodiscard VK_gpu_inline constexpr view_type view() const noexcept {
        return { data(), mapping(), accessor() };
      }

      VK_nodiscard VK_gpu_inline constexpr Span<element_type> span() const noexcept {
        return { storage_.getPointer(), size() };
      }
      VK_nodiscard VK_gpu_inline constexpr param_t<pointer> data() const noexcept { return storage_.getPointer(); }

    protected:
      storage_type storage_;

    private:
      template <typename Index, size_t ...I>
      VK_nodiscard VK_gpu_inline constexpr auto applySpanArgs_(Span<Index, rank()> span, std::index_sequence<I...>) const noexcept {
        return this->getAccessor().access(data(), this->getMapping()(span[I]...));
      }
    };


    /*namespace Detail{

      template <auto First, auto Last, auto Extent>
      struct SliceInfo{
        inline constexpr static auto extent() noexcept { return Extent; }
        inline constexpr static auto first() noexcept { return First; }
        inline constexpr static auto last() noexcept { return Last; }
      };
      template <auto First>
      struct SliceInfo<First, dynamicExtent, dynamicExtent>{
        inline constexpr        auto extent() const noexcept {
          return last_ - First;
        }
        inline constexpr static auto first() noexcept {
          return First;
        }
        inline constexpr        auto last() const noexcept {
          return last_;
        }
        std::remove_const_t<decltype(First)> last_;
      };
      template <>
      struct SliceInfo<dynamicExtent, dynamicExtent, dynamicExtent>{
        inline constexpr auto extent() const noexcept {
          return last_ - first_;
        }
        inline constexpr auto first() noexcept {
          return first_;
        }
        inline constexpr auto last() const noexcept {
          return last_;
        }
        size_t first_;
        size_t last_;
      };
      template <auto Ext>
      struct SliceInfo<dynamicExtent, dynamicExtent, Ext>{
        inline constexpr auto extent() const noexcept {
          return Ext;
        }
        inline constexpr auto first() noexcept {
          return pos_;
        }
        inline constexpr auto last() const noexcept {
          return pos_ + Ext;
        }
        size_t pos_;
      };

      template <auto F, auto L, auto E>
      inline constexpr static auto staticExtent(SliceInfo<F, L, E>) noexcept {
        return E;
      }

      template <typename SI>
      inline constexpr static auto staticExtent() noexcept {
        return staticExtent(SI{});
      }

      template <typename T>
      struct SliceType{
        template <typename U>
        inline constexpr static u32 staticExtent(U oldExtent) noexcept{
          return 1;
        }

        template <typename U>
        inline constexpr u32 extent(U oldExtent) const noexcept{
          VK_constexpr_assert(value_ < oldExtent);
          return 1;
        }

        constexpr SliceType(T val) noexcept : value_{val}{}


        T value_;
      };
      template <typename T, typename ...Opts>
      struct SliceType<Interval<T, Opts...>>{
        template <typename U>
        inline constexpr static auto staticExtent(U oldExtent) noexcept{
          return dynamicExtent;
        }

        template <typename U>
        inline constexpr auto extent(U oldExtent) const noexcept {
          VK_constexpr_assert(interval_.max - interval_.min <= oldExtent);
          VK_constexpr_assert(interval_.max <= oldExtent);
          return interval_.max - interval_.min;
        }

        template <typename ...Args> requires(ConstructibleFrom<Interval<T, Opts...>, Args...>)
        constexpr SliceType(Args&& ...args) noexcept : interval_{std::forward<Args>(args)...}{
          VK_constexpr_assert(interval_.min < interval_.max);
        }

        Interval<T, Opts...> interval_;
      };
      template <typename T, typename MinType, typename MaxType, MinType minVal, MaxType maxVal>
      struct SliceType<Interval<T, LowerBound<minVal>, UpperBound<maxVal>>>{
        template <typename U>
        inline constexpr static u32 staticExtent(U oldExtent) noexcept{
          return maxVal - minVal;
        }

        template <typename U>
        inline constexpr u32 extent(U oldExtent) const noexcept{
          VK_constexpr_assert(maxVal - minVal <= oldExtent);
          VK_constexpr_assert(maxVal <= oldExtent);
          return maxVal - minVal;
        }

        constexpr SliceType(Interval<T, LowerBound<minVal>, UpperBound<maxVal>>){}
      };
      template <>
      struct SliceType<DynamicExtentType>{
        template <typename U>
        inline constexpr static auto staticExtent(U oldExtent) noexcept {
          return oldExtent;
        }

        template <typename U>
        inline constexpr u32 extent(U oldExtent) const noexcept{
          return oldExtent;
        }

        constexpr SliceType(DynamicExtentType){}
      };

      template <typename T>
      SliceType(T val) -> SliceType<T>;
      template <typename T>
      SliceType(T from, T to) -> SliceType<Interval<T>>;
      SliceType(DynamicExtentType) -> SliceType<DynamicExtentType>;
      template <auto min, auto max>
      SliceType(Meta::ValueWrapper<min>, Meta::ValueWrapper<max>) -> SliceType<Interval<decltype(min), LowerBound<min>, UpperBound<max>>>;

      template <typename Ext, auto ...Out>
      struct ReduceExtents;
      template <typename I, auto Head, auto ...Ext, auto ...Out>
      struct ReduceExtents<BasicExtents<I, Head, Ext...>, Out...> : ReduceExtents<BasicExtents<I, Ext...>, Out..., Head>{};
      template <typename I, auto Head, auto ...Ext, auto ...Out> requires(Head == 1)
      struct ReduceExtents<BasicExtents<I, Head, Ext...>, Out...> : ReduceExtents<BasicExtents<I, Ext...>, Out...>{};
      template <typename I, auto ...Out>
      struct ReduceExtents<BasicExtents<I>, Out...>{
        using type = BasicExtents<I, Out...>;
      };


      template <typename Ext, typename ...>
      struct TransformExtents;
      template <typename I, auto ...E, typename ...S> requires(sizeof...(E) == sizeof...(S))
      struct TransformExtents<BasicExtents<I, E...>, S...>{
        using type = typename ReduceExtents<BasicExtents<I, S::staticExtent(E)...>>::type;

        inline constexpr static u32 rank        = type::rank();
        inline constexpr static u32 dynamicRank = type::rankDynamic();

        template <size_t N>
        inline constexpr static I staticExtent = std::get<N>(std::tuple{ E... });

        template <size_t N, typename Slice>
        VK_inline constexpr static void fillDynamicExtents(const BasicExtents<I, E...>& ext, StaticArray<I, dynamicRank>& dynExt, const Slice& slice) noexcept {
          if constexpr(SameAs<decltype(Slice::staticExtent(std::get<N>(std::tuple{E...}))), DynamicExtentType>){
            if constexpr(BasicExtents<I, E...>::isStaticExtent(N)){
              dynExt.push_back(slice.extent(staticExtent<N>));
            } else {
              dynExt.push_back(slice.extent(ext.template extent<N>()));
            }
          }

        }


        template <size_t ...N>
        inline constexpr static type transform(std::index_sequence<N...>, const BasicExtents<I, E...>& ext, S&&... slices) noexcept {
          StaticArray<I, dynamicRank> dynExt;
          ((fillDynamicExtents<N>(ext, dynExt, slices)), ...);
          return type(dynExt);
        }
        inline constexpr static type transform(const BasicExtents<I, E...>& ext, S&&... slices) noexcept {
          return transform(std::make_index_sequence<sizeof...(E)>{}, ext, std::forward<S>(slices)...);
        }
      };

      template <typename OriginalLayout, typename OriginalExt, typename NewExt>
      struct TransformLayout{};


      template <typename S, typename ...Args>
      concept SliceIndex = requires(S&& s, Args&& ...args){
        { SliceType(std::forward<S>(s), std::forward<Args>(args)...) };
      };
    }*/



    /*template <typename T, typename Ext, typename LayoutPolicy, typename AccessorPolicy, typename ...S>*//* requires(sizeof...(S) == Ext::rank())*//*
    constexpr auto view(const BasicArrayView<T, Ext, LayoutPolicy, AccessorPolicy>& arrayView, Detail::SliceType<S> ...slices) noexcept {
      using transformer_type = Detail::TransformExtents<Ext, Detail::SliceType<S>...>;
      using transformed_extents_type = typename transformer_type::type;
      using result_type = BasicArrayView<T, transformed_extents_type, LayoutPolicy, AccessorPolicy>;
      return result_type {};
    }
    template <typename View, typename ...S>*//* requires(sizeof...(S) == Ext::rank())*//*
    constexpr auto view(const View& arrayView, S&& ...slices) noexcept{
      return view(arrayView, Detail::SliceType{std::forward<S>(slices)}...);
    }*/
  }




  template <typename T, Detail::ExtentLike auto ...Extents>
  using Array = Arrays::BasicArray<T, Arrays::Extents<Extents...>>;
  template <typename T, Detail::ExtentLike auto ...Extents>
  using ArrayView = Arrays::BasicArrayView<T, Arrays::Extents<Extents...>>;



  /*static_assert(sizeof(ArrayView<float, 3, 4>) == sizeof(u64));


  void test(){

    ArrayView<float, 10, 4, 20> arrayView;
    auto result = view(arrayView, 4, _, Interval{6, 10});
  }*/





}// namespace valkyrie::Core


namespace valkyrie {
  // Trait specializations...

  template<typename T, size_t... Extents>
  struct Traits::StaticArray<Core::Array<T, Extents...>> {
    using element_type = T;
    inline constexpr static T *data(Core::Array<T, Extents...> &span) noexcept;
    inline constexpr static const T *data(const Core::Array<T, Extents...> &span) noexcept;
    inline constexpr static size_t size = (Extents * ...);
  };
  template<typename T, typename ExtentsType, typename LayoutType, typename AccessorType>
  requires(ExtentsType::dynamicRank() == 0) struct Traits::StaticArray<Core::Arrays::BasicArrayView<T, ExtentsType, LayoutType, AccessorType>> {
    using element_type = T;
    inline constexpr static T *data(const Core::Arrays::BasicArrayView<T, ExtentsType, LayoutType, AccessorType> &span) noexcept {
      return span.data();
    }
    inline constexpr static size_t size = ExtentsType::totalStaticExtent();
  };
  template<typename T, typename ExtentsType, typename LayoutType, typename AccessorType>
  struct Traits::DynamicArray<Core::Arrays::BasicArrayView<T, ExtentsType, LayoutType, AccessorType>> {
    using element_type = T;
    inline constexpr static T *data(const Core::Arrays::BasicArrayView<T, ExtentsType, LayoutType, AccessorType> &span) noexcept {
      return span.data();
    }
    inline constexpr static size_t size(const Core::Arrays::BasicArrayView<T, ExtentsType, LayoutType, AccessorType> &span) noexcept {
      return static_cast<size_t>(span.uniqueSize());
    }
  };
  template<typename T, size_t Extent_>
  struct Traits::StaticArray<Core::Span<T, Extent_>> {
    using element_type = T;
    inline constexpr static T *data(Core::Span<T, Extent_> span) noexcept {
      return span.data();
    }
    inline constexpr static size_t size = Extent_;
  };

  template<typename T>
  struct Traits::DynamicArray<Core::Span<T>> {
    using element_type = T;
    inline constexpr static T *data(Core::Span<T> span) noexcept {
      return span.data();
    }
    inline constexpr static size_t size(Core::Span<T> span) noexcept {
      return span.size();
    }
  };
  template<typename T, auto Ext>
  struct Traits::View<Core::Span<T, Ext>> {};

  /*template<typename T, size_t N>
  struct Traits::StaticArray<Core::CArray<T, N>> {
    inline constexpr static T *data(Core::CArray<T, N> &cArray) noexcept {
      return cArray.data();
    }
    inline constexpr static const T *data(const Core::CArray<T, N> &cArray) noexcept {
      return cArray.data();
    }
    inline constexpr static size_t size = N;
  };*/
  template<typename T, size_t N>
  struct Traits::StaticArray<T[N]> {
    inline constexpr static T *data(T (&cArray)[N]) noexcept {
      return cArray;
    }
    inline constexpr static size_t size = N;
  };
  template<typename T, size_t N>
  struct Traits::StaticArray<const T[N]> {
    inline constexpr static const T *data(const T (&cArray)[N]) noexcept {
      return cArray;
    }
    inline constexpr static size_t size = N;
  };
}// namespace valkyrie

#endif//VALKYRIE_ARRAY_HPP

#if defined(_ARRAY_)
#ifndef VALKYRIE_STL_COMPAT_ARRAY
#define VALKYRIE_STL_COMPAT_ARRAY

template <typename T, size_t N>
struct valkyrie::Traits::StaticArray<std::array<T, N>>{
  inline constexpr static T* getData(std::array<T, N>& array) noexcept { return array.data(); }
  inline constexpr static const T* getData(const std::array<T, N>& array) noexcept { return array.data(); }
  inline constexpr static size_t size = N;
};

#endif
#endif

#if defined(_VECTOR_)
#ifndef VALKYRIE_STL_COMPAT_VECTOR
#define VALKYRIE_STL_COMPAT_VECTOR

template <typename T, typename Alloc>
struct valkyrie::Traits::DynamicArray<std::vector<T, Alloc>>{
  inline static T* getData(std::vector<T, Alloc>& vector) noexcept { return vector.data(); }
  inline static const T* getData(const std::vector<T, Alloc>& vector) noexcept { return vector.data(); }
  inline static size_t getSize(const std::vector<T, Alloc>& vector) noexcept { return vector.size(); }
};

#endif
#endif

#if defined(_SPAN_)
#ifndef VALKYRIE_STL_COMPAT_SPAN
#define VALKYRIE_STL_COMPAT_SPAN

template <typename T, size_t N> requires(N != std::dynamic_extent)
struct valkyrie::Traits::StaticArray<std::span<T, N>>{
  inline constexpr static T* getData(std::span<T, N> span) noexcept { return span.data(); }
  inline constexpr static size_t size = N;
};
template <typename T>
struct valkyrie::Traits::DynamicArray<std::span<T>>{
  inline constexpr static T* getData(std::span<T> span) noexcept { return span.data(); }
  inline constexpr static size_t getSize(std::span<T> span) noexcept { return span.size(); }
};
template <typename T, size_t Ext>
struct valkyrie::Traits::View<std::span<T, Ext>>{ };

#endif
#endif //VALKYRIE_CORE_ADT_ARRAYS_HPP