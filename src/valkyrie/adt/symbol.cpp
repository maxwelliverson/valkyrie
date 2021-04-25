//
// Created by maxwe on 2021-03-12.
//

#include <valkyrie/adt/symbol.hpp>

#include <bit>



using namespace valkyrie;

/// Returns the number of buckets to allocate to ensure that the DenseMap can
/// accommodate \p NumEntries without need to grow().
static unsigned getMinBucketToReserveForEntries(unsigned NumEntries) {
  // Ensure that "NumEntries * 4 < NumBuckets * 3"
  if (NumEntries == 0)
    return 0;
  // +1 is required because of the strict equality.
  // For example if NumEntries is 48, we need to return 401.
  return std::bit_ceil(NumEntries * 4 / 3 + 1);
}

/// The Bernstein hash function used by the DWARF accelerator tables.
inline static u32 djbHash(std::span<const std::byte> buffer, uint32_t H = 5381) {
  for (std::byte C : buffer)
    H = (H << 5) + H + u8(C);
  return H;
}
/// The Bernstein hash function used by the DWARF accelerator tables.
inline static u32 djbHash(string_view buffer, uint32_t H = 5381) {
  for (u8 C : buffer)
    H = (H << 5) + H + C;
  return H;
}

symbol_registry_impl::symbol_registry_impl(unsigned InitSize, unsigned itemSize) {
  ItemSize = itemSize;

  // If a size is specified, initialize the table with that many buckets.
  if (InitSize) {
    // The table will grow when the number of entries reach 3/4 of the number of
    // buckets. To guarantee that "InitSize" number of entries can be inserted
    // in the table without growing, we allocate just what is needed here.
    init(getMinBucketToReserveForEntries(InitSize));
    return;
  }

  // Otherwise, initialize it with zero buckets to avoid the allocation.
  TheTable = nullptr;
  NumBuckets = 0;
  NumItems = 0;
  NumTombstones = 0;
}

void symbol_registry_impl::init(unsigned InitSize) {
  assert((InitSize & (InitSize - 1)) == 0 &&
         "Init Size must be a power of 2 or zero!");

  unsigned NewNumBuckets = InitSize ? InitSize : 16;
  NumItems = 0;
  NumTombstones = 0;

  TheTable = static_cast<detail::symbol_registry_entry_base **>(calloc(
      NewNumBuckets + 1, sizeof(detail::symbol_registry_entry_base **) + sizeof(unsigned)));

  // Set the member only if TheTable was successfully allocated
  NumBuckets = NewNumBuckets;

  // Allocate one extra bucket, set it to look filled so the iterators stop at
  // end.
  TheTable[NumBuckets] = (detail::symbol_registry_entry_base *)2;
}

/// lookup_bucket_for - Look up the bucket that the specified string should end
/// up in.  If it already exists as a key in the map, the Item pointer for the
/// specified bucket will be non-null.  Otherwise, it will be null.  In either
/// case, the FullHashValue field of the bucket will be set to the hash value
/// of the string.
unsigned symbol_registry_impl::lookup_bucket_for(string_view Name) {
  unsigned HTSize = NumBuckets;
  if (HTSize == 0) { // Hash table unallocated so far?
    init(16);
    HTSize = NumBuckets;
  }
  unsigned FullHashValue = djbHash(Name, 0);
  unsigned BucketNo = FullHashValue & (HTSize - 1);
  unsigned *HashTable = (unsigned *)(TheTable + NumBuckets + 1);

  unsigned ProbeAmt = 1;
  int FirstTombstone = -1;
  while (true) {
    detail::symbol_registry_entry_base *BucketItem = TheTable[BucketNo];
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
      if (Name == string_view(ItemStr, BucketItem->get_key_length())) {
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

/// find_key - Look up the bucket that contains the specified key. If it exists
/// in the map, return the bucket number of the key.  Otherwise return -1.
/// This does not modify the map.
int symbol_registry_impl::find_key(string_view Key) const {
  unsigned HTSize = NumBuckets;
  if (HTSize == 0)
    return -1; // Really empty table?
  unsigned FullHashValue = djbHash(Key, 0);
  unsigned BucketNo = FullHashValue & (HTSize - 1);
  unsigned *HashTable = (unsigned *)(TheTable + NumBuckets + 1);

  unsigned ProbeAmt = 1;
  while (true) {
    detail::symbol_registry_entry_base *BucketItem = TheTable[BucketNo];
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

/// remove_key - Remove the specified StringMapEntry from the table, but do not
/// delete it.  This aborts if the value isn't in the table.
void symbol_registry_impl::remove_key(detail::symbol_registry_entry_base *V) {
  utf8_string VStr = (utf8 *)V + ItemSize;
  detail::symbol_registry_entry_base *V2 = remove_key(string_view(VStr, V->get_key_length()));
  (void)V2;
  assert(V == V2 && "Didn't find key?");
}

/// remove_key - Remove the StringMapEntry for the specified key from the
/// table, returning it.  If the key is not in the table, this returns null.
detail::symbol_registry_entry_base *symbol_registry_impl::remove_key(string_view Key) {
  int Bucket = find_key(Key);
  if (Bucket == -1)
    return nullptr;

  detail::symbol_registry_entry_base *Result = TheTable[Bucket];
  TheTable[Bucket] = get_tombstone_val();
  --NumItems;
  ++NumTombstones;
  assert(NumItems + NumTombstones <= NumBuckets);

  return Result;
}

/// rehash_table - Grow the table, redistributing values into the buckets with
/// the appropriate mod-of-hashtable-size.
unsigned symbol_registry_impl::rehash_table(unsigned BucketNo) {
  unsigned NewSize;
  unsigned *HashTable = (unsigned *)(TheTable + NumBuckets + 1);

  // If the hash table is now more than 3/4 full, or if fewer than 1/8 of
  // the buckets are empty (meaning that many are filled with tombstones),
  // grow/rehash the table.
  if (NumItems * 4 > NumBuckets * 3) VK_unlikely {
    NewSize = NumBuckets * 2;
  } else if (NumBuckets - (NumItems + NumTombstones) <=
                           NumBuckets / 8) VK_unlikely {
    NewSize = NumBuckets;
  } else {
    return BucketNo;
  }

  unsigned NewBucketNo = BucketNo;
  // Allocate one extra bucket which will always be non-empty.  This allows the
  // iterators to stop at end.
  auto NewTableArray = static_cast<detail::symbol_registry_entry_base **>(calloc(
      NewSize + 1, sizeof(detail::symbol_registry_entry_base *) + sizeof(unsigned)));

  unsigned *NewHashArray = (unsigned *)(NewTableArray + NewSize + 1);
  NewTableArray[NewSize] = (detail::symbol_registry_entry_base *)2;

  // Rehash all the items into their new buckets.  Luckily :) we already have
  // the hash values available, so we don't have to rehash any strings.
  for (unsigned I = 0, E = NumBuckets; I != E; ++I) {
    detail::symbol_registry_entry_base *Bucket = TheTable[I];
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

  free(TheTable);

  TheTable = NewTableArray;
  NumBuckets = NewSize;
  NumTombstones = 0;
  return NewBucketNo;
}