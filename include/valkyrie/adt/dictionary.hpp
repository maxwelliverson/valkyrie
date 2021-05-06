//
// Created by maxwe on 2021-05-02.
//

#ifndef VALKYRIE_ADT_DICTIONARY_HPP
#define VALKYRIE_ADT_DICTIONARY_HPP

#include <valkyrie/adt/string_view.hpp>
#include <valkyrie/adt/iterator.hpp>
#include <valkyrie/memory/default_allocator.hpp>

#include <initializer_list>

namespace valkyrie{
  
  namespace impl{
    class dictionary_entry_base {
      u64 keyLength;

    public:
      explicit dictionary_entry_base(u64 keyLength) : keyLength(keyLength) {}

      VK_nodiscard u64 get_key_length() const { return keyLength; }
    };
    
    template <typename Val>
    class dictionary_entry_storage : public dictionary_entry_base{
      Val second;
    public:
      

      explicit dictionary_entry_storage(size_t keyLength)
      : dictionary_entry_base(keyLength), second() {}
      template<typename... InitTy>
      dictionary_entry_storage(size_t keyLength, InitTy &&...initVals)
      : dictionary_entry_base(keyLength),
      second(std::forward<InitTy>(initVals)...) {}
      dictionary_entry_storage(dictionary_entry_storage &e) = delete;

      const Val &get() const { return second; }
      Val &get() { return second; }

      void set(const Val &V) { second = V; }
    };

    template<>
    class dictionary_entry_storage<nothing_t> : public dictionary_entry_base {
    public:
      explicit dictionary_entry_storage(size_t keyLength, nothing_t none = nothing)
          : dictionary_entry_base(keyLength) {}
      dictionary_entry_storage(dictionary_entry_storage &entry) = delete;

      VK_nodiscard nothing_t get() const { return nothing; }
    };


    class dictionary {

      using PFN_allocate_table = dictionary_entry_base**(*)(u64, void*) noexcept;
      using PFN_deallocate_table = void(*)(dictionary_entry_base**, u64, void*) noexcept;

    protected:

      template <raw_allocator Alloc>
      VK_constant auto allocate_table_fn = [](u64 size, void* state) noexcept {
        using traits         = allocator_traits<Alloc>;
        using allocator_type = typename traits::allocator_type;
        return static_cast<dictionary_entry_base**>(traits::allocate_array(*static_cast<allocator_type*>(state), size + 1, sizeof(void*) + sizeof(u32), detail::max_alignment));
      };
      template <raw_allocator Alloc>
      VK_constant auto deallocate_table_fn = [](dictionary_entry_base** table, u64 size, void* state) noexcept {
        using traits         = allocator_traits<Alloc>;
        using allocator_type = typename traits::allocator_type;
        traits::deallocate_array(*static_cast<allocator_type*>(state), table, size + 1, sizeof(void*) + sizeof(u32), detail::max_alignment);
      };


      // Array of NumBuckets pointers to entries, null pointers are holes.
      // TheTable[NumBuckets] contains a sentinel value for easy iteration. Followed
      // by an array of the actual hash values as unsigned integers.
      dictionary_entry_base** TheTable      = nullptr;
      u32                     NumBuckets    = 0;
      u32                     NumItems      = 0;
      u32                     NumTombstones = 0;
      u32                     ItemSize;

    protected:
      explicit dictionary(unsigned itemSize) noexcept : ItemSize(itemSize) {}
      dictionary(dictionary &&RHS) noexcept
          : TheTable(RHS.TheTable),
            NumBuckets(RHS.NumBuckets),
            NumItems(RHS.NumItems),
            NumTombstones(RHS.NumTombstones),
            ItemSize(RHS.ItemSize) {
        RHS.TheTable = nullptr;
        RHS.NumBuckets = 0;
        RHS.NumItems = 0;
        RHS.NumTombstones = 0;
      }

      /// Allocate the table with the specified number of buckets and otherwise
      /// setup the map as empty.
      void init(u32 Size, PFN_allocate_table, void* allocator_state);
      void explicit_init(u32 size, PFN_allocate_table, void* allocator_state) noexcept;

