//
// Created by maxwe on 2021-05-01.
//

#ifndef VALKYRIE_UTILITY_WAYMARKING_HPP
#define VALKYRIE_UTILITY_WAYMARKING_HPP


#include <valkyrie/traits.hpp>
#include <valkyrie/adt/pointer_int_pair.hpp>
namespace valkyrie{

  enum class waymark : u32{};

  namespace detail {

    template <u32 NumBits, u32 PrecomputedTags>
    struct waymarking_traits {

      VK_constant u32 bits = NumBits;
      VK_constant u32 mark_size = bits - 1;
      VK_constant u32 stop_mask = (1 << mark_size);
      VK_constant u32 mark_mask = stop_mask - 1;
      VK_constant u32 tag_mask  = stop_mask | mark_mask;

      VK_constant u32 precomputed_tag_count = PrecomputedTags;




      VK_forceinline static bool is_stop_mark(waymark mark) noexcept {
        return stop_mask & static_cast<u32>(mark);
      }
      VK_forceinline static bool is_initial_mark(waymark mark) noexcept {
        return static_cast<u32>(mark) == stop_mask;
      }
      VK_forceinline static u32  get_mark(waymark mark) noexcept {
        return static_cast<u32>(mark) & mark_mask;
      }
      VK_forceinline static void compute_offset(i64& offset, waymark mark) noexcept {
        offset = (offset << mark_size) + static_cast<u32>(mark);
      }

    private:

      template <u32 Len, bool Stop, u32 Count, u8... Vals>
      struct add_tag;
      template <u32 Len, u32 Count, u8... Vals>
      struct gen_tag{
        using type = typename add_tag<Len, (Count <= mark_mask), Count, Vals...>::type;
      };
      template <u32 Len, u8... Vals>
      struct gen_offset{
        using type = typename gen_tag<Len, sizeof...(Vals), Vals...>::type;
      };
      template <u32 Len, u32 Count, u8... Vals> requires (Len > 0)
      struct add_tag<Len, false, Count, Vals...>{
        using type = typename gen_tag<Len - 1, (Count >> mark_size), Vals..., Count & mark_mask>::type;
      };
      template <u32 Len, u32 Count, u8... Vals> requires (Len > 0)
      struct add_tag<Len, true, Count, Vals...>{
        using type = typename gen_offset<Len - 1, Vals..., (Count & mark_mask) | stop_mask>::type;
      };

      template <u32 Count, u8... Vals>
      struct tags_data{
        VK_constant u32 remain                  = Count;
        VK_constant u8  values[sizeof...(Vals)] = { Vals... };
      };

      template <bool Stop, u32 Count, u8 ... Vals>
      struct add_tag<0, Stop, Count, Vals...>{
        using type = tags_data<Count, Vals...>;
      };

    public:
      using tags = typename gen_offset<precomputed_tag_count>::type;

      VK_constant u32 remaining_count = tags::remain;

      VK_forceinline static waymark precomputed(u64 index) noexcept {
        return static_cast<waymark>(tags::values[index]);
      }
    };
  } // end namespace detail



  template <typename T>
  using waymarked_ptr = pointer_int_pair<T*, waymark>;

  template <typename T, u32 BitsAvailable = pointer_traits<T>::free_low_bits, u32 PrecomputedTags = 32>
  struct waymarker{
    using traits = detail::waymarking_traits<BitsAvailable, PrecomputedTags>;

    static void    set(T& n, waymark tag) noexcept { n.set_waymark(tag); }
    static waymark get(const T& n) noexcept { return n.get_waymark(); }
  };
  template <typename T, u32 Bits, u32 PreComp>
  struct waymarker<T*, Bits, PreComp>{
    using traits = detail::waymarking_traits<Bits, PreComp>;

    static void    set(T*& n, waymark tag) noexcept { reinterpret_cast<u64&>(n) |= static_cast<u64>(tag); }
    static waymark get(const T*  n) noexcept { return static_cast<waymark>(static_cast<u32>(reinterpret_cast<u64>(n)) & traits::tag_mask); }
  };

