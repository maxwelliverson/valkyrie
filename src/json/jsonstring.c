//
// Created by maxwe on 2021-04-13.
//

#include "internal.h"
#include "jsonstring.h"
#include "jsontypes.h"
#include "jsonmm.h"


#define JSON_TOMBSTONE_VALUE ((json_symbol_t)0xFFFFFFFFFFFFFFF8)
#define JSON_DEFAULT_HASH_VALUE 5381
#define JSON_HASHTABLE_ELEMENT_SIZE (sizeof(void*) + sizeof(json_u32_t))
#define JSON_DEFAULT_TABLE_INIT_SIZE 16
#define JSON_INDEX_NOT_FOUND ((json_u32_t)-1)

#if __has_builtin(__builtin_memcmp)
#define JSON_memcmp __builtin_memcmp
#else
#define JSON_memcmp memcmp
#endif



/// The Bernstein hash function used by the DWARF accelerator tables.
inline static json_u32_t djb_hash__(const char* buffer, json_u64_t length, json_u32_t H) {
  for (json_u64_t i = 0; i < length; ++i) {
    H = (H << 5) + H + (json_u8_t)buffer[i];
  }
  return H;
}

/// Returns the number of buckets to allocate to ensure that the DenseMap can
/// accommodate \p NumEntries without need to grow().
inline static json_u32_t json_min_bucket_reservation_size__(json_u32_t numEntries) {
  json_assert_internal(numEntries > 0);

  return json_bit_ceil(numEntries * 4 / 3 + 1);
}


inline static json_status_t json_symbol_registry_init__(json_symbol_registry_t registry,
                                                        json_u32_t initSize,
                                                        json_internal_allocator_t allocator)  {
  json_assert_internal(json_is_power_of_two(initSize));

  json_status_t result;

  const json_u32_t newNumBuckets = initSize ? initSize : JSON_DEFAULT_TABLE_INIT_SIZE;
  registry->numItems = 0;
  registry->numTombstones = 0;

  registry->symbolTable = json_alloc_dynamic(&result, sizeof(json_symbol_t), newNumBuckets + 1, allocator);

  if (JSON_likely( result == JSON_SUCCESS )) {
    registry->numBuckets = newNumBuckets;
    registry->symbolTable[newNumBuckets] = (json_symbol_t)2;
    registry->allocator = allocator;
    //registry->itemSize  = itemSize;
  }
  return result;
}

inline static json_status_t symbol_create__(json_symbol_t* symbol, json_symbol_registry_t registry, const char* string, json_u64_t length) {
  json_status_t status;
  json_assert_internal( symbol );
  json_assert( length < UINT_MAX );
  json_symbol_t newSymbol = json_alloc(&status, sizeof(struct json_symbol) + length + 1, registry->allocator);
  if (JSON_likely(status == JSON_SUCCESS)) {
    newSymbol->length = (json_u32_t)length;
    newSymbol->refCount = 1;
    json_memcpy(newSymbol->data, string, length);
    newSymbol->data[length] = '\0';
    *symbol = newSymbol;
  }
  return status;
}
inline static void          symbol_release__(json_symbol_registry_t registry, json_symbol_t symbol) {
  if (!--symbol->refCount) {
    json_free(symbol, symbol->length + sizeof(struct json_symbol) + 1, registry->allocator);
  }
}

