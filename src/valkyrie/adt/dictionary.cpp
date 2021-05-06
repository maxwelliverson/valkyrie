//
// Created by maxwe on 2021-05-02.
//

#include <valkyrie/adt/dictionary.hpp>
#include <valkyrie/utility/hash.hpp>
#include <valkyrie/utility/pow2.hpp>

#include <bit>

using namespace valkyrie;

namespace {
  inline constexpr u32 default_bucket_count = 16;
  inline u32 get_min_bucket_to_reserve_for_entries(u32 entries) noexcept {
    if ( entries == 0 )
      return 0;
    return std::bit_ceil(entries * 4 / 3 + 1);
  }
}

void impl::dictionary::init(unsigned Size, PFN_allocate_table alloc_fn, void* allocator_state) {
  VK_assert_msg(is_pow2(Size), "Init Size must be a power of 2 or zero!");

  unsigned NewNumBuckets = Size ? Size : default_bucket_count;
  NumItems = 0;
  NumTombstones = 0;

  TheTable = alloc_fn(NewNumBuckets, allocator_state);
  /*TheTable = static_cast<impl::dictionary_entry_base **>(calloc(
      NewNumBuckets + 1, sizeof(impl::dictionary_entry_base **) + sizeof(unsigned)));*/

  // Set the member only if TheTable was successfully allocated
  NumBuckets = NewNumBuckets;

  // Allocate one extra bucket, set it to look filled so the iterators stop at
  // end.
  TheTable[NumBuckets] = (impl::dictionary_entry_base *)2;
}

unsigned int impl::dictionary::rehash_table(unsigned int BucketNo, PFN_allocate_table alloc_fn, PFN_deallocate_table dealloc_fn, void* allocator_state) {
  unsigned NewSize;
  unsigned *HashTable = (unsigned *)(TheTable + NumBuckets + 1);

  // If the hash table is now more than 3/4 full, or if fewer than 1/8 of
  // the buckets are empty (meaning that many are filled with tombstones),
  // grow/rehash the table.
  if (NumItems * 4 > NumBuckets * 3) VK_unlikely {
    NewSize = NumBuckets * 2;
  }
  else if (NumBuckets - (NumItems + NumTombstones) <= NumBuckets / 8) VK_unlikely {
    NewSize = NumBuckets;
  }
  else {
    return BucketNo;
  }

  unsigned NewBucketNo = BucketNo;
  // Allocate one extra bucket which will always be non-empty.  This allows the
  // iterators to stop at end.
  auto NewTableArray = alloc_fn(NewSize, allocator_state);

  unsigned *NewHashArray = (unsigned *)(NewTableArray + NewSize + 1);
  NewTableArray[NewSize] = (impl::dictionary_entry_base *)2;

  // Rehash all the items into their new buckets.  Luckily :) we already have
  // the hash values available, so we don't have to rehash any strings.
  for (unsigned I = 0, E = NumBuckets; I != E; ++I) {
    impl::dictionary_entry_base *Bucket = TheTable[I];
    if (Bucket && Bucket != get_tombstone_val()) {
      // Fast case, bucket available.
      unsigned FullHash = HashTable[I];
      unsigned NewBucket = FullHash & (NewSize - 1);
      if (!NewTableArray[NewBucket]) {
        NewTableArray[FullHash & (NewSize - 1)] = Bucket;
        NewHashArray[FullHash & (NewSize - 1)] = FullHash;
        if (I == BucketNo)
          NewBucketNo = NewBucket;
        continue;
      }

      // Otherwise probe for a spot.
      unsigned ProbeSize = 1;
      do {
        NewBucket = (NewBucket + ProbeSize++) & (NewSize - 1);
      } while (NewTableArray[NewBucket]);

      // Finally found a slot.  Fill it in.
      NewTableArray[NewBucket] = Bucket;
      NewHashArray[NewBucket] = FullHash;
      if (I == BucketNo)
        NewBucketNo = NewBucket;
    }
  }

  dealloc_fn(TheTable, NumBuckets, allocator_state);

  TheTable = NewTableArray;
  NumBuckets = NewSize;
  NumTombstones = 0;
  return NewBucketNo;
}

