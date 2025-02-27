/*
//@HEADER
// *****************************************************************************
//
//                                lbable.impl.h
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

#if !defined INCLUDED_VT_VRT_COLLECTION_BALANCE_PROXY_LBABLE_IMPL_H
#define INCLUDED_VT_VRT_COLLECTION_BALANCE_PROXY_LBABLE_IMPL_H

#include "vt/config.h"
#include "vt/vrt/collection/balance/proxy/lbable.h"
#include "vt/vrt/collection/manager.h"

namespace vt { namespace vrt { namespace collection {

template <typename ColT, typename IndexT, typename BaseProxyT>
LBable<ColT,IndexT,BaseProxyT>::LBable(
  typename BaseProxyT::ProxyType const& in_proxy,
  typename BaseProxyT::ElementProxyType const& in_elm
) : BaseProxyT(in_proxy, in_elm)
{ }

template <typename ColT, typename IndexT, typename BaseProxyT>
template <typename SerializerT>
void LBable<ColT,IndexT,BaseProxyT>::serialize(SerializerT& s) {
  BaseProxyT::serialize(s);
}

template <typename ColT, typename IndexT, typename BaseProxyT>
template <typename MsgT, ActiveColMemberTypedFnType<MsgT,ColT> f>
void LBable<ColT,IndexT,BaseProxyT>::LBsync(MsgT* msg, PhaseType p) const {
  auto col_proxy = this->getCollectionProxy();
  auto elm_proxy = this->getElementProxy();
  auto proxy = VrtElmProxy<ColT, IndexT>(col_proxy,elm_proxy);
  return theCollection()->elmReadyLB<MsgT,ColT,f>(proxy,p,msg,true);
}


template <typename ColT, typename IndexT, typename BaseProxyT>
template <
  typename MsgT, ActiveColMemberTypedFnType<MsgT,ColT> f, typename... Args
>
void LBable<ColT,IndexT,BaseProxyT>::LBsync(Args&&... args) const {
  return LBsync<MsgT,f>(makeMessage<MsgT>(args...));
}


template <typename ColT, typename IndexT, typename BaseProxyT>
template <typename MsgT, ActiveColMemberTypedFnType<MsgT,ColT> f>
void LBable<ColT,IndexT,BaseProxyT>::LBsync(
  MsgSharedPtr<MsgT> msg, PhaseType p
) const {
  return LBsync<MsgT,f>(msg.get(),p);
}

template <typename ColT, typename IndexT, typename BaseProxyT>
void LBable<ColT,IndexT,BaseProxyT>::LBsync(
  FinishedLBType cont, PhaseType p
) const {
  auto col_proxy = this->getCollectionProxy();
  auto elm_proxy = this->getElementProxy();
  auto proxy = VrtElmProxy<ColT, IndexT>(col_proxy,elm_proxy);
  return theCollection()->elmReadyLB<ColT>(proxy,p,true,cont);
}

template <typename ColT, typename IndexT, typename BaseProxyT>
template <typename MsgT, ActiveColMemberTypedFnType<MsgT,ColT> f>
void LBable<ColT,IndexT,BaseProxyT>::LB(MsgT* msg, PhaseType p) const {
  auto col_proxy = this->getCollectionProxy();
  auto elm_proxy = this->getElementProxy();
  auto proxy = VrtElmProxy<ColT, IndexT>(col_proxy,elm_proxy);
  return theCollection()->elmReadyLB<MsgT,ColT,f>(proxy,p,msg,false);
}

template <typename ColT, typename IndexT, typename BaseProxyT>
template <
  typename MsgT, ActiveColMemberTypedFnType<MsgT,ColT> f, typename... Args
>
void LBable<ColT,IndexT,BaseProxyT>::LB(Args&&... args) const {
  return LB<MsgT,f>(makeMessage<MsgT>(args...));
}

template <typename ColT, typename IndexT, typename BaseProxyT>
template <typename MsgT, ActiveColMemberTypedFnType<MsgT,ColT> f>
void LBable<ColT,IndexT,BaseProxyT>::LB(
  MsgSharedPtr<MsgT> msg, PhaseType p
) const {
  return LB<MsgT,f>(msg.get(),p);
}

template <typename ColT, typename IndexT, typename BaseProxyT>
void LBable<ColT,IndexT,BaseProxyT>::LB(
  FinishedLBType cont, PhaseType p
) const {
  auto col_proxy = this->getCollectionProxy();
  auto elm_proxy = this->getElementProxy();
  auto proxy = VrtElmProxy<ColT, IndexT>(col_proxy,elm_proxy);
  return theCollection()->elmReadyLB<ColT>(proxy,p,false,cont);
}


}}} /* end namespace vt::vrt::collection */

#endif /*INCLUDED_VT_VRT_COLLECTION_BALANCE_PROXY_LBABLE_IMPL_H*/
