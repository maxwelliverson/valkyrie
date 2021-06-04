//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_ADT_ARRAYS_HPP
#define VALKYRIE_ADT_ARRAYS_HPP

#include <valkyrie/policies.hpp>
#include <valkyrie/constants.hpp>
#include <valkyrie/adt/interval.hpp>
#include <valkyrie/math/calculus/polynomial.hpp>
#include <valkyrie/utility/iterator.hpp>


#include <algorithm>
#include <array>
#include <memory>
#include <vector>
#include <span>


namespace valkyrie{

  template <typename IndexType, auto ...Ext>
  class basic_index;

  template <typename IndexType, auto ...Ext>
  class basic_extents;



  namespace impl{
    
    struct static_extent{
      u64 is_dynamic  : 1;
      u64 size : 63;
    };

    consteval static_extent to_static_extent(u64 ext) noexcept {
      return { .is_dynamic = 0, .size = ext };
    }
    consteval static_extent to_static_extent(dynamic_t) noexcept {
      return { .is_dynamic = 1, .size = 0 };
    }
    
    template <size_t N>
    struct static_extent_array{
      VK_constant u64 size = N;
      static_extent extents[N];
    };

    template <typename SizeType, size_t DynamicRank, size_t TotalRank>
    struct index_map{
      using size_type = SizeType;

      VK_constant u64 dynamic_rank = DynamicRank;
      VK_constant u64 total_rank = TotalRank;

      size_type total_to_dynamic[total_rank];
      size_type dynamic_to_total[dynamic_rank];
    };
    
    template <typename ...T>
    consteval static_extent_array<sizeof...(T)> to_extent_array(T ...t) {
      static_extent_array<sizeof...(T)> array{ .extents = { to_static_extent(t) ... } };
      u64 dynamic_count;
      for (static_extent& el : array.extents) {
        if ( el.is_dynamic )
          el.size = dynamic_count++;
      }
      return array;
    }

    template <typename SizeType, size_t DynamicRank, size_t N>
    consteval index_map<SizeType, DynamicRank, N> make_index_map(const static_extent_array<N>& a) {
      using size_type = SizeType;
      using map_type = index_map<size_type, DynamicRank, N>;

      size_type dynamic_index = 0;
      map_type  map{};

      for ( size_type i = 0; i < N; ++i ) {
        if ( a.extents[i].is_dynamic ) {
          map.dynamic_to_total[dynamic_index] = i;
          map.total_to_dynamic[i] = dynamic_index++;
        }
        else {
          map.total_to_dynamic[i] = static_cast<size_type>(-1);
        }
      }

      return map;
    }

    template <typename T, size_t N>
    struct pseudo_array{
      T elements[N];
    };
    
    template <typename IndexType, auto ...Ext>
    struct extents_storage{

      using index_type = IndexType;
      using size_type  = std::make_signed_t<index_type>;

      VK_constant size_type total_rank     = sizeof...(Ext);
      VK_constant size_type dynamic_rank   = (((u64)same_as<dynamic_t, decltype(Ext)>) + ...);

      VK_constant static_extent_array<total_rank>                static_extents = to_extent_array(Ext...);
      VK_constant index_map<size_type, dynamic_rank, total_rank> index_map      = make_index_map<size_type, dynamic_rank>(static_extents);


      constexpr extents_storage() = default;
      constexpr extents_storage(const extents_storage&) = default;
      constexpr extents_storage(extents_storage&&) noexcept = default;

      constexpr extents_storage& operator=(const extents_storage&) = default;
      constexpr extents_storage& operator=(extents_storage&&) noexcept = default;

      template <typename OIndex, auto ...OExt>
      constexpr explicit extents_storage(const extents_storage<OIndex, OExt...>& other) noexcept{

        using other_type = extents_storage<OIndex, OExt...>;
        using other_size_type = typename other_type::size_type;

        for ( other_size_type i = 0; i < dynamic_rank; ++i )
          dynamic_extents.elements[i] = static_cast<index_type>(other[i]);

        VK_constexpr_assert_msg(is_compatible(other),
                                "Tried to construct a static extent from a dynamic extent with a different value.");
      }
      template <typename ...I>
      constexpr explicit extents_storage(I ...i) noexcept
          : dynamic_extents{ .elements = { static_cast<index_type>(i) ... } }{}
      template <typename I>
      constexpr explicit extents_storage(std::span<I, dynamic_rank> span) noexcept{
        for ( size_type i = 0; i < dynamic_rank; ++i )
          dynamic_extents.elements[i] = static_cast<index_type>(span[i]);
      }


      template <typename OIndex, auto ...OExt>
      constexpr extents_storage& operator=(const extents_storage<OIndex, OExt...>& other) noexcept{

        using other_type = extents_storage<OIndex, OExt...>;
        using other_size_type = typename other_type::size_type;

        for ( other_size_type i = 0; i < dynamic_rank; ++i )
          dynamic_extents.elements[i] = static_cast<index_type>(other[i]);

        VK_constexpr_assert_msg(is_compatible(other),
                                "Tried to assign a static extent from a dynamic extent with a different value.");
        return *this;
      }
      

      template <typename OIndex, auto ...OExt>
      constexpr bool is_compatible(const extents_storage<OIndex, OExt...>& other) const noexcept {
        for (auto i = 0; i < total_rank; ++i ) {
          if ( (*this)[i] != static_cast<index_type>(other[i]) ) VK_unlikely
            return false;
        }
        return true;
      }


      pseudo_array<index_type, dynamic_rank> dynamic_extents{};
      
      constexpr index_type operator[](size_type n) const noexcept {
        const auto ext = static_extents.extents[n];
        return ext.is_dynamic ? dynamic_extents.elements[ext.size] : ext.size;
      }
    };



    namespace ext_cpy{

      enum class extents_copy{
        implicitly,
        explicitly,
        never
      };

      consteval extents_copy& operator&=(extents_copy& a, extents_copy b) {
        const extents_copy lookup_table[3][3] = {
            { extents_copy::implicitly, extents_copy::explicitly, extents_copy::never },
            { extents_copy::explicitly, extents_copy::explicitly, extents_copy::never },
            { extents_copy::never,      extents_copy::never,      extents_copy::never }
        };
        return (a = lookup_table[static_cast<u32>(a)][static_cast<u32>(b)]);
      }
      consteval extents_copy extents_copy_type(static_extent from, static_extent to) {
        if ( to.is_dynamic )
          return extents_copy::implicitly;
        if ( from.is_dynamic )
          return extents_copy::explicitly;
        return from.size == to.size ? extents_copy::implicitly : extents_copy::never;
      }
      template <typename StorageA, typename StorageB>
      consteval extents_copy extents_copy_type() {
        using a = StorageA;
        using b = StorageB;

        if constexpr ( same_as<a, void> ) {
          return extents_copy::never;
        }
        else {
          if ( a::static_extents.size != b::static_extents.size )
            return extents_copy::never;

          if ( !static_castable_to<typename a::index_type, typename b::index_type> )
            return extents_copy::never;

          extents_copy ret = convertible_to<typename a::index_type, typename b::index_type> ?
                                                                    extents_copy::implicitly :
                                                                    extents_copy::explicitly;

          for (u64 i = 0; i < a::static_extents.size; ++i)
            ret &= extents_copy_type(a::static_extents.extents[i], b::static_extents.extents[i]);

          return ret;
        }
      }

      template <typename T>
      struct storage {
        using type = void;
      };
      template <typename IndexType, auto ...Ext>
      struct storage<basic_extents<IndexType, Ext...>>{
        using type = extents_storage<IndexType, Ext...>;
      };
    }

    template <typename From, typename To>
    VK_constant auto extents_copy_type = ext_cpy::extents_copy_type<typename ext_cpy::storage<From>::type,
                                                                    typename ext_cpy::storage<To>::type>();


    template <typename From, typename To>
    concept extents_castable_to = not_same_as<From, To> &&
        (extents_copy_type<From, To> != ext_cpy::extents_copy::never);
    template <typename From, typename To>
    concept extents_convertible_to = extents_castable_to<From, To> &&
        (extents_copy_type<From, To> == ext_cpy::extents_copy::implicitly);
  }


  template <typename Ind, auto ...ExtA, auto ...ExtB>
  constexpr bool operator==(const basic_extents<Ind, ExtA...>&, const basic_extents<Ind, ExtB...>&) noexcept;

#define VK_maybe_implicit(...) explicit(!impl::extents_convertible_to<__VA_ARGS__, self>)

  template <typename IndexType, auto ...Ext>
  class basic_extents {
    using self = basic_extents;
  public:
    using index_type = IndexType;
    using size_type  = std::make_signed_t<index_type>;

    constexpr basic_extents() noexcept = default;
    constexpr basic_extents(const basic_extents&) noexcept = default;
    constexpr basic_extents(basic_extents&&) noexcept = default;

    constexpr basic_extents& operator=(const basic_extents&) noexcept = default;
    constexpr basic_extents& operator=(basic_extents&&) noexcept = default;


    VK_nodiscard static constexpr size_type rank() noexcept { return static_cast<size_type>(sizeof...(Ext)); }
    VK_nodiscard static constexpr size_type rank_dynamic() noexcept { return static_cast<size_type>(storage_type::dynamic_rank); }
    VK_nodiscard static constexpr bool is_static_extent(size_type pos) noexcept {
      return !storage_type::static_extents.extents[pos].is_dynamic;
    }
    VK_nodiscard static constexpr index_type total_static_extent() noexcept requires(rank_dynamic() == 0){
      return (static_cast<index_type>(Ext) * ...);
    }
    