u32 impl::dictionary::lookup_bucket_for(string_view Key, PFN_allocate_table alloc_fn, void* allocator_state) {
  u32 HTSize = NumBuckets;
  if (HTSize == 0) { // Hash table unallocated so far?
    init(default_bucket_count, alloc_fn, allocator_state);
    HTSize = NumBuckets;
  }
  u32 FullHashValue = hash32(Key);
  u32 BucketNo = FullHashValue & (HTSize - 1);
  auto* HashTable = (u32*)(TheTable + NumBuckets + 1);

  unsigned ProbeAmt = 1;
  int FirstTombstone = -1;
  while (true) {
    impl::dictionary_entry_base *BucketItem = TheTable[BucketNo];
    // If we found an empty bucket, this key isn't in the table yet, return it.
    if (!BucketItem) VK_likely {
      // If we found a tombstone, we want to reuse the tombstone instead of an
      // empty bucket.  This reduces probing.
      if (FirstTombstone != -1) {
        HashTable[FirstTombstone] = FullHashValue;
        return FirstTombstone;
      }

      HashTable[BucketNo] = FullHashValue;
      return BucketNo;
    }

    if (BucketItem == get_tombstone_val()) {
      // Skip over tombstones.  However, remember the first one we see.
      if (FirstTombstone == -1)
        FirstTombstone = BucketNo;
    } else if (HashTable[BucketNo] == FullHashValue) VK_likely {
      // If the full hash value matches, check deeply for a match.  The common
      // case here is that we are only looking at the buckets (for item info
      // being non-null and for the full hash value) not at the items.  This
      // is important for cache locality.

      // Do the comparison like this because Name isn't necessarily
      // null-terminated!
      auto *ItemStr = (utf8*)BucketItem + ItemSize;
      if (Key == string_view(ItemStr, BucketItem->get_key_length())) {
        // We found a match!
        return BucketNo;
      }
    }

    // Okay, we didn't find the item.  Probe to the next bucket.
    BucketNo = (BucketNo + ProbeAmt) & (HTSize - 1);

    // Use quadratic probing, it has fewer clumping artifacts than linear
    // probing and has good cache behavior in the common case.
    ++ProbeAmt;
  }
}

void impl::dictionary::explicit_init(u32 initSize, PFN_allocate_table alloc_fn, void* allocator_state) noexcept {

  // If a size is specified, initialize the table with that many buckets.
  if (initSize) {
    // The table will grow when the number of entries reach 3/4 of the number of
    // buckets. To guarantee that "InitSize" number of entries can be inserted
    // in the table without growing, we allocate just what is needed here.
    init(get_min_bucket_to_reserve_for_entries(initSize), alloc_fn, allocator_state);
    return;
  }

  // Otherwise, initialize it with zero buckets to avoid the allocation.
  TheTable = nullptr;
  NumBuckets = 0;
  NumItems = 0;
  NumTombstones = 0;
}

int impl::dictionary::find_key(string_view Key) const {
  unsigned HTSize = NumBuckets;
  if (HTSize == 0)
    return -1; // Really empty table?
  u32 FullHashValue = hash32(Key);
  u32 BucketNo = FullHashValue & (HTSize - 1);
  auto* HashTable = (u32*)(TheTable + NumBuckets + 1);

  unsigned ProbeAmt = 1;
  while (true) {
    impl::dictionary_entry_base *BucketItem = TheTable[BucketNo];
    // If we found an empty bucket, this key isn't in the table yet, return.
    if (!BucketItem) VK_likely
      return -1;

    if (BucketItem == get_tombstone_val()) {
      // Ignore tombstones.
    } else if (HashTable[BucketNo] == FullHashValue) VK_likely {
      // If the full hash value matches, check deeply for a match.  The common
      // case here is that we are only looking at the buckets (for item info
      // being non-null and for the full hash value) not at the items.  This
      // is important for cache locality.

      // Do the comparison like this because NameStart isn't necessarily
      // null-terminated!
      auto *ItemStr = (utf8 *)BucketItem + ItemSize;
      if (Key == string_view(ItemStr, BucketItem->get_key_length())) {
        // We found a match!
        return BucketNo;
      }
    }

    // Okay, we didn't find the item.  Probe to the next bucket.
    BucketNo = (BucketNo + ProbeAmt) & (HTSize - 1);

    // Use quadratic probing, it has fewer clumping artifacts than linear
    // probing and has good cache behavior in the common case.
    ++ProbeAmt;
  }
}

impl::dictionary_entry_base * impl::dictionary::remove_key(string_view Key) {
  int Bucket = find_key(Key);
  if (Bucket == -1)
    return nullptr;

  impl::dictionary_entry_base *Result = TheTable[Bucket];
  TheTable[Bucket] = get_tombstone_val();
  --NumItems;
  ++NumTombstones;
  assert(NumItems + NumTombstones <= NumBuckets);

  return Result;
}

void impl::dictionary::remove_key(dictionary_entry_base *V) {
  utf8_string VStr = (utf8 *)V + ItemSize;
  impl::dictionary_entry_base *V2 = remove_key(string_view(VStr, V->get_key_length()));
  (void)V2;
  assert(V == V2 && "Didn't find key?");
}