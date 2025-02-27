/*
//@HEADER
// *****************************************************************************
//
//                                 smart_ptr.h
//                           DARMA Toolkit v. 1.0.0
//                       DARMA/vt => Virtual Transport
//
// Copyright 2019 National Technology & Engineering Solutions of Sandia, LLC
// (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact darma@sandia.gov
//
// *****************************************************************************
//@HEADER
*/

#if !defined INCLUDED_MESSAGING_MESSAGE_SMART_PTR_H
#define INCLUDED_MESSAGING_MESSAGE_SMART_PTR_H

#include "vt/config.h"
#include "vt/messaging/message/message.h"
#include "vt/messaging/message/refs.h"
#include "vt/messaging/message/smart_ptr_virtual.h"

#include <iostream>

#include "fmt/ostream.h"

namespace vt { namespace messaging {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static struct MsgInitOwnerType { } MsgInitOwnerTag { };
static struct MsgInitNonOwnerType { } MsgInitNonOwnerTag { };
#pragma GCC diagnostic pop

template <typename T>
struct MsgSharedPtr final {
  using MsgType        = T;
  using MsgPtrType     = MsgType*;
  using BaseMsgType    = BaseMessage;
  using BaseMsgPtrType = BaseMsgType*;

  MsgSharedPtr(MsgInitOwnerType, T* in)
    : ptr_(in)
  {
    if (in) {
      shared_ = isSharedMessage<T>(in);
      if (shared_) {
        vtAssertInfo(
          envelopeGetRef(in->env) > 0, "owner: ref must be greater than 0",
          shared_, envelopeGetRef(in->env)
        );
      }
    }
  }
  MsgSharedPtr(MsgInitNonOwnerType, T* in)
    : ptr_(in)
  {
    if (in) {
      shared_ = isSharedMessage<T>(in);
      if (shared_) {
        vtAssertInfo(
          envelopeGetRef(in->env) > 0, "non-owner: ref must be greater than 0",
          shared_, envelopeGetRef(in->env)
        );
        messageRef(in);
      }
    }
  }
  MsgSharedPtr(std::nullptr_t) : ptr_(nullptr) { }
  MsgSharedPtr(MsgSharedPtr<T> const& in) : ptr_(in.ptr_) { ref(); }
  MsgSharedPtr(MsgSharedPtr<T>&& in)      : ptr_(in.ptr_) { ref(); }

  template <typename U>
  explicit MsgSharedPtr(MsgSharedPtr<U> const& in) : ptr_(in.get()) { ref(); }

  ~MsgSharedPtr() {
    clear();
  }

  template <typename U>
  explicit operator MsgSharedPtr<U>() const { return to<U>(); }
  explicit operator MsgPtrType() const { return get(); }

  template <typename U>
  MsgSharedPtr<U> to() const {
    static_assert(
      std::is_trivially_destructible<T>(),
      "Message shall not be downcast unless trivially destructible"
    );

    return MsgSharedPtr<U>{*this};
  }

  template <typename U>
  MsgVirtualPtr<U> toVirtual() const {
    return MsgVirtualPtr<U>(*this);
  }

  MsgSharedPtr<T>& operator=(std::nullptr_t) { clear(); return *this; }
  MsgSharedPtr<T>& operator=(MsgSharedPtr<T> const& n) { set(n); return *this; }
  bool operator==(MsgSharedPtr<T> const& n) const { return ptr_ == n.ptr_; }
  bool operator!=(MsgSharedPtr<T> const& n) const { return ptr_ != n.ptr_; }
  bool operator==(std::nullptr_t) const           { return ptr_ == nullptr; }
  bool operator!=(std::nullptr_t) const           { return ptr_ != nullptr; }