    template <size_t pos> requires(pos < rank())
    VK_nodiscard inline constexpr index_type extent() const noexcept {
      return storage[pos];
    }
    VK_nodiscard inline constexpr index_type extent(size_t pos) const noexcept {
      VK_constexpr_assert(pos < rank());
      return storage[pos];
    }

    template <impl::extents_castable_to<self> OExtents>
    constexpr VK_maybe_implicit(OExtents) basic_extents(const OExtents& other) noexcept
        : storage(other.storage) { }

    template<static_castable_to<index_type>... I> requires(sizeof...(I) == rank_dynamic())
    constexpr explicit(!(convertible_to<I, index_type> && ...)) basic_extents(I... indices) noexcept
        : storage(indices...){}

    template <static_castable_to<index_type> I>
    constexpr explicit basic_extents(std::span<I> indices) noexcept
        : storage(std::span<I, rank_dynamic()>{ indices.data(), indices.size() }){
      VK_constexpr_assert( indices.size() == rank_dynamic() );
    }
    template <static_castable_to<index_type> I>
    constexpr explicit(!convertible_to<I, index_type>) basic_extents(std::span<I, rank_dynamic()> indices) noexcept
        : storage(indices){ }

    /*template< auto ...OtherExtents>
    requires(constructible_from<basic_extents, const basic_extents<IndexType, OtherExtents...>&>)
    constexpr basic_extents& operator=(const basic_extents<IndexType, OtherExtents...>& other) noexcept{
      this->~basic_extents();
      new(this) basic_extents(Other);
      return *this;
    }*/

    template <typename Ind, auto ...ExtA, auto ...ExtB>
    friend constexpr bool operator==(const basic_extents<Ind, ExtA...>&, const basic_extents<Ind, ExtB...>&) noexcept;


    template <typename, auto ...>
    friend class basic_extents;


  private:
    using storage_type = impl::extents_storage<index_type, Ext...>;
    storage_type storage;
  };
#undef VK_maybe_implicit


  template <typename Ind, auto ...ExtA, auto ...ExtB>
  constexpr bool operator==(const basic_extents<Ind, ExtA...>& a, const basic_extents<Ind, ExtB...>& b) noexcept {
    
    using ext_a = basic_extents<Ind, ExtA...>;
    using ext_b = basic_extents<Ind, ExtB...>;
    using size_type = typename ext_a::size_type;
    
    if constexpr ( ext_a::rank() != ext_b::rank() ) {
      return false;
    }
    else {
      for ( size_type i = 0; i < ext_a::rank(); ++i )
        if ( a.extent(i) != b.extent(i) )
          return false;
      return true;
    }
  }

  template <auto ...Ext>
  using extents = basic_extents<i64, Ext...>;
  
  
  namespace impl{
    
    template <typename Strides, size_t N>
    struct append_dynamic_to_strides;
    template <typename I, auto ...Ext, size_t N>
    struct append_dynamic_to_strides<basic_extents<I, Ext...>, N>{
      using type = typename append_dynamic_to_strides<basic_extents<I, Ext..., dynamic>, N - 1>::type;
    };
    template <typename Strides>
    struct append_dynamic_to_strides<Strides, 0>{
      using type = Strides;
    };
    
    template <typename Ext, auto ...Strides>
    struct map_strides{
      using type = void;
    };
    template <typename IndexType, auto ...Ext, auto ...Strides> requires(sizeof...(Strides) <= sizeof...(Ext))
    struct map_strides<basic_extents<IndexType, Ext...>, Strides...>{
      using type = typename append_dynamic_to_strides<basic_extents<IndexType, Strides...>, sizeof...(Ext) - sizeof...(Strides)>::type;
    };
    
    
    template <typename Ext, auto ...Strides>
    concept compatible_with_strides = not_same_as<typename map_strides<Ext, Strides...>::type, void>;
  }
  
  
  namespace policy{
    struct layout_left{
      template <typename E>
      class mapping : protected E{

        inline constexpr static std::make_index_sequence<E::rank()> indexSequence{};

      public:
        using index_type = typename E::index_type;
        using size_type  = typename E::size_type;
        using extents_type = E;

        template <typename OtherE>
        using remap = mapping<OtherE>;

        constexpr mapping() noexcept = default;
        constexpr mapping(const mapping &) noexcept = default;
        constexpr mapping(const extents_type & Ext) noexcept : E(Ext){}
        template <typename OtherE> requires constructible_from<E, OtherE>
        constexpr explicit(!convertible_to<OtherE, E>) mapping(const mapping<OtherE>& other) noexcept 
            : E(other.extents()){}

        constexpr mapping & operator=(const mapping &) noexcept = default;
        /*template <typename OtherE>
        constexpr mapping & operator=(const mapping<OtherE>&) noexcept;*/

        VK_nodiscard constexpr in_param_t<extents_type> extents() const noexcept { return *this; }

        VK_nodiscard constexpr index_type required_span_size() const noexcept {
          return this->get_req_span_size();
        }

        template <typename... I> requires ( sizeof...(I) == E::rank() )
        VK_nodiscard VK_forceinline constexpr index_type operator()(I... i) const noexcept{
          std::array<index_type, E::rank()> indexArray{ i ... };
          return get_left_horner_index(std::span<index_type, E::rank()>{indexArray});
        }
        VK_nodiscard VK_forceinline constexpr index_type operator()(std::span<index_type, E::rank()> span) const noexcept{
          return get_left_horner_index(span);
        }

        VK_nodiscard VK_forceinline static constexpr bool is_always_unique() noexcept { return true; }
        VK_nodiscard VK_gpu_inline static constexpr bool is_always_contiguous() noexcept { return true; }
        VK_nodiscard VK_gpu_inline static constexpr bool is_always_strided() noexcept { return true; }

        VK_nodiscard VK_gpu_inline constexpr bool is_unique() const noexcept { return true; }
        VK_nodiscard VK_gpu_inline constexpr bool is_contiguous() const noexcept { return true; }
        VK_nodiscard VK_gpu_inline constexpr bool is_strided() const noexcept { return true; }

        VK_nodiscard VK_gpu_inline constexpr index_type stride(size_type r) const noexcept {
          index_type result = 1;
          for (size_type i = 0; i < r; ++i)
            result *= this->extent(r);
          return result;
        }

        template <typename OtherE>
        VK_nodiscard VK_gpu_inline constexpr bool operator==(const mapping<OtherE>& otherMapping) const noexcept {
          return extents() == otherMapping.extents();
        }

      private:
        
        template <size_type N = 0>
        VK_nodiscard VK_forceinline constexpr index_type get_left_horner_index(std::span<index_type, E::rank()> indices) const noexcept {
          if constexpr (N == E::rank())
            return 0;
          else
            return indices[N] + extents().extent(N) * get_left_horner_index<N + 1>(indices);
        }
        
        template <size_type I = 0>
        VK_nodiscard VK_forceinline constexpr index_type get_req_span_size() const noexcept {
          if constexpr ( I == E::rank() )
            return 1;
          return this->extent(I) * get_req_span_size<I + 1>();
        }
      };
    };
    struct layout_right{
      template <typename E>
      class mapping : protected E {

        inline constexpr static std::make_index_sequence<E::rank()> indexSequence{};

      public:

        template <typename OtherE>
        using remap = mapping<OtherE>;

        using index_type = typename E::index_type;
        using size_type = typename E::size_type;
        using extents_type = E;

        constexpr mapping() noexcept = default;
        constexpr mapping(const mapping &) noexcept = default;
        constexpr mapping(const extents_type & Ext) noexcept : E(Ext){}
        template <typename OtherE> requires constructible_from<E, OtherE>
        constexpr explicit(!convertible_to<OtherE, E>) mapping(const mapping<OtherE>& other) noexcept
            : E(other.extents()){}

        constexpr mapping & operator=(const mapping &) noexcept = default;
        template <typename OtherE>
        /*VK_gpu_inline constexpr mapping & operator=(const mapping<OtherE>&) noexcept;*/

        VK_nodiscard VK_forceinline constexpr extents_type extents() const noexcept { return *this; }

        VK_nodiscard constexpr index_type required_span_size() const noexcept {
          return get_req_span_size();
        }

        template <typename... I> requires ( sizeof...(I) == E::rank() )
        VK_nodiscard constexpr index_type operator()(I... i) const noexcept{
          std::array<index_type, E::rank()> indexArray{ i ... };
          return get_right_horner_index(std::span<index_type, E::rank()>{ indexArray } );
        }

        VK_nodiscard VK_constant bool is_always_unique() noexcept { return true; }
        VK_nodiscard VK_constant bool is_always_contiguous() noexcept { return true; }
        VK_nodiscard VK_constant bool is_always_strided() noexcept { return true; }

        VK_nodiscard constexpr bool is_unique() const noexcept { return true; }
        VK_nodiscard constexpr bool is_contiguous() const noexcept { return true; }
        VK_nodiscard constexpr bool is_strided() const noexcept { return true; }

        VK_nodiscard constexpr index_type stride(size_t r) const noexcept {
          index_type result = 1;
          for (; r < E::rank() - 1; ++r)
            result *= this->extent(r);
          return result;
        }

        template <typename OtherE>
        VK_nodiscard constexpr bool operator==(const mapping<OtherE>& otherMapping) const noexcept {
          return extents() == otherMapping.extents();
        }

      private:

        template <size_type N = E::rank() - 1>
        VK_nodiscard VK_forceinline constexpr index_type get_right_horner_index(std::span<index_type, E::rank()> indices) const noexcept {
          if constexpr (N == 0)
            return indices[0];
          else
            return indices[N] + extents().extent(N) * get_right_horner_index<N - 1>(indices);
        }
        