      u32  rehash_table(u32 BucketNo, PFN_allocate_table, PFN_deallocate_table, void* allocator_state);

      /// lookup_bucket_for - Look up the bucket that the specified string should end
      /// up in.  If it already exists as a key in the map, the Item pointer for the
      /// specified bucket will be non-null.  Otherwise, it will be null.  In either
      /// case, the FullHashValue field of the bucket will be set to the hash value
      /// of the string.
      unsigned lookup_bucket_for(string_view Key, PFN_allocate_table, void* allocator_state);

      /// find_key - Look up the bucket that contains the specified key. If it exists
      /// in the map, return the bucket number of the key.  Otherwise return -1.
      /// This does not modify the map.
      VK_nodiscard i32 find_key(string_view Key) const;

      /// remove_key - Remove the specified dictionary_entry from the table, but do not
      /// delete it.  This aborts if the value isn't in the table.
      void remove_key(dictionary_entry_base *V);

      /// remove_key - Remove the dictionary_entry for the specified key from the
      /// table, returning it.  If the key is not in the table, this returns null.
      dictionary_entry_base *remove_key(string_view Key);



    public:
      inline static dictionary_entry_base* get_tombstone_val() noexcept {
        constexpr static u32 free_low_bits = pointer_traits<dictionary_entry_base*>::free_low_bits;
        uintptr_t Val = static_cast<uintptr_t>(-1);
        Val <<= free_low_bits;
        return reinterpret_cast<dictionary_entry_base *>(Val);
      }

      VK_nodiscard u32  get_num_buckets() const noexcept { return NumBuckets; }
      VK_nodiscard u32  get_num_items() const noexcept { return NumItems; }

      VK_nodiscard bool empty() const noexcept { return NumItems == 0; }
      VK_nodiscard u32  size() const noexcept { return NumItems; }

      void swap(dictionary& other) noexcept {
        std::swap(TheTable, other.TheTable);
        std::swap(NumBuckets, other.NumBuckets);
        std::swap(NumItems, other.NumItems);
        std::swap(NumTombstones, other.NumTombstones);
      }
    };

    template<typename DerivedTy, typename T>
    class dictionary_iter_base : public iterator_facade_base<DerivedTy, std::forward_iterator_tag, T> {
    protected:
      impl::dictionary_entry_base** Ptr = nullptr;

      using derived_type = DerivedTy;

    public:
      dictionary_iter_base() = default;

      explicit dictionary_iter_base(impl::dictionary_entry_base** Bucket, bool NoAdvance = false)
          : Ptr(Bucket) {
        if (!NoAdvance)
          AdvancePastEmptyBuckets();
      }

      derived_type& operator=(const derived_type& Other) {
        Ptr = Other.Ptr;
        return static_cast<derived_type &>(*this);
      }



      derived_type &operator++() {// Preincrement
        ++Ptr;
        AdvancePastEmptyBuckets();
        return static_cast<derived_type &>(*this);
      }

      derived_type operator++(int) {// Post-increment
        derived_type Tmp(Ptr);
        ++*this;
        return Tmp;
      }


      friend bool operator==(const derived_type& LHS, const derived_type &RHS) noexcept {
        return LHS.Ptr == RHS.Ptr;
      }

    private:
      void AdvancePastEmptyBuckets() {
        while (*Ptr == nullptr || *Ptr == impl::dictionary::get_tombstone_val())
          ++Ptr;
      }
    };

  }

  template<typename ValueTy>
  class dictionary_const_iterator;
  template<typename ValueTy>
  class dictionary_iterator;
  template<typename ValueTy>
  class dictionary_key_iterator;
  
  
  

  template<typename Val>
  class dictionary_entry final : public impl::dictionary_entry_storage<Val> {
  public:
    using impl::dictionary_entry_storage<Val>::dictionary_entry_storage;

    VK_nodiscard string_view key() const {
      return string_view(get_key_data(), this->get_key_length());
    }

    /// get_key_data - Return the start of the string data that is the key for this
    /// value.  The string data is always stored immediately after the
    /// dictionary_entry object.
    VK_nodiscard utf8_string get_key_data() const {
      return reinterpret_cast<const utf8 *>(this + 1);
    }

