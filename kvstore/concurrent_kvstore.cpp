#include "concurrent_kvstore.hpp"

#include <optional>

bool ConcurrentKvStore::Get(const GetRequest* req, GetResponse* res) {

  // Get index of bucket to search for
  size_t bucket_index = store.bucket(req->key);
  // Lock the mutex at bucket_index with a shared lock
  std::shared_lock<std::shared_mutex> lock(store.shared_mutex_list[bucket_index]);

  // Find the mutex correpsonding to the bucket_index and lock it

  // Gets the value for the key and stores it in item
  std::optional<DbItem> item = store.getIfExists(bucket_index, req->key);
  // If key is in bucket and item exists
  if (item) {
    res->value = item.value().value;
    return true;
  }
  else {
    return false;
  }
}

bool ConcurrentKvStore::Put(const PutRequest* req, PutResponse*) {
  // Get index of bucket to insert into
  size_t bucket_index = store.bucket(req->key);
  // Exclusive lock that mutex
  std::unique_lock lock(store.shared_mutex_list[bucket_index]);

  // Insert item into hash table
  store.insertItem(bucket_index, req->key, req->value);
  return true;
}

bool ConcurrentKvStore::Append(const AppendRequest* req, AppendResponse*) {
  size_t bucket_index = store.bucket(req->key);
  // Exclusive lock that mutex
  std::unique_lock lock(store.shared_mutex_list[bucket_index]);
  
  // Insert item into hash table
  std::optional<DbItem> item = store.getIfExists(bucket_index, req->key);
  // If item exists
  if (item) {
    // (item.value().value).append(req->value);
    // TODO: Why can't I set the value directly (above)
    std::string elem = item.value().value + req->value;
    store.insertItem(bucket_index, req->key, elem);
  }
  else {
    store.insertItem(bucket_index, req->key, req->value);
  }
  return true;
}

bool ConcurrentKvStore::Delete(const DeleteRequest* req, DeleteResponse* res) {
  // Get index of bucket to delete from
  size_t bucket_index = store.bucket(req->key);

  // Exclusive lock that mutex
  std::unique_lock lock(store.shared_mutex_list[bucket_index]);

  std::optional<DbItem> item = store.getIfExists(bucket_index, req->key);
  // If item exists
  if (item) {
    res->value = item.value().value;
    store.removeItem(bucket_index, req->key);
    return true;
  }
  return false;
}

bool ConcurrentKvStore::MultiGet(const MultiGetRequest* req,
                                 MultiGetResponse* res) {
  
  // Loop to get the values of the buckets and store them inside a vector
  std::vector<size_t> bucket_nums;
  for(std::string key : req->keys) {
    // Find the bucket in which a key is stored
    size_t bucket_index = store.bucket(key);
    bucket_nums.push_back(bucket_index);
  }
  // Sort the vector and remove duplicates
  std::sort(bucket_nums.begin(), bucket_nums.end());
  auto last = std::unique(bucket_nums.begin(), bucket_nums.end());
  bucket_nums.erase(last, bucket_nums.end());

  // Locks each bucket in the sorted order
  for(size_t bucket_num : bucket_nums) {
    store.shared_mutex_list[bucket_num].lock_shared();
  }
  
  // Loop through all keys in the GetRequest
  for(std::string key : req->keys) {
    // Find the bucket in which a key is stored
    size_t bucket_index = store.bucket(key);
    // Get the value and store it in the response
    std::optional<DbItem> item = store.getIfExists(bucket_index, key);
    if (item) {
      res->values.push_back(item.value().value);
    }
    else {
      store.shared_mutex_list[bucket_index].unlock_shared();
      return false;
    }
  }
  // Unlocks all the shared locks
  for(size_t bucket_num : bucket_nums) {
    store.shared_mutex_list[bucket_num].unlock_shared();
  }

  return true;
}

bool ConcurrentKvStore::MultiPut(const MultiPutRequest* req,
                                 MultiPutResponse*) {
  
  if (req->keys.size() != req->values.size()) {
    return false;
  }

  // Loop to get the values of the buckets and store them inside a vector
  std::vector<size_t> bucket_nums;
  for(std::string key : req->keys) {
    // Find the bucket in which a key is stored
    size_t bucket_index = store.bucket(key);
    bucket_nums.push_back(bucket_index);
  }
  // Sort the vector and remove duplicates
  sort(bucket_nums.begin(), bucket_nums.end());
  auto last = std::unique(bucket_nums.begin(), bucket_nums.end());
  bucket_nums.erase(last, bucket_nums.end());

  // Locks each bucket/mutex in the sorted order
  for(size_t bucket_num : bucket_nums) {
    store.shared_mutex_list[bucket_num].lock();
  }

  size_t i = 0;
  for(std::string key : req->keys) {
    size_t bucket_index = store.bucket(key);
    store.insertItem(bucket_index, req->keys[i], req->values[i]);
    i += 1;
  }
  // Unlocks all the shared locks
  for(size_t bucket_num : bucket_nums) {
    store.shared_mutex_list[bucket_num].unlock();
  }
  return true;
}

std::vector<std::string> ConcurrentKvStore::AllKeys() {
  std::vector<std::string> keys;
  // Loop through every bucket in the hash table
  for(std::list<DbItem> b : store.buckets) {
    // Loop through every key in a bucket
    for(DbItem i : b) {
      keys.push_back(i.key);
    }
  }
  return keys;
}
