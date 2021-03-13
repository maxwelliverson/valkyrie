//
// Created by maxwe on 2021-03-04.
//

#ifndef VALKYRIE_ADT_SYMBOL_HPP
#define VALKYRIE_ADT_SYMBOL_HPP

#include <valkyrie/adt/nothing.hpp>
#include <valkyrie/adt/string_view.hpp>
#include <valkyrie/memory/default_allocator.hpp>

#include "iterator.hpp"

namespace valkyrie {
  class symbol_manager {};

  namespace detail {
    /// symbol_registry_entry_base - Shared base class of symbol_registry_entry instances.
    class symbol_registry_entry_base {
      u64 keyLength;

    public:
      explicit symbol_registry_entry_base(u64 keyLength) : keyLength(keyLength) {}

      VK_nodiscard u64 get_key_length() const { return keyLength; }
    };

    /// symbol_registry_entry_storage - Holds the value in a symbol_registry_entry.
    ///
    /// Factored out into a separate base class to make it easier to specialize.
    /// This is primarily intended to support StringSet, which doesn't need a value
    /// stored at all.
    template<typename ValueTy>
    class symbol_registry_entry_storage : public symbol_registry_entry_base {
    public:
      ValueTy second;

      explicit symbol_registry_entry_storage(size_t keyLength)
          : symbol_registry_entry_base(keyLength), second() {}
      template<typename... InitTy>
      symbol_registry_entry_storage(size_t keyLength, InitTy &&...initVals)
          : symbol_registry_entry_base(keyLength),
            second(std::forward<InitTy>(initVals)...) {}
      symbol_registry_entry_storage(symbol_registry_entry_storage &e) = delete;

      const ValueTy &get() const { return second; }
      ValueTy &get() { return second; }

      void set(const ValueTy &V) { second = V; }
    };

    template<>
    class symbol_registry_entry_storage<nothing_type> : public symbol_registry_entry_base {
    public:
      explicit symbol_registry_entry_storage(size_t keyLength, nothing_type none = nothing)
          : symbol_registry_entry_base(keyLength) {}
      symbol_registry_entry_storage(symbol_registry_entry_storage &entry) = delete;

      VK_nodiscard nothing_type get() const { return nothing; }
    };

    /// symbol_registry_entry - This is used to represent one value that is inserted into
    /// a symbol_map.  It contains the Value itself and the key: the string length
    /// and data.
    template<typename ValueTy>
    class symbol_registry_entry final : public symbol_registry_entry_storage<ValueTy> {
    public:
      using symbol_registry_entry_storage<ValueTy>::symbol_registry_entry_storage;

      string_view key() const {
        return string_view(get_key_data(), this->get_key_length());
      }

      /// get_key_data - Return the start of the string data that is the key for this
      /// value.  The string data is always stored immediately after the
      /// symbol_registry_entry object.
      utf8_string get_key_data() const {
        return reinterpret_cast<const utf8 *>(this + 1);
      }

      string_view first() const {
        return key();
      }

      /// Create a symbol_registry_entry for the specified key construct the value using
      /// \p InitiVals.
      template<typename AllocatorTy, typename... InitTy>
      static symbol_registry_entry *create(string_view key, AllocatorTy &allocator,
                                           InitTy &&...initVals) {
        size_t keyLength = key.size();

        // Allocate a new item with space for the string at the end and a null
        // terminator.
        size_t allocSize = sizeof(symbol_registry_entry) + keyLength + 1;
        size_t alignment = alignof(symbol_registry_entry);

        auto *newItem = static_cast<symbol_registry_entry *>(allocator.allocate_node(allocSize, alignment));
        assert(newItem && "Unhandled out-of-memory");

        // Construct the value.
        new (newItem) symbol_registry_entry(keyLength, std::forward<InitTy>(initVals)...);

        // Copy the string information.
        utf8 *strBuffer = const_cast<utf8 *>(newItem->get_key_data());
        if (keyLength > 0)
          std::memcpy(strBuffer, key.data(), keyLength);
        strBuffer[keyLength] = 0;// Null terminate for convenience of clients.
        return newItem;
      }