    /// Create a dictionary_entry for the specified key construct the value using
    /// \p InitiVals.
    template<typename Alloc, typename... Args>
    static dictionary_entry* create(string_view key,
                                    typename allocator_traits<Alloc>::allocator_type& allocator,
                                    Args&& ...args) {
      size_t keyLength = key.length();

      // Allocate a new item with space for the string at the end and a null
      // terminator.
      size_t allocSize = sizeof(dictionary_entry) + keyLength + 1;
      size_t alignment = alignof(dictionary_entry);

      auto *newItem = static_cast<dictionary_entry*>(allocator_traits<Alloc>::allocate_node(allocator, allocSize, alignment));
      VK_assert_msg(newItem, "Unhandled out-of-memory");

      // Construct the value.
      new (newItem) dictionary_entry(keyLength, std::forward<Args>(args)...);

      // Copy the string information.
      utf8 *strBuffer = const_cast<utf8 *>(newItem->get_key_data());
      if (keyLength > 0)
        std::memcpy(strBuffer, key.data(), keyLength);
      strBuffer[keyLength] = 0;// Null terminate for convenience of clients.
      return newItem;
    }

    /// Getdictionary_entryFromKeyData - Given key data that is known to be embedded
    /// into a dictionary_entry, return the dictionary_entry itself.
    static dictionary_entry& get_dictionary_entry_from_key_data(utf8_string keyData) {
      utf8 *ptr = const_cast<utf8 *>(keyData) - sizeof(dictionary_entry<Val>);
      return *reinterpret_cast<dictionary_entry *>(ptr);
    }

    /// Destroy - Destroy this dictionary_entry, releasing memory back to the
    /// specified allocator.
    template<typename Alloc>
    void destroy(typename allocator_traits<Alloc>::allocator_type& allocator) {
      // Free memory referenced by the item.
      size_t AllocSize = sizeof(dictionary_entry) + this->get_key_length() + 1;
      this->~dictionary_entry();
      allocator_traits<Alloc>::deallocate_node(allocator, this, AllocSize, alignof(dictionary_entry));
    }
  };
  
  template <typename T, raw_allocator Alloc = default_allocator>
  class dictionary : public impl::dictionary {

    using alloc_traits = allocator_traits<Alloc>;
    using allocator_state = typename alloc_traits::allocator_type;

  public:
    using entry_type = dictionary_entry<T>;

    dictionary() noexcept : impl::dictionary(narrow_cast<u32>(sizeof(entry_type))) {}

    explicit dictionary(u32 InitialSize)
        : impl::dictionary(narrow_cast<u32>(sizeof(entry_type))), allocator{} {
      this->explicit_init(InitialSize, allocate_table_fn<Alloc>, &allocator);
    }

    explicit dictionary(allocator_state A)
        : impl::dictionary(narrow_cast<u32>(sizeof(entry_type))),
          allocator(std::move(A)) { }

    dictionary(unsigned InitialSize, allocator_state A)
        : impl::dictionary(narrow_cast<u32>(sizeof(entry_type))),
          allocator(std::move(A)) {
      this->explicit_init(InitialSize, allocate_table_fn<Alloc>, &allocator);
    }

    dictionary(std::initializer_list<std::pair<string_view, T>> List)
        : impl::dictionary(narrow_cast<u32>(sizeof(entry_type))),
          allocator{}{
      this->explicit_init(List.size(), allocate_table_fn<Alloc>, &allocator);
      for (const auto &P : List) {
        insert(P);
      }
    }

    dictionary(dictionary &&RHS) noexcept
        : impl::dictionary(std::move(RHS)),
          allocator(std::move(RHS.allocator)) {}

