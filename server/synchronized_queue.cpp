#include "synchronized_queue.hpp"

template <typename T>
size_t synchronized_queue<T>::size() {
  // TODO (Part A, Step 3): IMPLEMENT
  std::unique_lock lock(mtx);
  return this->q.size();
}

template <typename T>
bool synchronized_queue<T>::pop(T* elt) {
  std::unique_lock lock(mtx);
  // Checks if the queue is empty
  if (this->q.size() == 0) {
    while (this->q.size() == 0 && !is_stopped) {
      cv.wait(lock);
    }
  }
  // Checks if queue is stopped
  if (is_stopped) {
    return true;
  }
  // Store element to be popped
  T removed_elem = this->q.front();
  this->q.pop();
  // Set elt equal to the element that was popped
  *elt = removed_elem;
  return false;
}

template <typename T>
void synchronized_queue<T>::push(T elt) {
  // TODO (Part A, Step 3): IMPLEMENT
  std::unique_lock lock(mtx);
  this->q.push(elt);
  if (this->q.size() == 1) {
    cv.notify_all();
  }
}

template <typename T>
std::vector<T> synchronized_queue<T>::flush() {
  // TODO (Part A, Step 3): IMPLEMENT
  std::unique_lock lock(mtx);
  std::vector<T> res;
  size_t size = this->q.size();
  for(int i = 0; i < size; i++) {
    // Grabs the first element in the queue and inserts it into vector
    T elem = this->q.front();
    res.push_back(elem);
    // Removes that element from the queue
    this->q.pop();
  }
  return res;
}

template <typename T>
void synchronized_queue<T>::stop() {
  is_stopped = true;
  cv.notify_all();
}

// NOTE: DO NOT TOUCH! Why is this necessary? Because C++ is a beautiful
// language:
// https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
template class synchronized_queue<int>;
template class synchronized_queue<std::shared_ptr<ClientConn>>;
