
#if !defined INCLUDED_VRT_COLLECTION_DESTROY_DESTROYABLE_IMPL_H
#define INCLUDED_VRT_COLLECTION_DESTROY_DESTROYABLE_IMPL_H

#include "vt/config.h"
#include "vt/vrt/collection/destroy/destroyable.h"
#include "vt/vrt/proxy/base_collection_proxy.h"
#include "vt/vrt/collection/manager.h"

namespace vt { namespace vrt { namespace collection {

template <typename ColT, typename IndexT, typename BaseProxyT>
Destroyable<ColT,IndexT,BaseProxyT>::Destroyable(
  VirtualProxyType const in_proxy
) : BaseProxyT(in_proxy)
{ }

template <typename ColT, typename IndexT, typename BaseProxyT>
void Destroyable<ColT,IndexT,BaseProxyT>::destroy() const {
  return theCollection()->destroy<ColT,IndexT>(this->getProxy());
}

}}} /* end namespace vt::vrt::collection */

#endif /*INCLUDED_VRT_COLLECTION_DESTROY_DESTROYABLE_IMPL_H*/