      /// Getsymbol_registry_entryFromKeyData - Given key data that is known to be embedded
      /// into a symbol_registry_entry, return the symbol_registry_entry itself.
      static symbol_registry_entry &get_symbol_registry_entry_from_key_data(const utf8 *keyData) {
        utf8 *ptr = const_cast<utf8 *>(keyData) - sizeof(symbol_registry_entry<ValueTy>);
        return *reinterpret_cast<symbol_registry_entry *>(ptr);
      }

      /// Destroy - Destroy this symbol_registry_entry, releasing memory back to the
      /// specified allocator.
      template<typename AllocatorTy>
      void destroy(AllocatorTy &allocator) {
        // Free memory referenced by the item.
        size_t AllocSize = sizeof(symbol_registry_entry) + this->get_key_length() + 1;
        this->~symbol_registry_entry();
        allocator.deallocate_node(static_cast<void *>(this), AllocSize, alignof(symbol_registry_entry));
      }
    };
  }// namespace detail

  template<typename ValueTy>
  class symbol_map_const_iterator;
  template<typename ValueTy>
  class symbol_map_iterator;
  template<typename ValueTy>
  class symbol_map_key_iterator;

  /// symbol_registry_impl - This is the base class of symbol_map that is shared among
  /// all of its instantiations.
  class symbol_registry_impl {
  protected:
    // Array of NumBuckets pointers to entries, null pointers are holes.
    // TheTable[NumBuckets] contains a sentinel value for easy iteration. Followed
    // by an array of the actual hash values as unsigned integers.
    detail::symbol_registry_entry_base **TheTable = nullptr;
    unsigned NumBuckets = 0;
    unsigned NumItems = 0;
    unsigned NumTombstones = 0;
    unsigned ItemSize;

  protected:
    explicit symbol_registry_impl(unsigned itemSize) : ItemSize(itemSize) {}
    symbol_registry_impl(symbol_registry_impl &&RHS)
        : TheTable(RHS.TheTable), NumBuckets(RHS.NumBuckets),
          NumItems(RHS.NumItems), NumTombstones(RHS.NumTombstones),
          ItemSize(RHS.ItemSize) {
      RHS.TheTable = nullptr;
      RHS.NumBuckets = 0;
      RHS.NumItems = 0;
      RHS.NumTombstones = 0;
    }

    symbol_registry_impl(unsigned InitSize, unsigned ItemSize);
    unsigned rehash_table(unsigned BucketNo = 0);

    /// lookup_bucket_for - Look up the bucket that the specified string should end
    /// up in.  If it already exists as a key in the map, the Item pointer for the
    /// specified bucket will be non-null.  Otherwise, it will be null.  In either
    /// case, the FullHashValue field of the bucket will be set to the hash value
    /// of the string.
    unsigned lookup_bucket_for(string_view Key);

    /// find_key - Look up the bucket that contains the specified key. If it exists
    /// in the map, return the bucket number of the key.  Otherwise return -1.
    /// This does not modify the map.
    int find_key(string_view Key) const;

    /// remove_key - Remove the specified detail::symbol_registry_entry from the table, but do not
    /// delete it.  This aborts if the value isn't in the table.
    void remove_key(detail::symbol_registry_entry_base *V);

    /// remove_key - Remove the detail::symbol_registry_entry for the specified key from the
    /// table, returning it.  If the key is not in the table, this returns null.
    detail::symbol_registry_entry_base *remove_key(string_view Key);

    /// Allocate the table with the specified number of buckets and otherwise
    /// setup the map as empty.
    void init(unsigned Size);