    dictionary(const dictionary &RHS)
        : impl::dictionary(narrow_cast<u32>(sizeof(entry_type))),
          allocator(RHS.allocator) {
      if (RHS.empty())
        return;

      // Allocate TheTable of the same size as RHS's TheTable, and set the
      // sentinel appropriately (and NumBuckets).
      this->init(RHS.NumBuckets, allocate_table_fn<Alloc>, &allocator);
      auto* HashTable    = (u32*)(TheTable + NumBuckets + 1);
      auto* RHSHashTable = (u32*)(RHS.TheTable + NumBuckets + 1);

      NumItems = RHS.NumItems;
      NumTombstones = RHS.NumTombstones;

      for (u32 I = 0, E = NumBuckets; I != E; ++I) {
        impl::dictionary_entry_base *Bucket = RHS.TheTable[I];
        if (!Bucket || Bucket == get_tombstone_val()) {
          TheTable[I] = Bucket;
          continue;
        }

        TheTable[I] = entry_type::template create<Alloc>(
            static_cast<entry_type *>(Bucket)->key(),
            allocator,
            static_cast<entry_type *>(Bucket)->getValue());
        HashTable[I] = RHSHashTable[I];
      }
    }

    dictionary& operator=(dictionary&& RHS) noexcept {
      impl::dictionary::swap(RHS);
      std::swap(allocator, RHS.allocator);
      return *this;
    }

    ~dictionary() {
      // Delete all the elements in the map, but don't reset the elements
      // to default values.  This is a copy of clear(), but avoids unnecessary
      // work not required in the destructor.
      if (!empty()) {
        for (unsigned I = 0, E = NumBuckets; I != E; ++I) {
          impl::dictionary_entry_base *Bucket = TheTable[I];
          if (Bucket && Bucket != get_tombstone_val()) {
            static_cast<entry_type *>(Bucket)->template destroy<Alloc>(allocator);
          }
        }
      }
      deallocate_table_fn<Alloc>(TheTable, NumBuckets, &allocator);
    }

    allocator_state& get_allocator() noexcept { return allocator; }
    const allocator_state& get_allocator() const noexcept { return allocator; }

    using key_type = utf8_string;
    using mapped_type = T;
    using value_type = dictionary_entry<T>;
    using size_type = size_t;

    using const_iterator = dictionary_const_iterator<T>;
    using iterator = dictionary_iterator<T>;

    iterator begin() noexcept { return iterator(TheTable, NumBuckets == 0); }
    iterator end() noexcept { return iterator(TheTable + NumBuckets, true); }
    const_iterator begin() const noexcept {
      return const_iterator(TheTable, !NumBuckets);
    }
    const_iterator end() const noexcept {
      return const_iterator(TheTable + NumBuckets, true);
    }