inline static json_u32_t symbol_registry_rehash__(json_symbol_registry_t registry, json_u32_t bucketNo) {
  json_status_t    status;
  json_u32_t       newSize;
  const json_u32_t numItems      = registry->numItems;
  const json_u32_t numTombstones = registry->numTombstones;
  const json_u32_t oldSize       = registry->numBuckets;
  json_u32_t*      hashTable = (json_u32_t *)(registry->symbolTable + registry->numBuckets + 1);

  // If the hash table is now more than 3/4 full, or if fewer than 1/8 of
  // the buckets are empty (meaning that many are filled with tombstones),
  // grow/rehash the table.
  if (JSON_unlikely(numItems * 4 > oldSize * 3)) {
    newSize = oldSize * 2;
  }
  else if (JSON_unlikely ((oldSize - (numItems + numTombstones)) <= (oldSize / 8))) {
    newSize = oldSize;
  }
  else {
    return bucketNo;
  }

  unsigned newBucketNo = bucketNo;
  // Allocate one extra bucket which will always be non-empty.  This allows the
  // iterators to stop at end.
  json_symbol_t* newTable = json_alloc_dynamic(&status, JSON_HASHTABLE_ELEMENT_SIZE, newBucketNo + 1, registry->allocator);

  unsigned *newHashArray = (json_u32_t *)(newTable + newSize + 1);
  newTable[newSize] = (json_symbol_t)2;

  // Rehash all the items into their new buckets.  Luckily :) we already have
  // the hash values available, so we don't have to rehash any strings.
  for (unsigned I = 0, E = oldSize; I != E; ++I) {
    json_symbol_t bucket = registry->symbolTable[I];
    if (bucket && bucket != JSON_TOMBSTONE_VALUE) {
      // Fast case, bucket available.
      json_u32_t fullHash = hashTable[I];
      unsigned newBucket = fullHash & (newSize - 1);
      if (!newTable[newBucket]) {
        newTable[newBucket] = bucket;
        newHashArray[newBucket] = fullHash;
        if (I == bucketNo)
          newBucketNo = newBucket;
        continue;
      }

      // Otherwise probe for a spot.
      unsigned ProbeSize = 1;
      do {
        newBucket = (newBucket + ProbeSize++) & (newSize - 1);
      } while (newTable[newBucket]);

      // Finally found a slot.  Fill it in.
      newTable[newBucket] = bucket;
      newHashArray[newBucket] = fullHash;
      if (I == bucketNo)
        newBucketNo = newBucket;
    }
  }

  json_free_dynamic(registry->symbolTable, JSON_HASHTABLE_ELEMENT_SIZE, oldSize, registry->allocator);

  registry->symbolTable   = newTable;
  registry->numBuckets    = newSize;
  registry->numTombstones = 0;

  return newBucketNo;
}
inline static json_u32_t symbol_registry_lookup_bucket__(json_symbol_registry_t registry, const char* string, json_u64_t length) {
  json_u32_t hashTableSize = registry->numBuckets;
  json_assert_internal(hashTableSize > 0);
  if (hashTableSize == 0) { // Hash table unallocated so far?
    json_symbol_registry_init__(registry, JSON_DEFAULT_TABLE_INIT_SIZE, registry->allocator);
    hashTableSize = registry->numBuckets;
  }
  unsigned fullHashValue = djb_hash__(string, length, 0);
  unsigned bucketNo = fullHashValue & (hashTableSize - 1);
  json_u32_t* hashTable = (json_u32_t *)(registry->symbolTable + hashTableSize + 1);

  json_u32_t probeAmt = 1;
  json_u32_t firstTombstone = JSON_INDEX_NOT_FOUND;

  while (true) {
    json_symbol_t bucketItem = registry->symbolTable[bucketNo];
    // If we found an empty bucket, this key isn't in the table yet, return it.
    if (JSON_likely(!bucketItem)) {
      // If we found a tombstone, we want to reuse the tombstone instead of an
      // empty bucket.  This reduces probing.
      if (firstTombstone != JSON_INDEX_NOT_FOUND) {
        hashTable[firstTombstone] = fullHashValue;
        return firstTombstone;
      }

      hashTable[bucketNo] = fullHashValue;
      return bucketNo;
    }

    if (bucketItem == JSON_TOMBSTONE_VALUE) {
      // Skip over tombstones.  However, remember the first one we see.
      if (firstTombstone == JSON_INDEX_NOT_FOUND)
        firstTombstone = bucketNo;
    }
    else if (JSON_likely(hashTable[bucketNo] == fullHashValue)) {
      // If the full hash value matches, check deeply for a match.  The common
      // case here is that we are only looking at the buckets (for item info
      // being non-null and for the full hash value) not at the items.  This
      // is important for cache locality.

      if ( length == bucketItem->length && JSON_memcmp(string, bucketItem->data, length) == 0 ) {
        return bucketNo;
      }
    }

    // Okay, we didn't find the item.  Probe to the next bucket.
    bucketNo = (bucketNo + probeAmt) & (hashTableSize - 1);

    // Use quadratic probing, it has fewer clumping artifacts than linear
    // probing and has good cache behavior in the common case.
    ++probeAmt;
  }
}
inline static json_u32_t symbol_registry_find_key__(const struct json_symbol_registry* registry,  const char* string, json_u64_t length) {
  unsigned hashTableSize = registry->numBuckets;
  if (hashTableSize == 0)
    return JSON_INDEX_NOT_FOUND; // Really empty table?
  json_u32_t  fullHashValue = djb_hash__(string, length, 0);
  json_u32_t  bucketNo      = fullHashValue & (hashTableSize - 1);
  json_u32_t* hashTable     = (json_u32_t*)(registry->symbolTable + hashTableSize + 1);

  json_symbol_t* theTable = registry->symbolTable;

  json_u32_t  probeAmt = 1;

  while (true) {

    json_symbol_t bucketItem = theTable[bucketNo];
    // If we found an empty bucket, this key isn't in the table yet, return.
    if (JSON_likely(!bucketItem)) {
      return JSON_INDEX_NOT_FOUND;
    }


    if (bucketItem == JSON_TOMBSTONE_VALUE) {
      // Ignore tombstones.
    } else if (JSON_likely(hashTable[bucketNo] == fullHashValue)) {

      // If the full hash value matches, check deeply for a match.  The common
      // case here is that we are only looking at the buckets (for item info
      // being non-null and for the full hash value) not at the items.  This
      // is important for cache locality.

      if ( length == bucketItem->length && JSON_memcmp(string, bucketItem->data, length) == 0 ) {
        return bucketNo;
      }
    }

    // Okay, we didn't find the item.  Probe to the next bucket.
    bucketNo = (bucketNo + probeAmt) & (hashTableSize - 1);

    // Use quadratic probing, it has fewer clumping artifacts than linear
    // probing and has good cache behavior in the common case.
    ++probeAmt;
  }
}






