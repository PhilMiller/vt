
#include "config.h"
#include "concurrent_deque.h"

#include <deque>
#include <mutex>
#include <cassert>

namespace vt { namespace util { namespace container {

template <typename T>
void ConcurrentDeque<T>::emplaceBack(T&& elm) {
  std::lock_guard<std::mutex> guard(container_mutex_);
  container_.emplace_back(std::forward<T>(elm));
}

template <typename T>
void ConcurrentDeque<T>::emplaceFront(T&& elm) {
  std::lock_guard<std::mutex> guard(container_mutex_);
  container_.emplace_front(std::forward<T>(elm));
}

template <typename T>
typename ConcurrentDeque<T>::TConstRef ConcurrentDeque<T>::front() const {
  std::lock_guard<std::mutex> guard(container_mutex_);
  return container_.front();
}

template <typename T>
typename ConcurrentDeque<T>::TConstRef ConcurrentDeque<T>::back() const {
  std::lock_guard<std::mutex> guard(container_mutex_);
  return container_.back();
}

template <typename T>
typename ConcurrentDeque<T>::TRef ConcurrentDeque<T>::at(SizeType const& pos) {
  std::lock_guard<std::mutex> guard(container_mutex_);
  return container_.at(pos);
}

template <typename T>
typename ConcurrentDeque<T>::TConstRef ConcurrentDeque<T>::at(
  SizeType const& pos
) const {
  std::lock_guard<std::mutex> guard(container_mutex_);
  return container_.at(pos);
}

template <typename T>
typename ConcurrentDeque<T>::TRef ConcurrentDeque<T>::front() {
  std::lock_guard<std::mutex> guard(container_mutex_);
  return container_.front();
}

template <typename T>
typename ConcurrentDeque<T>::TRef ConcurrentDeque<T>::back() {
  std::lock_guard<std::mutex> guard(container_mutex_);
  return container_.back();
}

template <typename T>
void ConcurrentDeque<T>::popFront() {
  std::lock_guard<std::mutex> guard(container_mutex_);
  return container_.pop_front();
}

template <typename T>
void ConcurrentDeque<T>::popBack() {
  std::lock_guard<std::mutex> guard(container_mutex_);
  return container_.pop_back();
}

template <typename T>
typename ConcurrentDeque<T>::SizeType ConcurrentDeque<T>::size() const {
  std::lock_guard<std::mutex> guard(container_mutex_);
  return container_.size();
}

}}} //end namespace vt::util::container