    range_view<dictionary_key_iterator<T>> keys() const noexcept {
      return { dictionary_key_iterator<T>(begin()),
               dictionary_key_iterator<T>(end()) };
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
    T lookup(string_view Key) const {
      const_iterator it = find(Key);
      if (it != end())
        return it->second;
      return T();
    }

    /// Lookup the ValueTy for the \p Key, or create a default constructed value
    /// if the key is not in the map.
    T& operator[](string_view Key) noexcept { return try_emplace(Key).first->second; }

    /// count - Return 1 if the element is in the map, 0 otherwise.
    VK_nodiscard size_type count(string_view Key) const { return find(Key) == end() ? 0 : 1; }

    template<typename InputTy>
    VK_nodiscard size_type count(const dictionary_entry<InputTy>& entry) const {
      return count(entry.key());
    }

    /// equal - check whether both of the containers are equal.
    bool operator==(const dictionary &RHS) const {
      if (size() != RHS.size())
        return false;

      for (const auto &KeyValue : *this) {
        auto FindInRHS = RHS.find(KeyValue.key());

        if (FindInRHS == RHS.end())
          return false;

        if (KeyValue.get() != FindInRHS->get())
          return false;
      }

      return true;
    }

    /// insert - Insert the specified key/value pair into the map.  If the key
    /// already exists in the map, return false and ignore the request, otherwise
    /// insert it and return true.
    bool insert(entry_type *KeyValue) {
      unsigned BucketNo = lookup_bucket_for(KeyValue->key());
      impl::dictionary_entry_base *&Bucket = TheTable[BucketNo];
      if (Bucket && Bucket != get_tombstone_val())
        return false;// Already exists in map.

      if (Bucket == get_tombstone_val())
        --NumTombstones;
      Bucket = KeyValue;
      ++NumItems;
      VK_assert(NumItems + NumTombstones <= NumBuckets);

      rehash_table();
      return true;
    }

    /// insert - Inserts the specified key/value pair into the map if the key
    /// isn't already in the map. The bool component of the returned pair is true
    /// if and only if the insertion takes place, and the iterator component of
    /// the pair points to the element with key equivalent to the key of the pair.
    std::pair<iterator, bool> insert(std::pair<string_view, T> KV) {
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
      impl::dictionary_entry_base *&Bucket = TheTable[BucketNo];
      if (Bucket && Bucket != get_tombstone_val())
        return std::make_pair(iterator(TheTable + BucketNo, false),
                              false);// Already exists in map.

      if (Bucket == get_tombstone_val())
        --NumTombstones;
      Bucket = entry_type::template create<Alloc>(Key, allocator, std::forward<ArgsTy>(Args)...);
      ++NumItems;
      assert(NumItems + NumTombstones <= NumBuckets);

      BucketNo = this->rehash_table(BucketNo);
      return std::make_pair(iterator(TheTable + BucketNo, false), true);
    }

    // clear - Empties out the dictionary
    void clear() {
      if (empty())
        return;

      // Zap all values, resetting the keys back to non-present (not tombstone),
      // which is safe because we're removing all elements.
      for (unsigned I = 0, E = NumBuckets; I != E; ++I) {
        impl::dictionary_entry_base *&Bucket = TheTable[I];
        if (Bucket && Bucket != get_tombstone_val()) {
          static_cast<entry_type *>(Bucket)->template destroy<Alloc>(allocator);
        }
        Bucket = nullptr;
      }

      NumItems = 0;
      NumTombstones = 0;
    }

    /// remove - Remove the specified key/value pair from the map, but do not
    /// erase it.  This aborts if the key is not in the map.
    void remove(entry_type *KeyValue) { remove_key(KeyValue); }

    void erase(iterator I) {
      entry_type &V = *I;
      remove(&V);
      V.template destroy<Alloc>(allocator);
    }

    bool erase(string_view Key) {
      iterator I = find(Key);
      if (I == end())
        return false;
      erase(I);
      return true;
    }
    
  private:
    [[no_unique_address]] allocator_state allocator;
  };
  
  




  template<typename T>
  class dictionary_const_iterator : public impl::dictionary_iter_base<
      dictionary_const_iterator<T>,
      const dictionary_entry<T>> {
    using base = impl::dictionary_iter_base<dictionary_const_iterator<T>, const dictionary_entry<T>>;

  public:
    using base::base;

    const dictionary_entry<T>& operator*() const noexcept {
      return *static_cast<const dictionary_entry<T>*>(*this->Ptr);
    }
};

  template<typename T>
  class dictionary_iterator : public impl::dictionary_iter_base<
      dictionary_iterator<T>,
      dictionary_entry<T>> {
    using base = impl::dictionary_iter_base<dictionary_iterator<T>, dictionary_entry<T>>;

  public:
    using base::base;

    dictionary_entry<T>& operator*() const noexcept {
      return *static_cast<dictionary_entry<T>*>(*this->Ptr);
    }

    operator dictionary_const_iterator<T>() const noexcept {
      return dictionary_const_iterator<T>(this->Ptr, true);
    }
  };

  template<typename T>
  class dictionary_key_iterator : public iterator_adaptor_base<
                                      dictionary_key_iterator<T>,
                                      dictionary_const_iterator<T>,
                                      std::forward_iterator_tag,
                                      string_view>{
    using base = iterator_adaptor_base<dictionary_key_iterator, dictionary_const_iterator<T>, std::forward_iterator_tag, string_view>;
  public:

    dictionary_key_iterator() = default;
    explicit dictionary_key_iterator(dictionary_const_iterator<T> iter) noexcept : base(std::move(iter)){}


    string_view& operator*() const noexcept {
      cache_key = this->wrapped()->key();
      return cache_key;
    }

  private:
    mutable string_view cache_key;
  };


}

#endif  //VALKYRIE_ADT_DICTIONARY_HPP