json_symbol_t* symbol_registry_find(json_symbol_registry_t registry,
                                    const char* buffer,
                                    json_u64_t length) {
  json_u32_t bucket = symbol_registry_find_key__(registry, buffer, length);
  return (bucket == JSON_INDEX_NOT_FOUND) ? NULL : registry->symbolTable + bucket;
}

json_symbol_t symbol_registry_register_symbol(json_symbol_registry_t registry,
                                              const char* string,
                                              json_u64_t length) {
  json_u32_t bucketNo   = symbol_registry_lookup_bucket__(registry, string, length);
  json_symbol_t* bucket = registry->symbolTable + bucketNo;
  if (*bucket && *bucket != JSON_TOMBSTONE_VALUE ) {
    return *bucket;
  }

  if (*bucket == JSON_TOMBSTONE_VALUE)
    --registry->numTombstones;
  json_status_t status = symbol_create__(bucket, registry, string, length);
  UNUSED_RELEASE(status);
  json_assert(status == JSON_SUCCESS);
  ++registry->numItems;
  json_assert_internal(registry->numItems + registry->numTombstones <= registry->numBuckets);

  symbol_registry_rehash__(registry, 0);
  return *bucket;
}

void          symbol_registry_clear(json_symbol_registry_t registry) {
  if (symbol_registry_is_empty(registry)) {
    return;
  }

  json_symbol_t* const theTable = registry->symbolTable;

  for (json_u32_t i = 0, e = registry->numBuckets; i != e; ++i) {
    json_symbol_t* bucket = theTable + i;
    if (*bucket && *bucket != JSON_TOMBSTONE_VALUE) {
      symbol_release__(registry, *bucket);
    }
    *bucket = NULL;
  }

  registry->numItems = 0;
  registry->numTombstones = 0;
}

json_symbol_t symbol_registry_remove(json_symbol_registry_t registry, const char* string, json_u64_t length) {
  json_u32_t bucket = symbol_registry_find_key__(registry, string, length);
  if (bucket == JSON_INDEX_NOT_FOUND)
    return NULL;

  json_symbol_t result = registry->symbolTable[bucket];

  registry->symbolTable[bucket] = JSON_TOMBSTONE_VALUE;
  --registry->numItems;
  ++registry->numTombstones;
      json_assert_internal(registry->numItems + registry->numTombstones < registry->numBuckets);

  return result;
}
void          symbol_registry_remove_symbol(json_symbol_registry_t registry, json_symbol_t symbol) {
  json_symbol_t V2 = symbol_registry_remove(registry, symbol->data, symbol->length);
  UNUSED_RELEASE(V2);
  json_assert(symbol == V2 && "Didn't find key?");
}


json_bool_t    symbol_registry_erase(json_symbol_registry_t registry, const char* string, json_u64_t length) {
  json_symbol_t removedSymbol = symbol_registry_remove(registry, string, length);
  if ( removedSymbol ) {
    symbol_release__(registry, removedSymbol);
    return true;
  }
  return false;
}
void           symbol_registry_erase_symbol(json_symbol_registry_t registry, json_symbol_t symbol) {
  if ( symbol ) {
    json_symbol_t result = symbol_registry_remove(registry, symbol->data, symbol->length);
    UNUSED_RELEASE(result);
    json_assert_internal(result == symbol);
    symbol_release__(registry, symbol);
  }
}

json_status_t symbol_registry_init(json_symbol_registry_t registry,
                                   json_u32_t initSize,
                                   json_internal_allocator_t allocator)  {
  if ( initSize ) {
    return json_symbol_registry_init__(registry, json_min_bucket_reservation_size__(initSize), allocator);
  }

  registry->symbolTable   = NULL;
  registry->allocator     = allocator;
  registry->numBuckets    = 0;
  registry->numItems      = 0;
  registry->numTombstones = 0;

  return JSON_SUCCESS;
}


void          symbol_registry_cleanup(json_symbol_registry_t registry) {
  if (!symbol_registry_is_empty(registry)) {
    const json_u32_t numBuckets = registry->numBuckets;
    for (unsigned I = 0, E = numBuckets; I != E; ++I) {
      json_symbol_t bucket = registry->symbolTable[I];
      if (bucket && bucket != JSON_TOMBSTONE_VALUE) {
        symbol_release__(registry, bucket);
      }
    }
  }
  json_free_dynamic(registry->symbolTable,
                    2 * sizeof(void*),
                    registry->numBuckets + 1,
                    registry->allocator);
}