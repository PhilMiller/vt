/*
//@HEADER
// *****************************************************************************
//
//                                   atomic.h
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

#if !defined INCLUDED_UTILS_ATOMIC_ATOMIC_H
#define INCLUDED_UTILS_ATOMIC_ATOMIC_H

#include "vt/config.h"

#if backend_check_enabled(openmp)
  #include "vt/utils/atomic/omp_atomic.h"
#elif backend_check_enabled(stdthread)
  #include "vt/utils/atomic/std_atomic.h"
#elif backend_no_threading
  #include "vt/utils/atomic/null_atomic.h"
#else
  backend_static_assert_unreachable
#endif

namespace vt { namespace util { namespace atomic {

#if backend_check_enabled(openmp)
  template <typename T>
  using AtomicType = AtomicOMP<T>;
#elif backend_check_enabled(stdthread)
  template <typename T>
  using AtomicType = AtomicSTD<T>;
#elif backend_no_threading
  template <typename T>
  using AtomicType = AtomicNull<T>;
#else
  backend_static_assert_unreachable
#endif

}}} /* end namespace vt::util::atomic */

#endif /*INCLUDED_UTILS_ATOMIC_ATOMIC_H*/
