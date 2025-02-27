/*
//@HEADER
// *****************************************************************************
//
//                            serialize_interface.h
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

#if !defined INCLUDED_SERIALIZATION_SERIALIZE_INTERFACE_H
#define INCLUDED_SERIALIZATION_SERIALIZE_INTERFACE_H

#include <cstdlib>
#include <functional>
#include <memory>

#if HAS_SERIALIZATION_LIBRARY
  #include "serialization_library_headers.h"
#else

namespace serialization { namespace interface {

using SizeType = size_t;
using SerialByteType = char;

using BufferCallbackType = std::function<SerialByteType*(SizeType size)>;

struct SerializedInfo {
  virtual SizeType getSize() const = 0;
  virtual SerialByteType* getBuffer() const = 0;
  virtual ~SerializedInfo() { }
};

using SerializedInfoPtrType = std::unique_ptr<SerializedInfo>;
using SerializedReturnType = SerializedInfoPtrType;

template <typename T>
SerializedReturnType serialize(T& target, BufferCallbackType fn = nullptr);

template <typename T>
T* deserialize(SerialByteType* buf, SizeType size, T* user_buf = nullptr);

template <typename T>
SerializedReturnType serializePartial(T& target, BufferCallbackType fn = nullptr);

template <typename T>
T* deserializePartial(SerialByteType* buf, SizeType size, T* user_buf = nullptr);


}} /* end namespace serialization::interface */

#include "vt/serialize_interface.impl.h"

#endif

#endif /*INCLUDED_SERIALIZATION_SERIALIZE_INTERFACE_H*/

