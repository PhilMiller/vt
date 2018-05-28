
#if !defined INCLUDED_VRT_PROXY_COLLECTION_WRAPPER_H
#define INCLUDED_VRT_PROXY_COLLECTION_WRAPPER_H

#include "config.h"
#include "vrt/collection/send/sendable.h"
#include "vrt/collection/destroy/destroyable.h"
#include "vrt/proxy/proxy_element.h"
#include "vrt/proxy/proxy_collection.h"
#include "vrt/proxy/base_wrapper.h"

namespace vt { namespace vrt { namespace collection {

/*
 * `CollectionProxy': variant w/o IndexT baked into class. Thus, all accesses
 * require that IndexT be determinable (so the index cannot be created on the
 * fly).
 */

struct CollectionProxy {
  template <typename ColT, typename IndexT>
  using ElmProxyType = VrtElmProxy<ColT, IndexT>;

  CollectionProxy() = default;
  CollectionProxy(VirtualProxyType const in_proxy);

  VirtualProxyType getProxy() const;

  template <typename ColT, typename IndexT>
  ElmProxyType<ColT, IndexT> index(IndexT const& idx);
  template <typename ColT, typename IndexT>
  ElmProxyType<ColT, IndexT> operator[](IndexT const& idx);
  template <typename ColT, typename IndexT>
  ElmProxyType<ColT, IndexT> operator()(IndexT const& idx);

private:
  VirtualProxyType const proxy_ = no_vrt_proxy;
};

/*
 * `CollectionIndexProxy' (variant with IndexT baked into class, allowing
 * constructors to be forwarded for building indicies in line without the type.
 */

template <typename ColT, typename IndexT>
struct CollectionIndexProxy : Broadcastable<ColT, IndexT> {
  using ElmProxyType = VrtElmProxy<ColT, IndexT>;

  CollectionIndexProxy() = default;
  CollectionIndexProxy(VirtualProxyType const in_proxy);

  template <typename... IndexArgsT>
  ElmProxyType index_build(IndexArgsT&&... idx);
  template <typename... IndexArgsT>
  ElmProxyType operator[](IndexArgsT&&... idx);
  template <typename... IndexArgsT>
  ElmProxyType operator()(IndexArgsT&&... idx);

  ElmProxyType index(IndexT const& idx);
  ElmProxyType operator[](IndexT const& idx);
  ElmProxyType operator()(IndexT const& idx);
};


}}} /* end namespace vt::vrt::collection */

#include "vrt/proxy/collection_wrapper.impl.h"

#endif /*INCLUDED_VRT_PROXY_COLLECTION_WRAPPER_H*/