  public:
    static detail::symbol_registry_entry_base *get_tombstone_val() {

      constexpr static u32 LowBitsAvailable = 2;
      uintptr_t Val = static_cast<uintptr_t>(-1);
      Val <<= LowBitsAvailable;
      return reinterpret_cast<detail::symbol_registry_entry_base *>(Val);
    }

    unsigned get_num_buckets() const { return NumBuckets; }
    unsigned get_num_items() const { return NumItems; }

    bool empty() const { return NumItems == 0; }
    unsigned size() const { return NumItems; }

    void swap(symbol_registry_impl &Other) {
      std::swap(TheTable, Other.TheTable);
      std::swap(NumBuckets, Other.NumBuckets);
      std::swap(NumItems, Other.NumItems);
      std::swap(NumTombstones, Other.NumTombstones);
    }
  };

  /// symbol_map - This is an unconventional map that is specialized for handling
  /// keys that are "strings", which are basically ranges of bytes. This does some
  /// funky memory allocation and hashing things to make it extremely efficient,
  /// storing the string data *after* the value in the map.
  template<typename ValueTy, raw_allocator Alloc = default_allocator>
  class symbol_map : public symbol_registry_impl {

    using alloc_state = typename allocator_traits<Alloc>::allocator_type;

    alloc_state Allocator;

  public:
    using MapEntryTy = detail::symbol_registry_entry<ValueTy>;

    symbol_map() : symbol_registry_impl(static_cast<unsigned>(sizeof(MapEntryTy))) {}

    explicit symbol_map(unsigned InitialSize)
        : symbol_registry_impl(InitialSize, static_cast<unsigned>(sizeof(MapEntryTy))) {}

    explicit symbol_map(alloc_state A)
        : symbol_registry_impl(static_cast<unsigned>(sizeof(MapEntryTy))), Allocator(A) {
    }

    symbol_map(unsigned InitialSize, alloc_state A)
        : symbol_registry_impl(InitialSize, static_cast<unsigned>(sizeof(MapEntryTy))),
          Allocator(A) {}

    symbol_map(std::initializer_list<std::pair<string_view, ValueTy>> List)
        : symbol_registry_impl(List.size(), static_cast<unsigned>(sizeof(MapEntryTy))) {
      for (const auto &P : List) {
        insert(P);
      }
    }

    symbol_map(symbol_map &&RHS)
        : symbol_registry_impl(std::move(RHS)), Allocator(std::move(RHS.Allocator)) {}

    symbol_map(const symbol_map &RHS)
        : symbol_registry_impl(static_cast<unsigned>(sizeof(MapEntryTy))),
          Allocator(RHS.Allocator) {
      if (RHS.empty())
        return;

      // Allocate TheTable of the same size as RHS's TheTable, and set the
      // sentinel appropriately (and NumBuckets).
      init(RHS.NumBuckets);
      unsigned *HashTable = (unsigned *) (TheTable + NumBuckets + 1),
               *RHSHashTable = (unsigned *) (RHS.TheTable + NumBuckets + 1);

      NumItems = RHS.NumItems;
      NumTombstones = RHS.NumTombstones;
      for (unsigned I = 0, E = NumBuckets; I != E; ++I) {
        detail::symbol_registry_entry_base *Bucket = RHS.TheTable[I];
        if (!Bucket || Bucket == get_tombstone_val()) {
          TheTable[I] = Bucket;
          continue;
        }

        TheTable[I] = MapEntryTy::Create(
            static_cast<MapEntryTy *>(Bucket)->getKey(), Allocator,
            static_cast<MapEntryTy *>(Bucket)->getValue());
        HashTable[I] = RHSHashTable[I];
      }

      // Note that here we've copied everything from the RHS into this object,
      // tombstones included. We could, instead, have re-probed for each key to
      // instantiate this new object without any tombstone buckets. The
      // assumption here is that items are rarely deleted from most symbol_maps,
      // and so tombstones are rare, so the cost of re-probing for all inputs is
      // not worthwhile.
    }