        template <size_type I = 0>
        VK_nodiscard VK_forceinline constexpr index_type get_req_span_size() const noexcept {
          if constexpr ( I == E::rank() )
            return 1;
          return this->extent(I) * get_req_span_size<I + 1>();
        }
      };
    };
    /*template <auto ...Strides>
    struct layout_strided{
      template <typename E> requires impl::compatible_with_strides<E, Strides...>
      class mapping : protected E{

        template <typename>
        friend class mapping;
        
      public:


        using index_type = typename E::index_type;
        using extents_type = E;
        using strides_type = typename impl::map_strides<E, Strides...>::type;
        using stride_type = typename S::index_type;

        template <typename OtherE>
        using remap = mapping<OtherE>;

        constexpr mapping() noexcept = default;
        constexpr mapping(const mapping&) noexcept = default;
        constexpr mapping(const extents_type& extents, const strides_type& strides) noexcept 
            : E(extents), 
              stride_storage(strides){}
        template <convertible_to<E> OtherE>
        constexpr mapping(const mapping<OtherE>& other) noexcept 
            : E(other.extents()), stride_storage(other.strides()){}

        constexpr mapping& operator=(const mapping &) noexcept = default;

        template <convertible_to<E> OtherE>
        constexpr mapping& operator=(const mapping<OtherE>& other) noexcept{
          if constexpr ( exact_same_as<E, OtherE> ) {
            if (this != &other) {
              E::operator=(other.extents());
              stride_storage = other.strides();
            }
          }
          else {
            E::operator=(other.extents());
            stride_storage = other.strides();
          }
          return *this;
        }

        VK_nodiscard VK_forceinline constexpr in_param_t<extents_type> extents() const noexcept { return *this; }
        VK_nodiscard VK_forceinline constexpr in_param_t<strides_type> strides() const noexcept { return stride_storage; }

        VK_nodiscard constexpr index_type required_span_size() const noexcept {
          return get_req_span_size();
        }

        template <convertible_to<index_type>... I> requires(sizeof...(I) == E::rank())
        VK_nodiscard constexpr index_type operator()(I... i) const noexcept {
          VK_assert(indices_are_valid(i...));
          std::array<index_type, E::rank()> indices{ i ... };
          return this->private_get_index(std::span<index_type, E::rank()>{indices});
        }

        VK_nodiscard VK_forceinline static constexpr bool is_always_unique() noexcept { return true; }
        VK_nodiscard VK_forceinline static constexpr bool is_always_contiguous() noexcept { return E::rank() == 0; }
        VK_nodiscard VK_forceinline static constexpr bool is_always_strided() noexcept { return true; }

        VK_nodiscard VK_forceinline constexpr bool is_unique() const noexcept { return true; }
        VK_nodiscard constexpr bool is_contiguous() const noexcept {
          return get_is_contiguous(contiguous_index_seq);
        }
        VK_nodiscard VK_forceinline constexpr bool is_strided() const noexcept { return true; }

        VK_nodiscard VK_forceinline constexpr stride_type stride(size_t r) const noexcept {
          return stride_storage.extent(r);
        }

        template <typename OtherE>
        VK_nodiscard constexpr bool operator==(const mapping<OtherE>& other) const noexcept {
          return extents() == other.extents() && strides() == other.strides();
        }

      private:
        
        [[no_unique_address]] strides_type stride_storage;

        template <std::convertible_to<index_type>... I>
        VK_nodiscard constexpr bool indices_are_valid(I ...i) const noexcept {
          if constexpr ( E::rank() == 0 )
            return true;
          else {
            std::array<index_type, sizeof...(I)> indices{i ...};
            for (u32 r = 0; r < E::rank(); ++r)
              if ( indices[r] >= E::extent(r) )
                return false;
            return true;
          }
        }

        template <size_t I = 0>
        VK_nodiscard VK_forceinline constexpr index_type private_get_index(std::span<index_type, E::rank()> indices) const noexcept {
          if constexpr ( I == E::rank() )
            return 0;
          else
            return (indices[I] * stride(I)) + private_get_index<I + 1>();
        }

        template <size_t I = 0>
        VK_nodiscard VK_forceinline constexpr index_type get_req_span_size() const noexcept {
          if constexpr ( I == E::rank() )
            return 1;
          else
            return std::max(this->extent(I) * this->stride(I), get_req_span_size<I + 1>());
        }
        
        VK_constant auto contiguous_index_seq = std::make_index_sequence<E::rank()>{};
        
        template <size_t ...I>
        VK_nodiscard VK_forceinline constexpr bool get_is_contiguous([[maybe_unused]] std::index_sequence<I...>) const noexcept{
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
    };*/

    template <typename ElementType, typename ExtentsType>
    struct basic_accessor{
      using index_type = typename ExtentsType::index_type;
      using offset_policy = basic_accessor;
      using element_type = ElementType;
      using reference = element_type&;
      using pointer = element_type*;

      template <typename Ext>
      using rebind = basic_accessor<ElementType, Ext>;

      VK_nodiscard VK_gpu_inline constexpr typename offset_policy::pointer offset(pointer p, i64 i) const noexcept {
        return p + i;
      }
      VK_nodiscard VK_gpu_inline constexpr reference access(pointer p, i64 i) const noexcept {
        return p[i];
      }
      VK_nodiscard VK_gpu_inline constexpr pointer decay(pointer p) const noexcept {
        return p;
      }
    };
    template <typename ElementType, typename ExtentsType, typename BaseAccessor = basic_accessor<ElementType, ExtentsType>>
    struct strided_accessor : BaseAccessor {
      using typename BaseAccessor::index_type;
      using typename BaseAccessor::offset_policy;
      using typename BaseAccessor::element_type;
      using typename BaseAccessor::reference;
      using typename BaseAccessor::pointer;

      template <typename Ext>
      using rebind = strided_accessor<ElementType, Ext, typename BaseAccessor::template rebind<Ext>>;

      constexpr strided_accessor() noexcept = default;

      VK_nodiscard constexpr typename offset_policy::pointer offset(pointer p, in_param_t<index_type> i) const noexcept {
        return BaseAccessor::offset(p, i * stride);
      }
      VK_nodiscard constexpr reference access(pointer p, in_param_t<index_type> i) const noexcept {
        return BaseAccessor::decay(offset(p, i));
      }

      index_type stride;
    };
  }


  template <typename T, 
            typename Extents, 
            array_layout_p<Extents> LayoutPolicy = policy::layout_left,
            array_accessor_p AccessorPolicy = policy::basic_accessor<T, Extents>>
  class basic_array_view{

    template <typename, typename OtherE, array_layout_p<OtherE>, array_accessor_p>
    friend class basic_array_view;

    inline constexpr static std::make_index_sequence<Extents::rank()> spanAccessor{};

  public:

    using extents_type = Extents;
    using layout_type = LayoutPolicy;
    using accessor_type = AccessorPolicy;
    using mapping_type = typename layout_type::template mapping<extents_type>;
    using element_type = typename accessor_type::element_type;
    using value_type = std::remove_cv_t<element_type>;
    using index_type = typename extents_type::index_type;
    using size_type  = typename extents_type::size_type;

    using pointer = typename accessor_type::pointer;
    using reference = typename accessor_type::reference;
    using difference_type = typename pointer_traits<pointer>::difference_type;
    //using difference_type = typename std::pointer_traits<pointer>::difference_type ;

    constexpr basic_array_view() noexcept = default;
    constexpr basic_array_view(const basic_array_view&) noexcept = default;
    constexpr basic_array_view(basic_array_view&&) noexcept = default;

    [[nodiscard]] static constexpr size_type rank() noexcept { return extents_type::rank(); }
    [[nodiscard]] static constexpr size_type rank_dynamic() noexcept { return extents_type::rank_dynamic(); }
    [[nodiscard]] static constexpr index_type static_extent(size_t r) noexcept { return extents_type::static_extent(r); }

    template <typename ...Ext> requires(sizeof...(Ext) == rank_dynamic())
    VK_gpu_inline constexpr basic_array_view(pointer ptr, Ext... extents) noexcept
        : ptr_{ptr}, mapping_{ extents_type(extents...) }, accessor_{}{}
    template <typename Ext>
    VK_gpu_inline constexpr basic_array_view(pointer ptr, std::span<Ext, rank_dynamic()> extents) noexcept
        : ptr_{ptr}, mapping_{ extents_type(extents) }, accessor_{}{}
    template <typename Ext>
    VK_gpu_inline explicit constexpr basic_array_view(pointer ptr, std::span<Ext> extents) noexcept
        : ptr_{ptr}, mapping_{ extents_type(extents) }, accessor_{}{}
    VK_gpu_inline constexpr basic_array_view(pointer ptr, const mapping_type& mapping) noexcept
        : ptr_{ptr}, mapping_(mapping), accessor_{}{}
    constexpr basic_array_view(pointer ptr, const mapping_type & mapping, const accessor_type & accessor) noexcept
        : ptr_{ptr}, mapping_(mapping), accessor_(accessor){}
    template <typename OtherT, typename OtherE, typename OtherLayout, typename OtherA>
    VK_gpu_inline constexpr basic_array_view(const basic_array_view<OtherT, OtherE, OtherLayout, OtherA>& other) noexcept
        : ptr_{ other.ptr_ }, mapping_{ other.mapping() }, accessor_{ other.accessor() }{}

