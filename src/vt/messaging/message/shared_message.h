/*
//@HEADER
// *****************************************************************************
//
//                               shared_message.h
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

#if !defined INCLUDED_MESSAGING_MESSAGE_SHARED_MESSAGE_H
#define INCLUDED_MESSAGING_MESSAGE_SHARED_MESSAGE_H

#include "vt/config.h"
#include "vt/messaging/envelope.h"
#include "vt/messaging/message/smart_ptr.h"
#include "vt/pool/pool.h"

namespace vt {

template <typename MessageT, typename... Args>
MessageT* makeSharedMessage(Args&&... args);

template <typename MsgT, typename... Args>
MsgSharedPtr<MsgT> makeMessage(Args&&... args);

template <typename MsgT, typename... Args>
MsgSharedPtr<MsgT> makeMsg(Args&&... args);

template <typename MessageT, typename... Args>
MessageT* makeSharedMessageSz(std::size_t size, Args&&... args);

template <typename MsgT, typename... Args>
MsgSharedPtr<MsgT> makeMessageSz(std::size_t size, Args&&... args);

template <typename MessageT>
void messageConvertToShared(MessageT* msg);

template <typename MessageT>
void messageSetUnmanaged(MessageT* msg);

template <typename MsgPtrT>
void messageResetDeserdes(MsgPtrT const& msg);

} //end namespace vt

#include "vt/messaging/message/shared_message.impl.h"

#endif /*INCLUDED_MESSAGING_MESSAGE_SHARED_MESSAGE_H*/