    symbol_map &operator=(symbol_map RHS) {
      symbol_registry_impl::swap(RHS);
      std::swap(Allocator, RHS.Allocator);
      return *this;
    }

    ~symbol_map() {
      // Delete all the elements in the map, but don't reset the elements
      // to default values.  This is a copy of clear(), but avoids unnecessary
      // work not required in the destructor.
      if (!empty()) {
        for (unsigned I = 0, E = NumBuckets; I != E; ++I) {
          detail::symbol_registry_entry_base *Bucket = TheTable[I];
          if (Bucket && Bucket != get_tombstone_val()) {
            static_cast<MapEntryTy *>(Bucket)->Destroy(Allocator);
          }
        }
      }
      free(TheTable);
    }

    alloc_state &getAllocator() { return Allocator; }
    const alloc_state &getAllocator() const { return Allocator; }

    using key_type = const char *;
    using mapped_type = ValueTy;
    using value_type = detail::symbol_registry_entry<ValueTy>;
    using size_type = size_t;

    using const_iterator = symbol_map_const_iterator<ValueTy>;
    using iterator = symbol_map_iterator<ValueTy>;

    iterator begin() { return iterator(TheTable, NumBuckets == 0); }
    iterator end() { return iterator(TheTable + NumBuckets, true); }
    const_iterator begin() const {
      return const_iterator(TheTable, NumBuckets == 0);
    }
    const_iterator end() const {
      return const_iterator(TheTable + NumBuckets, true);
    }

    iterator_range<symbol_map_key_iterator<ValueTy>> keys() const {
      return make_range(symbol_map_key_iterator<ValueTy>(begin()),
                        symbol_map_key_iterator<ValueTy>(end()));
    }

    iterator find(string_view Key) {
      int Bucket = find_key(Key);
      if (Bucket == -1)
        return end();
      return iterator(TheTable + Bucket, true);
    }

    const_iterator find(string_view Key) const {
      int Bucket = find_key(Key);
      if (Bucket == -1)
        return end();
      return const_iterator(TheTable + Bucket, true);
    }

    /// lookup - Return the entry for the specified key, or a default
    /// constructed value if no such entry exists.
    ValueTy lookup(string_view Key) const {
      const_iterator it = find(Key);
      if (it != end())
        return it->second;
      return ValueTy();
    }

    /// Lookup the ValueTy for the \p Key, or create a default constructed value
    /// if the key is not in the map.
    ValueTy &operator[](string_view Key) { return try_emplace(Key).first->second; }

    /// count - Return 1 if the element is in the map, 0 otherwise.
    size_type count(string_view Key) const { return find(Key) == end() ? 0 : 1; }

    template<typename InputTy>
    size_type count(const detail::symbol_registry_entry<InputTy> &MapEntry) const {
      return count(MapEntry.getKey());
    }

    /// equal - check whether both of the containers are equal.
    bool operator==(const symbol_map &RHS) const {
      if (size() != RHS.size())
        return false;

      for (const auto &KeyValue : *this) {
        auto FindInRHS = RHS.find(KeyValue.getKey());

        if (FindInRHS == RHS.end())
          return false;

        if (!(KeyValue.get() == FindInRHS->get()))
          return false;
      }

      return true;
    }

    /// insert - Insert the specified key/value pair into the map.  If the key
    /// already exists in the map, return false and ignore the request, otherwise
    /// insert it and return true.
    bool insert(MapEntryTy *KeyValue) {
      unsigned BucketNo = lookup_bucket_for(KeyValue->getKey());
      detail::symbol_registry_entry_base *&Bucket = TheTable[BucketNo];
      if (Bucket && Bucket != get_tombstone_val())
        return false;// Already exists in map.

      if (Bucket == get_tombstone_val())
        --NumTombstones;
      Bucket = KeyValue;
      ++NumItems;
      assert(NumItems + NumTombstones <= NumBuckets);

      rehash_table();
      return true;
    }