    VK_gpu_inline constexpr basic_array_view& operator=(const basic_array_view&) noexcept = default;
    VK_gpu_inline constexpr basic_array_view& operator=(basic_array_view&&) noexcept = default;
    template <typename OtherT, typename OtherE, typename OtherLayout, typename OtherA>
    VK_gpu_inline constexpr basic_array_view& operator=(const basic_array_view<OtherT, OtherE, OtherLayout, OtherA>& Other) noexcept;


    VK_nodiscard VK_gpu_inline constexpr reference operator[](index_type i) const noexcept requires(rank() == 1) {
      return (*this)(i);
    }
    template <std::convertible_to<index_type> ...Index> requires(sizeof...(Index) == rank())
    VK_nodiscard VK_gpu_inline constexpr reference operator()(Index... indices) const noexcept {
      return accessor_.access(ptr_, mapping_(indices...));
    }
    template <std::convertible_to<index_type> Index>
    VK_nodiscard VK_gpu_inline constexpr reference operator()(std::span<Index, rank()> indices) const noexcept {
      return accessor_.access(ptr_, mapping_(indices));
    }


    VK_nodiscard VK_gpu_inline constexpr in_param_t<accessor_type> accessor() const { return accessor_; }


    [[nodiscard]] static constexpr bool is_always_unique() noexcept{ return mapping_type::is_always_unique(); }
    [[nodiscard]] static constexpr bool is_always_contiguous() noexcept{ return mapping_type::is_always_contiguous(); }
    [[nodiscard]] static constexpr bool is_always_strided() noexcept{ return mapping_type::is_always_strided(); }

    VK_nodiscard VK_gpu_inline constexpr in_param_t<mapping_type> mapping() const noexcept { return mapping_; }
    VK_nodiscard VK_gpu_inline constexpr bool is_unique() const noexcept { return mapping_.is_unique(); }
    VK_nodiscard VK_gpu_inline constexpr bool is_contiguous() const noexcept { return mapping_.is_contiguous(); }
    VK_nodiscard VK_gpu_inline constexpr bool is_strided() const noexcept { return mapping_.is_strided(); }
    VK_nodiscard VK_gpu_inline constexpr index_type stride(size_t r) const { return mapping_.stride(r); }

    VK_nodiscard VK_gpu_inline constexpr in_param_t<extents_type> extents() const noexcept { return mapping_.extents(); }
    VK_nodiscard VK_gpu_inline constexpr index_type extent(size_t N) const noexcept { return extents().extent(N); }
    VK_nodiscard VK_gpu_inline constexpr index_type size() const noexcept {
      if constexpr (is_always_unique()) {
        return unique_size();
      }
      else {
        index_type ret = 1;
        for (u32 i = 0; i < rank(); ++i)
          ret *= extent(i);
        return ret;
      }
    }
    VK_nodiscard VK_gpu_inline constexpr index_type unique_size() const noexcept {
      return this->getMapping().required_span_size();
    }

    VK_nodiscard VK_gpu_inline constexpr std::span<element_type> range() const noexcept requires(is_always_contiguous()){
      return { ptr_, size() };
    }
    VK_nodiscard VK_gpu_inline constexpr pointer data() const noexcept { return ptr_; }


  protected:
    pointer ptr_{};

    [[no_unique_address]] mapping_type mapping_;
    [[no_unique_address]] accessor_type accessor_;
  };
  