  inline bool valid() const { return ptr_ != nullptr; }
  inline void set(MsgSharedPtr<T> const& n) {
    clear();
    if (n.valid()) {
      shared_ = isSharedMessage<T>(n.get());
      if (shared_) {
        vtAssertInfo(
          envelopeGetRef(n.get()->env) > 0, "Bad Ref (before ref set)",
          shared_, envelopeGetRef(n.get()->env)
        );
        debug_print(
          pool, node,
          "MsgSmartPtr: (auto) set(), ptr={}, ref={}, address={}\n",
          print_ptr(n.get()), envelopeGetRef(n.get()->env), print_ptr(this)
        );
        messageRef(n.get());
      }
      ptr_ = n.get();
    }
  }
  inline void ref() {
    if (valid()) {
      shared_ = isSharedMessage<T>(get());
      if (shared_) {
        vtAssertInfo(
          envelopeGetRef(get()->env) > 0, "Bad Ref (before ref)",
          shared_, envelopeGetRef(get()->env)
        );
        debug_print(
          pool, node,
          "MsgSmartPtr: (auto) ref(), ptr={}, ref={}, address={}\n",
          print_ptr(get()), envelopeGetRef(get()->env), print_ptr(this)
        );
        messageRef(get());
      }
    }
  }

  inline T* operator->() const { return get(); }
  inline T& operator*() const { return *ptr_; }
  inline T* get() const {
    return ptr_ ? reinterpret_cast<MsgPtrType>(ptr_) : nullptr;
  }

  friend std::ostream& operator<<(std::ostream&os, MsgSharedPtr<T> const& m) {
    auto nrefs = m.valid() && m.shared_ ? envelopeGetRef(m.get()->env) : -1;
    return os << "MsgSharedPtr("
              <<              m.ptr_    << ","
              << "shared=" << m.shared_ << ","
              << "ref="    << nrefs
              << ")";
  }

protected:
  inline void set(T* t) {
    clear();
    debug_print(
      pool, node,
      "MsgSmartPtr: (auto) set() BARE, ptr={}, ref={}, address={}\n",
      print_ptr(t), envelopeGetRef(t->env), print_ptr(this)
    );
    setPtr(t);
  }

  inline void setPtr(T* t) {
    ptr_ = reinterpret_cast<BaseMsgPtrType>(t);
  }

  inline void clear() {
    if (valid()) {
      if (shared_) {
        vtAssertInfo(
          envelopeGetRef(get()->env) > 0, "Bad Ref (before deref)",
          shared_, envelopeGetRef(get()->env)
        );
        debug_print(
          pool, node,
          "MsgSmartPtr: (auto) clear(), ptr={}, ref={}, address={}\n",
          print_ptr(get()), envelopeGetRef(get()->env), print_ptr(this)
        );
        messageDeref(get());
      }
    }
    ptr_ = nullptr;
  }

private:
  BaseMsgPtrType ptr_ = nullptr;
  bool shared_        = true;
};

}} /* end namespace vt::messaging */

namespace vt {

template <typename T>
using MsgSharedPtr = messaging::MsgSharedPtr<T>;

template <typename T>
inline messaging::MsgSharedPtr<T> promoteMsgOwner(T* const msg) {
  msg->has_owner_ = true;
  return MsgSharedPtr<T>{messaging::MsgInitOwnerTag,msg};
}

template <typename T>
inline messaging::MsgSharedPtr<T> promoteMsg(MsgSharedPtr<T> msg) {
  vtAssert(msg->has_owner_, "promoteMsg shared ptr must have owner");
  return MsgSharedPtr<T>{messaging::MsgInitNonOwnerTag,msg.get()};
}

template <typename T>
inline messaging::MsgSharedPtr<T> promoteMsg(T* msg) {
  if (!msg->has_owner_) {
    return promoteMsgOwner(msg);
  } else {
    return MsgSharedPtr<T>{messaging::MsgInitNonOwnerTag,msg};
  }
}

} /* end namespace vt */


#endif /*INCLUDED_MESSAGING_MESSAGE_SMART_PTR_H*/