    /// insert - Inserts the specified key/value pair into the map if the key
    /// isn't already in the map. The bool component of the returned pair is true
    /// if and only if the insertion takes place, and the iterator component of
    /// the pair points to the element with key equivalent to the key of the pair.
    std::pair<iterator, bool> insert(std::pair<string_view, ValueTy> KV) {
      return try_emplace(KV.first, std::move(KV.second));
    }

    /// Inserts an element or assigns to the current element if the key already
    /// exists. The return type is the same as try_emplace.
    template<typename V>
    std::pair<iterator, bool> insert_or_assign(string_view Key, V &&Val) {
      auto Ret = try_emplace(Key, std::forward<V>(Val));
      if (!Ret.second)
        Ret.first->second = std::forward<V>(Val);
      return Ret;
    }

    /// Emplace a new element for the specified key into the map if the key isn't
    /// already in the map. The bool component of the returned pair is true
    /// if and only if the insertion takes place, and the iterator component of
    /// the pair points to the element with key equivalent to the key of the pair.
    template<typename... ArgsTy>
    std::pair<iterator, bool> try_emplace(string_view Key, ArgsTy &&...Args) {
      unsigned BucketNo = lookup_bucket_for(Key);
      detail::symbol_registry_entry_base *&Bucket = TheTable[BucketNo];
      if (Bucket && Bucket != get_tombstone_val())
        return std::make_pair(iterator(TheTable + BucketNo, false),
                              false);// Already exists in map.

      if (Bucket == get_tombstone_val())
        --NumTombstones;
      Bucket = MapEntryTy::Create(Key, Allocator, std::forward<ArgsTy>(Args)...);
      ++NumItems;
      assert(NumItems + NumTombstones <= NumBuckets);

      BucketNo = rehash_table(BucketNo);
      return std::make_pair(iterator(TheTable + BucketNo, false), true);
    }

    // clear - Empties out the symbol_map
    void clear() {
      if (empty())
        return;

      // Zap all values, resetting the keys back to non-present (not tombstone),
      // which is safe because we're removing all elements.
      for (unsigned I = 0, E = NumBuckets; I != E; ++I) {
        detail::symbol_registry_entry_base *&Bucket = TheTable[I];
        if (Bucket && Bucket != get_tombstone_val()) {
          static_cast<MapEntryTy *>(Bucket)->Destroy(Allocator);
        }
        Bucket = nullptr;
      }

      NumItems = 0;
      NumTombstones = 0;
    }

    /// remove - Remove the specified key/value pair from the map, but do not
    /// erase it.  This aborts if the key is not in the map.
    void remove(MapEntryTy *KeyValue) { remove_key(KeyValue); }

    void erase(iterator I) {
      MapEntryTy &V = *I;
      remove(&V);
      V.Destroy(Allocator);
    }

    bool erase(string_view Key) {
      iterator I = find(Key);
      if (I == end())
        return false;
      erase(I);
      return true;
    }
  };

  template<typename DerivedTy, typename ValueTy>
  class symbol_map_iter_base
      : public iterator_facade_base<DerivedTy, std::forward_iterator_tag,
                                    ValueTy> {
  protected:
    detail::symbol_registry_entry_base **Ptr = nullptr;

  public:
    symbol_map_iter_base() = default;

    explicit symbol_map_iter_base(detail::symbol_registry_entry_base **Bucket,
                                bool NoAdvance = false)
        : Ptr(Bucket) {
      if (!NoAdvance)
        AdvancePastEmptyBuckets();
    }

    DerivedTy &operator=(const DerivedTy &Other) {
      Ptr = Other.Ptr;
      return static_cast<DerivedTy &>(*this);
    }

    bool operator==(const DerivedTy &RHS) const { return Ptr == RHS.Ptr; }

    DerivedTy &operator++() {// Preincrement
      ++Ptr;
      AdvancePastEmptyBuckets();
      return static_cast<DerivedTy &>(*this);
    }

    DerivedTy operator++(int) {// Post-increment
      DerivedTy Tmp(Ptr);
      ++*this;
      return Tmp;
    }

  private:
    void AdvancePastEmptyBuckets() {
      while (*Ptr == nullptr || *Ptr == symbol_registry_impl::get_tombstone_val())
        ++Ptr;
    }
  };