  template <typename T, auto ...Ext>
  using array_view = basic_array_view<T, basic_extents<i64, Ext...>>;
  
  
  
  
  
  
  
  
  
  
  /*namespace Arrays{

    template <typename T>
    struct Stride;

    template <typename T, auto ...Val>
    class BasicIndex{


    private:
      T value_;
    };


    template <typename IndexType, *//*extent_type<IndexType>*//*typename Ext>
    inline constexpr static auto normalizeExtent(Ext extent) noexcept {
      if constexpr (exact_same_as<Ext, dynamic_t>)
        return extent;
      else
        return static_cast<IndexType>(extent);
    }

    namespace detail{

      template <typename T, auto...>
      using discard_value_t = T;

      inline constexpr static enum class CopyConstructTag{} copyConstruct{};


      template <typename T, auto ...Val>
      class IndexStorage{

      };













      template <typename IndexType, *//*detail::strict_extent_type<IndexType> *//*auto ...Ext>
      struct ExtentStorage;
      template <typename IndexType>
      struct ExtentStorage<IndexType>{
        constexpr ExtentStorage() noexcept = default;
        constexpr ExtentStorage(CopyConstructTag, const ExtentStorage&) noexcept{}
      };
      template <typename IndexType, IndexType Head, *//*detail::strict_extent_type<IndexType>*//* auto ...Tail>
      struct ExtentStorage<IndexType, Head, Tail...> : public ExtentStorage<IndexType, Tail...>{

        constexpr ExtentStorage() noexcept = default;
        constexpr ExtentStorage(const ExtentStorage&) noexcept = default;
        constexpr ExtentStorage(ExtentStorage&&) noexcept = default;



        template <*//*detail::strict_extent_type<IndexType>*//* auto ...OtherTail>
        inline constexpr explicit ExtentStorage(CopyConstructTag, const ExtentStorage<IndexType, Head, OtherTail...>& Other) noexcept
            //requires(constructible_from<ExtentStorage<IndexType, Tail...>, CopyConstructTag, const ExtentStorage<IndexType, OtherTail...>&>)
            : ExtentStorage<IndexType, Tail...>(copyConstruct, Other.base()){}
        template <*//*detail::strict_extent_type<IndexType>*//* auto ...OtherTail>
        inline constexpr explicit ExtentStorage(CopyConstructTag, const ExtentStorage<IndexType, dynamic, OtherTail...>& Other) noexcept
            //requires(constructible_from<ExtentStorage<IndexType, Tail...>, CopyConstructTag, const ExtentStorage<IndexType, OtherTail...>&>)
            : ExtentStorage<IndexType, Tail...>(copyConstruct, Other.base()){
          VK_constexpr_assert(Other.extent() == Head);
        }

        template <std::convertible_to<IndexType> ...Index>
        inline constexpr ExtentStorage(Index&& ...values_) noexcept
            : ExtentStorage<IndexType, Tail...>(std::forward<Index>(values_)...){}

        inline constexpr in_param_t<IndexType> extent() const noexcept {
          return Head;
        }
        inline constexpr const ExtentStorage<IndexType, Tail...>& base() const noexcept {
          return *this;
        }
      };
      template <typename IndexType, *//*detail::strict_extent_type<IndexType>*//* auto ...Tail>
      struct ExtentStorage<IndexType, dynamic, Tail...> : public ExtentStorage<IndexType, Tail...>{

        constexpr ExtentStorage() noexcept = default;
        constexpr ExtentStorage(const ExtentStorage&) noexcept = default;
        constexpr ExtentStorage(ExtentStorage&&) noexcept = default;

        template <*//*detail::strict_extent_type<IndexType>*//* auto OtherHead, *//*detail::strict_extent_type<IndexType>*//* auto ...OtherTail>
        inline constexpr ExtentStorage(CopyConstructTag, const ExtentStorage<IndexType, OtherHead, OtherTail...>& Other) noexcept
            requires(constructible_from<ExtentStorage<IndexType, Tail...>, const ExtentStorage<IndexType, OtherTail...>&>)
            : ExtentStorage<IndexType, Tail...>(copyConstruct, Other.base()),
              thisIndex_(Other.extent()){}
        template <std::convertible_to<IndexType> I, std::convertible_to<IndexType> ...Index>
        inline constexpr ExtentStorage(I&& thisValue_, Index&& ...values_) noexcept
            : ExtentStorage<IndexType, Tail...>(std::forward<Index>(values_)...), thisIndex_(std::forward<I>(thisValue_)){}

        inline constexpr in_param_t<IndexType> extent() const noexcept {
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
      template <typename IndexType, IndexType Head, *//*detail::strict_extent_type<IndexType>*//* auto ...Tail>
      struct ExtentStorage<Stride<IndexType>, Head, Tail...> : public ExtentStorage<Stride<IndexType>, Tail...>{

        constexpr ExtentStorage() noexcept = default;
        constexpr ExtentStorage(const ExtentStorage&) noexcept = default;
        constexpr ExtentStorage(ExtentStorage&&) noexcept = default;



        template <*//*detail::strict_extent_type<IndexType>*//* auto ...OtherTail>
        inline constexpr explicit ExtentStorage(CopyConstructTag, const ExtentStorage<Stride<IndexType>, Head, OtherTail...>& Other) noexcept
        //requires(constructible_from<ExtentStorage<IndexType, Tail...>, CopyConstructTag, const ExtentStorage<IndexType, OtherTail...>&>)
            : ExtentStorage<Stride<IndexType>, Tail...>(copyConstruct, Other.base()){}
        template <*//*detail::strict_extent_type<IndexType>*//* auto ...OtherTail>
        inline constexpr explicit ExtentStorage(CopyConstructTag, const ExtentStorage<Stride<IndexType>, dynamic, OtherTail...>& Other) noexcept
        //requires(constructible_from<ExtentStorage<IndexType, Tail...>, CopyConstructTag, const ExtentStorage<IndexType, OtherTail...>&>)
            : ExtentStorage<Stride<IndexType>, Tail...>(copyConstruct, Other.base()){
          VK_constexpr_assert(Other.extent() == Head);
        }

        template <std::convertible_to<IndexType> ...Index>
        inline constexpr ExtentStorage(Index&& ...values_) noexcept
            : ExtentStorage<Stride<IndexType>, Tail...>(std::forward<Index>(values_)...){}

        inline constexpr in_param_t<IndexType> extent() const noexcept {
          return Head;
        }
        inline constexpr const ExtentStorage<Stride<IndexType>, Tail...>& base() const noexcept {
          return *this;
        }
      };
      template <typename IndexType, *//*detail::strict_extent_type<IndexType>*//* auto ...Tail>
      struct ExtentStorage<Stride<IndexType>, dynamic, Tail...> : public ExtentStorage<Stride<IndexType>, Tail...>{

        constexpr ExtentStorage() noexcept = default;
        constexpr ExtentStorage(const ExtentStorage&) noexcept = default;
        constexpr ExtentStorage(ExtentStorage&&) noexcept = default;

        template <*//*detail::strict_extent_type<IndexType>*//* auto OtherHead, *//*detail::strict_extent_type<IndexType>*//* auto ...OtherTail>
        inline constexpr ExtentStorage(CopyConstructTag, const ExtentStorage<Stride<IndexType>, OtherHead, OtherTail...>& Other) noexcept
        requires(constructible_from<ExtentStorage<Stride<IndexType>, Tail...>, const ExtentStorage<Stride<IndexType>, OtherTail...>&>)
            : ExtentStorage<Stride<IndexType>, Tail...>(copyConstruct, Other.base()),
              thisIndex_(Other.extent()){}
        template <std::convertible_to<IndexType> I, std::convertible_to<IndexType> ...Index>
        inline constexpr ExtentStorage(I&& thisValue_, Index&& ...values_) noexcept
            : ExtentStorage<Stride<IndexType>, Tail...>(std::forward<Index>(values_)...), thisIndex_(std::forward<I>(thisValue_)){}

        inline constexpr in_param_t<IndexType> extent() const noexcept {
          return thisIndex_;
        }
        inline constexpr const ExtentStorage<Stride<IndexType>, Tail...>& base() const noexcept {
          return *this;
        }
        IndexType thisIndex_ = 0;
      };




      template <size_t pos, typename IndexType, *//*detail::strict_extent_type<IndexType>*//* auto Head, *//*detail::strict_extent_type<IndexType>*//* auto ...Tail>
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
      template <typename IndexType, *//*detail::strict_extent_type<IndexType>*//* auto Head, *//*detail::strict_extent_type<IndexType>*//* auto ...Tail>
      inline constexpr decltype(auto) index(size_t pos, const ExtentStorage<IndexType, Head, Tail...>& Storage_) noexcept {
        if (!pos) return Storage_.extent();
        return index(pos - 1, Storage_.base());
      }

      template <*//*detail::strict_extent_type*//* auto Head, *//*detail::strict_extent_type*//* auto ...Tail>
      inline constexpr bool isStaticExtent(size_t pos) noexcept {
        if (!pos) return !std::same_as<decltype(Head), dynamic_t>;
        if constexpr (sizeof...(Tail) > 0)
          return isStaticExtent<Tail...>(pos - 1);
        else {
          VK_constexpr_assert(pos == 0xBAD'ACCE55);
          VK_unreachable;
          return false;
        }
      }


      template <typename IndexType>
      inline constexpr bool is_equal(const ExtentStorage<IndexType>& A, const ExtentStorage<IndexType>& B) noexcept{
        return true;
      }

      template <typename IndexType, *//*detail::strict_extent_type<IndexType>*//* auto ...ExtA, *//*detail::strict_extent_type<IndexType>*//* auto ...ExtB>
      requires(sizeof...(ExtA) == sizeof...(ExtB))
      inline constexpr bool is_equal(const ExtentStorage<IndexType, ExtA...>& A, const ExtentStorage<IndexType, ExtB...>& B) noexcept {
        return A.extent() == B.extent() && is_equal(A.base(), B.base());
      }
      template <typename IndexType, *//*detail::strict_extent_type<IndexType>*//* auto ...ExtA, *//*detail::strict_extent_type<IndexType>*//* auto ...ExtB>
      inline constexpr bool is_equal(const ExtentStorage<IndexType, ExtA...>& A, const ExtentStorage<IndexType, ExtB...>& B) noexcept{
        return false;
      }
    }

    template <typename IndexType, auto ...Ext>
    class BasicExtents;

    template <typename IndexType, extent_type<IndexType> auto ...Ext>
    class BasicExtents<IndexType, Ext...> : detail::ExtentStorage<IndexType, normalizeExtent<IndexType>(Ext)...>{
      using Storage_ = detail::ExtentStorage<IndexType, normalizeExtent<IndexType>(Ext)...>;
      using in_param_type = in_param_t<IndexType>;
    public:
      using index_type = IndexType;

      constexpr BasicExtents() noexcept = default;
      constexpr BasicExtents(const BasicExtents&) noexcept = default;
      constexpr BasicExtents(BasicExtents&&) noexcept = default;

      constexpr BasicExtents& operator=(const BasicExtents&) noexcept = default;
      constexpr BasicExtents& operator=(BasicExtents&&) noexcept = default;


      VK_nodiscard static constexpr size_t rank() noexcept { return sizeof...(Ext); }
      VK_nodiscard static constexpr size_t rankDynamic() noexcept { return (size_t(exact_same_as<decltype(Ext), dynamic_t>) + ...); }
      VK_nodiscard static constexpr bool isStaticExtent(size_t pos) noexcept {
        return detail::isStaticExtent<Ext...>(pos);
      }
      VK_nodiscard static constexpr index_type totalStaticExtent() noexcept requires(rankDynamic() == 0){
        return (static_cast<index_type>(Ext) * ...);
      }
      template <size_t pos> requires(pos < rank())
      VK_nodiscard inline constexpr in_param_type extent() const noexcept {
        return detail::index<pos>(*this);
      }
      VK_nodiscard inline constexpr in_param_type extent(size_t pos) const noexcept {
        VK_constexpr_assert(pos < rank());
        return detail::index(pos, *this);
      }

      template<*//*extent_type<IndexType>*//* auto ...OtherExtents>
      constexpr BasicExtents(const BasicExtents<IndexType, OtherExtents...>& Other) noexcept
      requires(!std::same_as<BasicExtents, BasicExtents<IndexType, OtherExtents...>> &&
          constructible_from<Storage_, const typename BasicExtents<IndexType, OtherExtents...>::Storage_&>)
          : Storage_{detail::copyConstruct, Other}{}

      template<std::convertible_to<index_type>... IndexType_>
      constexpr BasicExtents(IndexType_&&... indices) noexcept requires(sizeof...(IndexType_) == rankDynamic())
          : Storage_(std::forward<IndexType_>(indices)...){}

      constexpr explicit BasicExtents(span<index_type, rankDynamic()> indices) noexcept
          : BasicExtents(indices, std::make_index_sequence<rankDynamic()>{}){}

      template<*//*extent_type<IndexType>*//* auto ...OtherExtents>
      requires(constructible_from<BasicExtents, const BasicExtents<IndexType, OtherExtents...>&>)
      constexpr BasicExtents& operator=(const BasicExtents<IndexType, OtherExtents...>& Other) noexcept{
        this->~Extents();
        new(this) BasicExtents(Other);
        return *this;
      }

      template <typename Ind, *//*extent_type<Ind>*//* auto ...ExtA, *//*extent_type<Ind>*//* auto ...ExtB>
      friend constexpr bool operator==(const BasicExtents<Ind, ExtA...>&, const BasicExtents<Ind, ExtB...>&) noexcept;

    private:
      template<size_t ...I>
      constexpr BasicExtents(span<index_type, rankDynamic()> indices, std::index_sequence<I...>) noexcept
          : Storage_{indices[I]...}{}
    };

    template <typename IndexType, extent_type<IndexType> auto ...Ext>
    class BasicExtents<Stride<IndexType>, Ext...> : detail::ExtentStorage<Stride<IndexType>, normalizeExtent<IndexType>(Ext)...>{
      using Storage_ = detail::ExtentStorage<Stride<IndexType>, normalizeExtent<IndexType>(Ext)...>;
      using in_param_type = in_param_t<IndexType>;
    public:
      using index_type = IndexType;

      constexpr BasicExtents() noexcept = default;
      constexpr BasicExtents(const BasicExtents&) noexcept = default;
      constexpr BasicExtents(BasicExtents&&) noexcept = default;

      constexpr BasicExtents& operator=(const BasicExtents&) noexcept = default;
      constexpr BasicExtents& operator=(BasicExtents&&) noexcept = default;


      VK_nodiscard static constexpr size_t rank() noexcept { return sizeof...(Ext); }
      VK_nodiscard static constexpr size_t rankDynamic() noexcept { return (size_t(exact_same_as<decltype(Ext), dynamic_t>) + ...); }
      VK_nodiscard static constexpr bool isStaticStride(size_t pos) noexcept {
        return detail::isStaticExtent<Ext...>(pos);
      }
      VK_nodiscard static constexpr index_type totalStaticStride() noexcept requires(rankDynamic() == 0){
        return (static_cast<index_type>(Ext) * ...);
      }
      template <size_t pos> requires(pos < rank())
      VK_nodiscard inline constexpr in_param_type stride() const noexcept {
        return detail::index<pos>(*this);
      }
      VK_nodiscard inline constexpr in_param_type stride(size_t pos) const noexcept {
        VK_constexpr_assert(pos < rank());
        return detail::index(pos, *this);
      }

      template<*//*extent_type<IndexType> *//*auto ...OtherExtents>
      constexpr BasicExtents(const BasicExtents<Stride<IndexType>, OtherExtents...>& Other) noexcept
      requires(!std::same_as<BasicExtents, BasicExtents<Stride<IndexType>, OtherExtents...>> &&
               constructible_from<Storage_, const typename BasicExtents<Stride<IndexType>, OtherExtents...>::Storage_&>)
          : Storage_{detail::copyConstruct, Other}{}

      template<std::convertible_to<index_type>... IndexType_>
      constexpr BasicExtents(IndexType_&&... indices) noexcept requires(sizeof...(IndexType_) == rankDynamic())
          : Storage_(std::forward<IndexType_>(indices)...){}

      constexpr explicit BasicExtents(span<index_type, rankDynamic()> indices) noexcept
          : BasicExtents(indices, std::make_index_sequence<rankDynamic()>{}){}

      template<*//*extent_type<IndexType>*//* auto ...OtherExtents>
      requires(constructible_from<BasicExtents, const BasicExtents<Stride<IndexType>, OtherExtents...>&>)
      constexpr BasicExtents& operator=(const BasicExtents<Stride<IndexType>, OtherExtents...>& Other) noexcept{
        this->~Extents();
        new(this) BasicExtents(Other);
        return *this;
      }

      template <typename Ind, *//*extent_type<Ind>*//* auto ...ExtA, *//*extent_type<Ind> *//*auto ...ExtB>
      friend constexpr bool operator==(const BasicExtents<Stride<Ind>, ExtA...>&, const BasicExtents<Stride<Ind>, ExtB...>&) noexcept;

    private:
      template<size_t ...I>
      constexpr BasicExtents(span<index_type, rankDynamic()> indices, std::index_sequence<I...>) noexcept
          : Storage_{indices[I]...}{}
    };

    template <typename IndexType, auto ...Srd>
    using BasicStrides = BasicExtents<Stride<IndexType>, Srd...>;






    template <typename Ind, *//*extent_type<Ind> *//*auto ...ExtA, *//*extent_type<Ind>*//* auto ...ExtB>
    constexpr bool operator==(const BasicExtents<Ind, ExtA...>& A, const BasicExtents<Ind, ExtB...>& B) noexcept {
      return detail::is_equal(A, B);
    }
    template <typename Ind, *//*extent_type<Ind>*//* auto ...ExtA, *//*extent_type<Ind> *//*auto ...ExtB>
    constexpr bool operator==(const BasicExtents<Stride<Ind>, ExtA...>& A, const BasicExtents<Stride<Ind>, ExtB...>& B) noexcept {
      return detail::is_equal(A, B);
    }

    template <extent_type auto ...Ext>
    using Extents = BasicExtents<i64, Ext...>;
    template <auto ...Ext>
    using Strides = BasicStrides<i64, Ext...>;



    struct LayoutLeft{
      template <typename E>
      class mapping : protected E{

        using in_param_type = in_param_t<typename E::index_type>;


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

        VK_nodiscard VK_gpu_inline constexpr in_param_t<extents_type> extents() const noexcept { return *this; }

        VK_nodiscard VK_gpu_inline constexpr index_type required_span_size() const noexcept {
          return this->getReqSpanSize_(indexSequence);
        }

        template <typename... I>
        VK_nodiscard VK_gpu_inline constexpr index_type operator()(I... i) const noexcept{
          std::array<index_type, E::rank()> indexArray{ i ... };
          return this->template get_left_horner_index<0>(indexArray, 0);
          //return this->getIndex_(indexSequence, i...);
        }

        VK_nodiscard VK_gpu_inline static constexpr bool is_always_unique() noexcept { return true; }
        VK_nodiscard VK_gpu_inline static constexpr bool is_always_contiguous() noexcept { return true; }
        VK_nodiscard VK_gpu_inline static constexpr bool is_always_strided() noexcept { return true; }

        VK_nodiscard VK_gpu_inline constexpr bool is_unique() const noexcept { return true; }
        VK_nodiscard VK_gpu_inline constexpr bool is_contiguous() const noexcept { return true; }
        VK_nodiscard VK_gpu_inline constexpr bool is_strided() const noexcept { return true; }

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
        VK_inline constexpr index_type get_left_horner_index(const std::array<index_type, E::rank()>& indices, index_type currentValue) const noexcept {
          if constexpr (N == E::rank() - 1)
            return indices[E::rank() - 1] + currentValue;
          else
            return get_left_horner_index<N + 1>(indices, indices[N] + extents().extent(N) * currentValue);
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

        VK_nodiscard VK_gpu_inline constexpr index_type required_span_size() const noexcept {
          return this->getReqSpanSize_(indexSequence);
        }

        template <typename... I>
        VK_nodiscard VK_gpu_inline constexpr index_type operator()(I... i) const noexcept{
          std::array<index_type, E::rank()> indexArray{ i ... };
          return getRightHornerIndex_<E::rank() - 1>(indexArray, 0);
          //return this->getIndex_(indexSequence, i...);
        }

        VK_nodiscard VK_gpu_inline static constexpr bool is_always_unique() noexcept { return true; }
        VK_nodiscard VK_gpu_inline static constexpr bool is_always_contiguous() noexcept { return true; }
        VK_nodiscard VK_gpu_inline static constexpr bool is_always_strided() noexcept { return true; }

        VK_nodiscard VK_gpu_inline constexpr bool is_unique() const noexcept { return true; }
        VK_nodiscard VK_gpu_inline constexpr bool is_contiguous() const noexcept { return true; }
        VK_nodiscard VK_gpu_inline constexpr bool is_strided() const noexcept { return true; }

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

        *//*template <typename>
        friend class mapping;*//*

        inline constexpr static std::make_index_sequence<E::rank()> indexSequence{};

        template <std::convertible_to<index_type>... I>
        bool indices_are_valid(I ...i) const noexcept {
          std::array<index_type, sizeof...(I)> indices{i ...};
          for (u32 r = 0; r < E::rank(); ++r)
            VK_assert(indices[r] < E::extent(r));
        }


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

        VK_nodiscard VK_gpu_inline constexpr index_type required_span_size() const noexcept {
          *//*index_type maxValue = 0;
          for (u32 r = 0; r < E::rank(); ++r) {
            index_type currentValue = E::extent(r) * S::stride(r);
            if (maxValue < currentValue)
              maxValue = currentValue;
          }
          return maxValue;*//*
          return this->getReqSpanSize_(indexSequence);
        }

        template <std::convertible_to<index_type>... I> requires(sizeof...(I) == E::rank())
        VK_nodiscard VK_gpu_inline constexpr index_type operator()(I... i) const noexcept {
          VK_assert(indices_are_valid(i...));
          return this->getIndex_(indexSequence, i...);
        }

        VK_nodiscard VK_gpu_inline static constexpr bool is_always_unique() noexcept { return true; }
        VK_nodiscard VK_gpu_inline static constexpr bool is_always_contiguous() noexcept { return E::rank() == 0; }
        VK_nodiscard VK_gpu_inline static constexpr bool is_always_strided() noexcept { return true; }

        VK_nodiscard VK_gpu_inline constexpr bool is_unique() const noexcept { return true; }
        VK_nodiscard VK_gpu_inline constexpr bool is_contiguous() const noexcept {
          return this->getIsContiguous_(indexSequence);
        }
        VK_nodiscard VK_gpu_inline constexpr bool is_strided() const noexcept { return true; }

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
    struct basic_accessor{
      using index_type = typename ExtentsType::index_type;
      using offset_policy = basic_accessor;
      using element_type = ElementType;
      using reference = element_type&;
      using pointer = element_type*;

      template <typename Ext>
      using rebind = basic_accessor<ElementType, Ext>;

      VK_nodiscard VK_gpu_inline constexpr typename offset_policy::pointer offset(pointer p, i64 i) const noexcept {
        return p + i;
      }
      VK_nodiscard VK_gpu_inline constexpr reference access(pointer p, i64 i) const noexcept {
        return p[i];
      }
      VK_nodiscard VK_gpu_inline constexpr pointer decay(pointer p) const noexcept {
        return p;
      }
    };
    template <typename ElementType, typename ExtentsType, typename BaseAccessor = basic_accessor<ElementType, ExtentsType>>
    struct strided_accessor : BaseAccessor {
      using typename BaseAccessor::index_type;
      using typename BaseAccessor::offset_policy;
      using typename BaseAccessor::element_type;
      using typename BaseAccessor::reference;
      using typename BaseAccessor::pointer;

      template <typename Ext>
      using rebind = strided_accessor<ElementType, Ext, typename BaseAccessor::template rebind<Ext>>;

      constexpr strided_accessor() noexcept = default;

      VK_nodiscard VK_gpu_inline constexpr typename offset_policy::pointer offset(pointer p, in_param_t<index_type> i) const noexcept {
        return BaseAccessor::offset(p, i * stride);
      }
      VK_nodiscard VK_gpu_inline constexpr reference access(pointer p, in_param_t<index_type> i) const noexcept {
        return BaseAccessor::decay(offset(p, i));
      }

      index_type stride;
    };




    namespace detail{

      template <typename M>
      concept IsMappingType = requires(const M& map){
        typename M::index_type;
        typename M::extents_type;
        typename M::template remap<Extents<4>>;
        { map.extents() } -> same_as<typename M::extents_type>;
        { map.required_span_size() } -> same_as<typename M::index_type>;
        { map.is_unique() } -> std::convertible_to<bool>;
        { map.is_contiguous() } -> std::convertible_to<bool>;
        { map.is_strided() } -> std::convertible_to<bool>;
        { M::is_always_unique() } -> std::convertible_to<bool>;
        { M::is_always_contiguous() } -> std::convertible_to<bool>;
        { M::is_always_strided() } -> std::convertible_to<bool>;
      };

      template <typename Ext>
      struct IsStatic;
      template <typename T, auto ...Ext>
      struct IsStatic<BasicExtents<T, Ext...>>{
        inline constexpr static bool value = !(same_as<decltype(Ext), dynamic_t> || ...);
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
        template <typename ...Args> requires(std::default_initializable<Accessor> && constructible_from<typename Layout::template mapping<Ext>, Args...>)
        constexpr explicit ArrayViewStorage(Args&& ...args) noexcept
            : ArrayViewStorageMapping<Ext, Layout>(std::forward<Args>(args)...){}
        template <typename ...Args> requires(constructible_from<typename Layout::template mapping<Ext>, Args...>)
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
              typename AccessorPolicy = basic_accessor<T, Ext>>
    class basic_array_view : public detail::ArrayViewStorage<Ext, LayoutPolicy, AccessorPolicy>{

      using storage = detail::ArrayViewStorage<Ext, LayoutPolicy, AccessorPolicy>;

      template <typename OtherT, typename OtherE, typename OtherLayout, typename OtherA>
      friend class basic_array_view;

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

      constexpr basic_array_view() noexcept = default;
      constexpr basic_array_view(const basic_array_view&) noexcept = default;
      constexpr basic_array_view(basic_array_view&&) noexcept = default;

      [[nodiscard]] static constexpr u32 rank() noexcept { return extents_type::rank(); }
      [[nodiscard]] static constexpr u32 rankDynamic() noexcept { return extents_type::rankDynamic(); }
      [[nodiscard]] static constexpr index_type staticExtent(size_t r) noexcept { return extents_type::staticExtent(r); }

      template <typename ...Index> requires(sizeof...(Index) == rankDynamic())
      VK_gpu_inline explicit constexpr basic_array_view(pointer ptr, Index&&... is_dynamics) noexcept
          : storage(std::forward<Index>(is_dynamics)...), ptr_{ptr}{}
      template <typename Index>
      VK_gpu_inline explicit constexpr basic_array_view(in_param_t<pointer> ptr, span<Index, rankDynamic()> is_dynamics) noexcept
          : storage(is_dynamics), ptr_{ptr}{}
      VK_gpu_inline constexpr basic_array_view(pointer ptr, const mapping_type& mapping) noexcept
          : storage(mapping), ptr_{ptr}{}
      VK_gpu_inline constexpr basic_array_view(pointer ptr, const mapping_type & mapping, const accessor_type & accessor) noexcept
          : storage(accessor, mapping), ptr_{ptr}{}
      template <typename OtherT, typename OtherE, typename OtherLayout, typename OtherA>
      VK_gpu_inline constexpr basic_array_view(const basic_array_view<OtherT, OtherE, OtherLayout, OtherA>& Other) noexcept
          : storage(Other.accessor(), Other.mapping()), ptr_{Other.ptr_}{}

      VK_gpu_inline constexpr basic_array_view& operator=(const basic_array_view&) noexcept = default;
      VK_gpu_inline constexpr basic_array_view& operator=(basic_array_view&&) noexcept = default;
      template <typename OtherT, typename OtherE, typename OtherLayout, typename OtherA>
      VK_gpu_inline constexpr basic_array_view& operator=(const basic_array_view<OtherT, OtherE, OtherLayout, OtherA>& Other) noexcept;


      VK_nodiscard VK_gpu_inline constexpr reference operator[](in_param_t<index_type> i) const noexcept requires(rank() == 1) {
        return (*this)(i);
      }
      template <std::convertible_to<index_type> ...Index> requires(sizeof...(Index) == rank())
      VK_nodiscard VK_gpu_inline constexpr reference operator()(Index... indices) const noexcept {
        return this->getAccessor().access(ptr_, this->getMapping()(indices...));
      }
      template <std::convertible_to<index_type> Index>
      VK_nodiscard VK_gpu_inline constexpr reference operator()(span<Index, rank()> indices) const noexcept {
        return applySpanArgs_(indices, spanAccessor);
      }


      VK_nodiscard VK_gpu_inline constexpr in_param_t<accessor_type> accessor() const { return this->getAccessor(); }


      [[nodiscard]] static constexpr bool is_always_unique() noexcept{ return mapping_type::is_always_unique(); }
      [[nodiscard]] static constexpr bool is_always_contiguous() noexcept{ return mapping_type::is_always_contiguous(); }
      [[nodiscard]] static constexpr bool is_always_strided() noexcept{ return mapping_type::is_always_strided(); }

      VK_nodiscard VK_gpu_inline constexpr in_param_t<mapping_type> mapping() const noexcept { return this->getMapping(); }
      VK_nodiscard VK_gpu_inline constexpr bool is_unique() const noexcept { return this->getMapping().is_unique(); }
      VK_nodiscard VK_gpu_inline constexpr bool is_contiguous() const noexcept { return this->getMapping().is_contiguous(); }
      VK_nodiscard VK_gpu_inline constexpr bool is_strided() const noexcept { return this->getMapping().is_strided(); }
      VK_nodiscard VK_gpu_inline constexpr index_type stride(size_t r) const { return this->getMapping().stride(r); }

      VK_nodiscard VK_gpu_inline constexpr in_param_t<extents_type> extents() const noexcept { return this->getMapping().extents(); }
      VK_nodiscard VK_gpu_inline constexpr index_type extent(size_t N) const noexcept { return extents().extent(N); }
      VK_nodiscard VK_gpu_inline constexpr index_type size() const noexcept {
        if constexpr (is_always_unique()) {
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
        return this->getMapping().required_span_size();
      }

      VK_nodiscard VK_gpu_inline constexpr span<element_type> range() const noexcept {
        return {ptr_, size()};
      }
      VK_nodiscard VK_gpu_inline constexpr in_param_t<pointer> data() const noexcept { return ptr_; }





    protected:
      pointer ptr_{};

    private:

      template <typename Index, size_t ...I>
      VK_nodiscard VK_gpu_inline constexpr auto applySpanArgs_(span<Index, rank()> span, std::index_sequence<I...>) const noexcept {
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

        inline constexpr static bool is_resizable() noexcept { return !detail::IsStatic<E>::value; }
        inline constexpr static bool is_stable() noexcept { return !is_resizable(); }
        inline constexpr static bool is_unique() noexcept { return true; }



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

        template <template <typename> typename Mapping> requires(detail::IsMappingType<Mapping<E>>)
        storage(const storage& other, const Mapping<E>& mapping) noexcept(alloc_traits::nothrow)
        : Base(other.getAllocator()), pData(alloc_traits::clone(*this, mapping.required_span_size())){}
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
        inline constexpr in_param_t<allocator_type> getAllocator() const noexcept{
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
      template <typename T, typename E> requires(detail::IsStatic<E>::value)
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
        template <detail::IsMappingType M>
        constexpr storage(const storage& other, const M& mapping) noexcept(std::is_nothrow_copy_constructible_v<T>)
            : storage(other){}


        inline constexpr static bool is_resizable() noexcept { return false; }
        inline constexpr static bool is_stable() noexcept { return true; }
        inline constexpr static bool is_unique() noexcept { return true; }

        inline constexpr pointer getPointer() const noexcept {
          return storage_;
        }

      private:
        T storage_[RequiredStorage];
      };
    };

    *//*struct UniqueOwnership{

    };
    struct LocalSharedOwnership{};
    struct SharedOwnership{};*//*


    template <typename T,
              typename Ext,
              typename StoragePolicy = DynamicStorage<>,
              typename LayoutPolicy = LayoutRight,
              typename AccessorPolicy = basic_accessor<T, Ext>>
    class BasicArray : public detail::ArrayViewStorage<Ext, LayoutPolicy, AccessorPolicy>{
      using view_storage = detail::ArrayViewStorage<Ext, LayoutPolicy, AccessorPolicy>;

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

      using view_type  = basic_array_view<T, Ext, LayoutPolicy, AccessorPolicy>;

      using pointer = typename accessor_type::pointer;
      using reference = typename accessor_type::reference;
      using difference_type = typename pointer_traits<pointer>::difference_type;


      constexpr BasicArray() noexcept = default;
      constexpr BasicArray(const BasicArray& other) noexcept : view_storage(other.getViewStorage()), storage_(other.storage_, other.getMapping()){}
      constexpr BasicArray(BasicArray&&) noexcept = default;

      [[nodiscard]] static constexpr u32 rank() noexcept { return extents_type::rank(); }
      [[nodiscard]] static constexpr u32 rankDynamic() noexcept { return extents_type::rankDynamic(); }
      [[nodiscard]] static constexpr index_type staticExtent(size_t r) noexcept { return extents_type::staticExtent(r); }

      [[nodiscard]] static constexpr bool is_resizable() noexcept { return rankDynamic() > 0 && storage_type::is_resizable(); }
      [[nodiscard]] static constexpr bool is_allocated() noexcept { return not_same_as<allocator_type, void>; *//*TODO: Change to a static allocator type (see boost for example) *//*}

      [[nodiscard]] static constexpr bool is_always_unique() noexcept{ return mapping_type::is_always_unique(); }
      [[nodiscard]] static constexpr bool is_always_contiguous() noexcept{ return mapping_type::is_always_contiguous(); }
      [[nodiscard]] static constexpr bool is_always_strided() noexcept{ return mapping_type::is_always_strided(); }



      template <typename ...Index> requires(sizeof...(Index) == rankDynamic())
      VK_gpu_inline explicit constexpr BasicArray(Index&&... is_dynamics) noexcept
          : view_storage(std::forward<Index>(is_dynamics)...), storage_{this->uniqueSize()}{}
      template <typename Index>
      VK_gpu_inline explicit constexpr BasicArray(span<Index, rankDynamic()> is_dynamics) noexcept
          : view_storage(is_dynamics), storage_{this->uniqueSize()}{}
      VK_gpu_inline constexpr BasicArray(const mapping_type& mapping) noexcept
          : view_storage(mapping), storage_{this->uniqueSize()}{}
      VK_gpu_inline constexpr BasicArray(const mapping_type & mapping, const accessor_type & accessor) noexcept
          : view_storage(accessor, mapping), storage_{this->uniqueSize()}{}
      *//*template <typename OtherT, typename OtherE, typename OtherStorage, typename OtherLayout, typename OtherA>
      VK_gpu_inline constexpr BasicArray(const BasicArray<OtherT, OtherE, OtherStorage, OtherLayout, OtherA>& Other) noexcept
          : view_storage(Other.accessor(), Other.mapping()), ptr_{Other.ptr_}{}*//*

      VK_gpu_inline constexpr BasicArray& operator=(const BasicArray&) noexcept = default;
      VK_gpu_inline constexpr BasicArray& operator=(BasicArray&&) noexcept = default;
      template <typename OtherT, typename OtherE, typename OtherLayout, typename OtherA>
      VK_gpu_inline constexpr BasicArray& operator=(const BasicArray<OtherT, OtherE, OtherLayout, OtherA>& Other) noexcept;


      VK_nodiscard VK_gpu_inline constexpr reference operator[](in_param_t<index_type> i) const noexcept requires(rank() == 1) {
        return (*this)(i);
      }
      template <std::convertible_to<index_type> ...Index> requires(sizeof...(Index) == rank())
      VK_nodiscard VK_gpu_inline constexpr reference operator()(Index... indices) const noexcept {
        return this->getAccessor().access(storage_.getPointer(), this->getMapping()(indices...));
      }
      template <std::convertible_to<index_type> Index>
      VK_nodiscard VK_gpu_inline constexpr reference operator()(span<Index, rank()> indices) const noexcept {
        return applySpanArgs_(indices, spanAccessor);
      }


      VK_nodiscard VK_gpu_inline constexpr in_param_t<accessor_type> accessor() const { return this->getAccessor(); }

      VK_nodiscard VK_gpu_inline constexpr in_param_t<mapping_type> mapping() const noexcept { return this->getMapping(); }
      VK_nodiscard VK_gpu_inline constexpr bool is_unique() const noexcept { return this->getMapping().is_unique(); }
      VK_nodiscard VK_gpu_inline constexpr bool is_contiguous() const noexcept { return this->getMapping().is_contiguous(); }
      VK_nodiscard VK_gpu_inline constexpr bool is_strided() const noexcept { return this->getMapping().is_strided(); }
      VK_nodiscard VK_gpu_inline constexpr index_type stride(size_t r) const { return this->getMapping().stride(r); }

      VK_nodiscard VK_gpu_inline constexpr in_param_t<extents_type> extents() const noexcept { return this->getMapping().extents(); }
      VK_nodiscard VK_gpu_inline constexpr index_type extent(size_t N) const noexcept { return extents().extent(N); }
      VK_nodiscard VK_gpu_inline constexpr index_type size() const noexcept {
        if constexpr (is_always_unique()) {
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
        return this->getMapping().required_span_size();
      }

      VK_nodiscard VK_gpu_inline constexpr view_type view() const noexcept {
        return { data(), mapping(), accessor() };
      }

      VK_nodiscard VK_gpu_inline constexpr span<element_type> range() const noexcept {
        return { storage_.getPointer(), size() };
      }
      VK_nodiscard VK_gpu_inline constexpr in_param_t<pointer> data() const noexcept { return storage_.getPointer(); }

    protected:
      storage_type storage_;

    private:
      template <typename Index, size_t ...I>
      VK_nodiscard VK_gpu_inline constexpr auto applySpanArgs_(span<Index, rank()> span, std::index_sequence<I...>) const noexcept {
        return this->getAccessor().access(data(), this->getMapping()(span[I]...));
      }
    };


    *//*namespace detail{

      template <auto First, auto Last, auto Extent>
      struct SliceInfo{
        inline constexpr static auto extent() noexcept { return Extent; }
        inline constexpr static auto first() noexcept { return First; }
        inline constexpr static auto last() noexcept { return Last; }
      };
      template <auto First>
      struct SliceInfo<First, dynamic, dynamic>{
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
      struct SliceInfo<dynamic, dynamic, dynamic>{
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
      struct SliceInfo<dynamic, dynamic, Ext>{
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
      struct SliceType<interval<T, Opts...>>{
        template <typename U>
        inline constexpr static auto staticExtent(U oldExtent) noexcept{
          return dynamic;
        }

        template <typename U>
        inline constexpr auto extent(U oldExtent) const noexcept {
          VK_constexpr_assert(interval_.max - interval_.min <= oldExtent);
          VK_constexpr_assert(interval_.max <= oldExtent);
          return interval_.max - interval_.min;
        }

        template <typename ...Args> requires(constructible_from<interval<T, Opts...>, Args...>)
        constexpr SliceType(Args&& ...args) noexcept : interval_{std::forward<Args>(args)...}{
          VK_constexpr_assert(interval_.min < interval_.max);
        }

        interval<T, Opts...> interval_;
      };
      template <typename T, typename MinType, typename MaxType, MinType minVal, MaxType maxVal>
      struct SliceType<interval<T, LowerBound<minVal>, UpperBound<maxVal>>>{
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

        constexpr SliceType(interval<T, LowerBound<minVal>, UpperBound<maxVal>>){}
      };
      template <>
      struct SliceType<dynamic_t>{
        template <typename U>
        inline constexpr static auto staticExtent(U oldExtent) noexcept {
          return oldExtent;
        }

        template <typename U>
        inline constexpr u32 extent(U oldExtent) const noexcept{
          return oldExtent;
        }

        constexpr SliceType(dynamic_t){}
      };

      template <typename T>
      SliceType(T val) -> SliceType<T>;
      template <typename T>
      SliceType(T from, T to) -> SliceType<interval<T>>;
      SliceType(dynamic_t) -> SliceType<dynamic_t>;
      template <auto min, auto max>
      SliceType(meta::ValueWrapper<min>, meta::ValueWrapper<max>) -> SliceType<interval<decltype(min), LowerBound<min>, UpperBound<max>>>;

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
        VK_inline constexpr static void fillDynamicExtents(const BasicExtents<I, E...>& ext, static_vector<I, dynamicRank>& dynExt, const Slice& slice) noexcept {
          if constexpr(same_as<decltype(Slice::staticExtent(std::get<N>(std::tuple{E...}))), dynamic_t>){
            if constexpr(BasicExtents<I, E...>::isStaticExtent(N)){
              dynExt.push_back(slice.extent(staticExtent<N>));
            } else {
              dynExt.push_back(slice.extent(ext.template extent<N>()));
            }
          }

        }


        template <size_t ...N>
        inline constexpr static type transform(std::index_sequence<N...>, const BasicExtents<I, E...>& ext, S&&... slices) noexcept {
          static_vector<I, dynamicRank> dynExt;
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
    }*//*



    *//*template <typename T, typename Ext, typename LayoutPolicy, typename AccessorPolicy, typename ...S>*//**//* requires(sizeof...(S) == Ext::rank())*//**//*
    constexpr auto view(const basic_array_view<T, Ext, LayoutPolicy, AccessorPolicy>& arrayView, detail::SliceType<S> ...slices) noexcept {
      using transformer_type = detail::TransformExtents<Ext, detail::SliceType<S>...>;
      using transformed_extents_type = typename transformer_type::type;
      using result_type = basic_array_view<T, transformed_extents_type, LayoutPolicy, AccessorPolicy>;
      return result_type {};
    }
    template <typename View, typename ...S>*//**//* requires(sizeof...(S) == Ext::rank())*//**//*
    constexpr auto view(const View& arrayView, S&& ...slices) noexcept{
      return view(arrayView, detail::SliceType{std::forward<S>(slices)}...);
    }*//*
  }




  template <typename T, extent_type auto ...Extents>
  using Array = Arrays::BasicArray<T, Arrays::Extents<Extents...>>;
  template <typename T, extent_type auto ...Extents>
  using ArrayView = Arrays::basic_array_view<T, Arrays::Extents<Extents...>>;*/



