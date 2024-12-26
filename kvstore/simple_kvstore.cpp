#include "simple_kvstore.hpp"

bool SimpleKvStore::Get(const GetRequest* req, GetResponse* res) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT

  // Get the value correspnoding to key from the internal KV store
  auto it = kvmap.find(req->key);
  
  // If value was not found
  if (it == kvmap.end()) {
    return false;
  }
  // If value was found
  else {
    // Stores value in value field of res
    res->value = kvmap.find((*req).key)->second;
    return true;
  }
}

bool SimpleKvStore::Put(const PutRequest* req, PutResponse*) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  mtx.lock();
  auto it = kvmap.find(req->key);
  // If item is not found, then we want to put it there
  if (it == kvmap.end()) {
    kvmap.emplace(req->key, req->value);
  }
  // If the item is there, we want to replace existing value
  else {
    it->second = req->value;
  }
  mtx.unlock();
  return true;
}

bool SimpleKvStore::Append(const AppendRequest* req, AppendResponse*) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  mtx.lock();
  auto it = kvmap.find(req->key);

  // If item does not already exist, create a new key-value pair
  if (it == kvmap.end()) {
    kvmap.emplace(req->key, req->value);
  }
  else {
    it->second = it->second.append(req->value);
  }
  mtx.unlock();
  return true;
}

bool SimpleKvStore::Delete(const DeleteRequest* req, DeleteResponse* res) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  mtx.lock();
  auto it = kvmap.find(req->key);

  // If key does not exist
  if (it == kvmap.end()) {
    return false;
  }
  else {
    res->value = it->second;
    kvmap.erase(it);
  }
  mtx.unlock();
  return res;
}

bool SimpleKvStore::MultiGet(const MultiGetRequest* req,
                             MultiGetResponse* res) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  
  // Loop through all keys in the MultiGetRequest
  mtx.lock();
  for(std::string element : req->keys) {
    auto it = kvmap.find(element);
    // If key does not exist
    if (it == kvmap.end()) {
      return false;
    }
    res->values.push_back(it->second);
  }
  mtx.unlock();
  return true;
}

bool SimpleKvStore::MultiPut(const MultiPutRequest* req, MultiPutResponse*) {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  mtx.lock();
  if (req->keys.size() != req->values.size()) {
    return false;
  }
  for (int i = 0; i < req->keys.size(); ++i) {
    auto it = kvmap.find(req->keys[i]);
    // If key does not exist
    if (it == kvmap.end()) {
      kvmap.emplace(req->keys[i], req->values[i]);
    }
    else {
      it->second = req->values[i];
    }
  }
  mtx.unlock();
  return true;
}

std::vector<std::string> SimpleKvStore::AllKeys() {
  // TODO (Part A, Steps 1 and 2): IMPLEMENT
  std::vector<std::string> keys;
  for(auto& x : kvmap) {
    keys.push_back(x.first);
  }
  return keys;
}