  template<typename ValueTy>
  class symbol_map_const_iterator
      : public symbol_map_iter_base<symbol_map_const_iterator<ValueTy>,
                                  const detail::symbol_registry_entry<ValueTy>> {
    using base = symbol_map_iter_base<symbol_map_const_iterator<ValueTy>,
                                    const detail::symbol_registry_entry<ValueTy>>;

  public:
    symbol_map_const_iterator() = default;
    explicit symbol_map_const_iterator(detail::symbol_registry_entry_base **Bucket,
                                     bool NoAdvance = false)
        : base(Bucket, NoAdvance) {}

    const detail::symbol_registry_entry<ValueTy> &operator*() const {
      return *static_cast<const detail::symbol_registry_entry<ValueTy> *>(*this->Ptr);
    }
  };

  template<typename ValueTy>
  class symbol_map_iterator : public symbol_map_iter_base<symbol_map_iterator<ValueTy>,
                                                       detail::symbol_registry_entry<ValueTy>> {
    using base =
        symbol_map_iter_base<symbol_map_iterator<ValueTy>, detail::symbol_registry_entry<ValueTy>>;

  public:
    symbol_map_iterator() = default;
    explicit symbol_map_iterator(detail::symbol_registry_entry_base **Bucket,
                                bool NoAdvance = false)
        : base(Bucket, NoAdvance) {}

    detail::symbol_registry_entry<ValueTy> &operator*() const {
      return *static_cast<detail::symbol_registry_entry<ValueTy> *>(*this->Ptr);
    }

    operator symbol_map_const_iterator<ValueTy>() const {
      return symbol_map_const_iterator<ValueTy>(this->Ptr, true);
    }
  };

  template<typename ValueTy>
  class symbol_map_key_iterator
      : public iterator_adaptor_base<symbol_map_key_iterator<ValueTy>,
                                     symbol_map_const_iterator<ValueTy>,
                                     std::forward_iterator_tag, string_view> {
    using base = iterator_adaptor_base<symbol_map_key_iterator<ValueTy>,
                                       symbol_map_const_iterator<ValueTy>,
                                       std::forward_iterator_tag, string_view>;

  public:
    symbol_map_key_iterator() = default;
    explicit symbol_map_key_iterator(symbol_map_const_iterator<ValueTy> Iter)
        : base(std::move(Iter)) {}

    string_view &operator*() {
      Key = this->wrapped()->getKey();
      return Key;
    }

  private:
    string_view Key;
  };


  /// StringSet - A wrapper for StringMap that provides set-like functionality.
  template <raw_allocator AllocatorTy = default_allocator>
  class symbol_set : public symbol_map<nothing_type, AllocatorTy> {
    using Base = symbol_map<nothing_type, AllocatorTy>;

  public:
    symbol_set() = default;
    symbol_set(std::initializer_list<string_view> initializer) {
      for (string_view str : initializer)
        insert(str);
    }
    explicit symbol_set(AllocatorTy a) : Base(a) {}

    std::pair<typename Base::iterator, bool> insert(string_view key) {
      return Base::try_emplace(key);
    }

    template <typename InputIt>
    void insert(const InputIt &begin, const InputIt &end) {
      for (auto it = begin; it != end; ++it)
        insert(*it);
    }

    template <typename ValueTy>
    std::pair<typename Base::iterator, bool>
    insert(const detail::symbol_registry_entry<ValueTy> &mapEntry) {
      return insert(mapEntry.getKey());
    }
  };



  class symbol {};
  class symbol_registry {};
}// namespace valkyrie

#endif//VALKYRIE_ADT_SYMBOL_HPP
