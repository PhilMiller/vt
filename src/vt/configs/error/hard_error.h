/*
//@HEADER
// *****************************************************************************
//
//                                 hard_error.h
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

#if !defined INCLUDED_CONFIGS_ERROR_HARD_ERROR_H
#define INCLUDED_CONFIGS_ERROR_HARD_ERROR_H

/*
 *  A hard error is always checked and leads to failure in any mode if
 *  triggered
 */

#include "vt/configs/debug/debug_config.h"
#include "vt/configs/types/types_type.h"
#include "vt/configs/error/common.h"
#include "vt/configs/error/error.h"

#include <string>
#include <tuple>
#include <type_traits>

#if backend_check_enabled(production)
  #define vtAbort(str)                                                \
    ::vt::error::display(str,1);
  #define vtAbortCode(xy,str)                                         \
    ::vt::error::display(str,xy);
#else
  #define vtAbort(str)                                                \
    ::vt::error::displayLoc(str,1, DEBUG_LOCATION,std::make_tuple());
  #define vtAbortCode(xy,str)                                         \
    ::vt::error::displayLoc(str,xy,DEBUG_LOCATION,std::make_tuple());
#endif

#define vtAbortIf(cond,str)                                             \
  do {                                                                  \
    if ((cond)) {                                                       \
      vtAbort(str);                                                     \
    }                                                                   \
  } while (false)
#define vtAbortIfCode(code,cond,str)                                    \
  do {                                                                  \
    if ((cond)) {                                                       \
      vtAbortCode(code,str);                                            \
    }                                                                   \
  } while (false)

#define vtAbortIfNot(cond,str)                                          \
  vtAbortIf(INVERT_COND(cond),str)
#define vtAbortIfNotCode(code,cond,str)                                 \
  vtAbortIfCode(code,INVERT_COND(cond),str)

#endif /*INCLUDED_CONFIGS_ERROR_HARD_ERROR_H*/