  /*static_assert(sizeof(ArrayView<float, 3, 4>) == sizeof(u64));


  void test(){

    ArrayView<float, 10, 4, 20> arrayView;
    auto result = view(arrayView, 4, _, interval{6, 10});
  }*/





}// namespace valkyrie

#endif//VALKYRIE_ARRAY_HPP





/*
#if defined(_ARRAY_)
#ifndef VALKYRIE_STL_COMPAT_ARRAY
#define VALKYRIE_STL_COMPAT_ARRAY

template <typename T, size_t N>
struct valkyrie::traits::static_array<std::array<T, N>>{
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
struct valkyrie::traits::DynamicArray<std::vector<T, Alloc>>{
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
struct valkyrie::traits::static_array<std::span<T, N>>{
  inline constexpr static T* getData(std::span<T, N> span) noexcept { return span.data(); }
  inline constexpr static size_t size = N;
};
template <typename T>
struct valkyrie::traits::DynamicArray<std::span<T>>{
  inline constexpr static T* getData(std::span<T> span) noexcept { return span.data(); }
  inline constexpr static size_t getSize(std::span<T> span) noexcept { return span.size(); }
};
template <typename T, size_t Ext>
struct valkyrie::traits::View<std::span<T, Ext>>{ };

#endif
#endif // defined(_SPAN_)*/