  template <typename T, u32 Bits, u32 PreComp>
  struct waymarker<waymarked_ptr<T>, Bits, PreComp>{
    using traits = detail::waymarking_traits<Bits, PreComp>;

    inline static void set(waymarked_ptr<T>& n, waymark tag) noexcept {
      n.set_integer(tag);
    }
    inline static waymark  get(const waymarked_ptr<T>& n) noexcept {
      return n.get_integer();
    }
  };


  /// Sets up the waymarking algorithm's tags for a given range [Begin, End).
  ///
  /// \param begin The beginning of the range to mark with tags (inclusive).
  /// \param end The ending of the range to mark with tags (exclusive).
  /// \param offset The position in the supposed tags array from which to start
  /// marking the given range.
  template <typename TIter,
            typename Marker   = waymarker<typename iterator_traits<TIter>::value_type>,
            typename Sentinel = TIter>
  requires (iterator_c<TIter> && std::sentinel_for<Sentinel, TIter>)
  void fill_waymarks(TIter begin, Sentinel end, u64 offset = 0) {
    if (begin == end)
      return;

    using marker = Marker;
    using traits = typename marker::traits;

    u32 count = traits::remaining_count;

    if ( offset <= traits::precomputed_tag_count ) {
    // Start by filling the pre-calculated tags, starting from the given offset.
      while ( offset != traits::precomputed_tag_count ) {
        marker::set(*begin, traits::precomputed(offset));

        ++offset;
        ++begin;

        if ( begin == end ){
          return;
        }

      }
    }
    else {
      u64 off = traits::precomputed_tag_count;
      do {
        ++off;
        u32 tag = count & traits::mark_mask;
        if (count <= traits::mark_mask) {
          tag |= traits::stop_mask;
          count = off;
        }
        else {
          count >>= traits::mark_size;
        }
      } while (off != offset);
    }

    do {
      ++offset;

      u32 tag = count & traits::mark_mask;
      if (count <= traits::mark_mask) {
        tag |= traits::stop_mask;
        count = offset;
      }
      else {
        count >>= traits::mark_size;
      }

      marker::set(*begin, static_cast<waymark>(tag));
      ++begin;
    } while (begin != end);
  }

  /// Sets up the waymarking algorithm's tags for a given range.
  ///
  /// \param range The range to mark with tags.
  /// \param offset The position in the supposed tags array from which to start
  /// marking the given range.
  template <typename R, class Marker = waymarker<typename range_traits<remove_ref_t<R>>::value_type>>
  void fill_waymarks(R && range, u64 offset = 0) {
    return fill_waymarks<decltype(std::begin(std::declval<R &>())), Marker>(
        std::ranges::begin(std::forward<R>(range)),
        std::ranges::end(std::forward<R>(range)),
        offset);
  }

  /// Retrieves the element marked with tag of only Marker::traits::stop_mask, by
  /// following the waymarks. This is the first element in a range passed to a
  /// previous call to \c fill_waymarks with \c offset 0.
  ///
  /// For the trivial usage of calling \c fill_waymarks(array), and \I is an
  /// iterator inside \c array, this function retrieves the head of \c Array, by
  /// following the waymarks.
  ///
  /// \param i The iterator into an array which was marked by the waymarking tags
  /// (by a previous call to \c fill_waymarks).
  template <typename TIter,
            typename Marker = waymarker<typename iterator_traits<TIter>::value_type>>
  requires iterator_c<TIter>
  TIter follow_waymarks(TIter i) {

    using marker = Marker;
    using traits = typename marker::traits;


    waymark mark;

    do
      mark = Marker::get(*i--);
    while (!traits::is_stop_mark(mark));

    if (!traits::is_initial_mark(mark)) VK_likely {
      i64 offset = traits::get_mark(mark);
      mark       = Marker::get(*i);
      for (; !traits::is_stop_mark(mark); mark = Marker::get(*--i))
        traits::compute_offset(offset, mark);
      i -= offset;
    }
    return ++i;
  }
}

#endif  //VALKYRIE_UTILITY_WAYMARKING_HPP
