/*
//@HEADER
// *****************************************************************************
//
//                                  read_lb.cc
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

#include "vt/config.h"
#include "vt/context/context.h"
#include "vt/vrt/collection/balance/read_lb.h"
#include "vt/vrt/collection/balance/lb_type.h"

#include <string>
#include <fstream>
#include <cassert>
#include <cctype>
#include <cmath>

namespace vt { namespace vrt { namespace collection { namespace balance {

/*static*/ std::string ReadLBSpec::filename = {};
/*static*/ SpecIndex ReadLBSpec::num_entries_ = 0;
/*static*/ std::unordered_map<SpecIndex,SpecEntry> ReadLBSpec::spec_ = {};
/*static*/ bool ReadLBSpec::has_spec_ = false;
/*static*/ bool ReadLBSpec::read_complete_ = false;

/*static*/ bool ReadLBSpec::openFile(std::string const name) {
  std::ifstream file(name);
  filename = name;
  has_spec_ = file.good();
  return file.good();
}

/*static*/ LBType ReadLBSpec::getLB(SpecIndex const& idx) {
  auto const lb = entry(idx);
  if (lb) {
    return lb->getLB();
  } else {
    return LBType::NoLB;
  }
}

/*static*/ SpecEntry const* ReadLBSpec::entry(SpecIndex const& idx) {
  auto spec_iter = spec_.find(idx);
  debug_print(
    lb, node,
    "idx={},idx-mod={},found={},size={},num_entries={}\n",
    idx,
    idx % num_entries_,
    spec_iter != spec_.end(),
    spec_.size(),
    num_entries_
  );
  if (spec_iter == spec_.end()) {
    auto idx2 = idx;
    if (idx2 > num_entries_) {
      auto const div = (idx2 - num_entries_) / num_entries_;
      idx2 -= div;
    }
    while (idx2 > num_entries_) {
      idx2 -= num_entries_;
    }
    spec_iter = spec_.find(idx2/* % num_entries_*/);
    if (spec_iter != spec_.end()) {
      return &spec_iter->second;
    }
  } else {
    return &spec_iter->second;
  }
  return nullptr;
}

/*static*/ void ReadLBSpec::readFile() {
  if (read_complete_ || !has_spec_) {
    return;
  }

  std::ifstream file(filename);
  vtAssert(file.good(), "must be valid");

  constexpr int64_t const max_num_times = 100000;
  int64_t max_entry = 0;
  int64_t num_times = 0;
  std::string cur_line;
  while (!file.eof()) {
    double lb_min = 0.0f, lb_max = 0.0f;
    int64_t lb_iter = -1;
    std::string lb_name;

    if (std::isalpha(file.peek())) {
      file >> lb_name;
      lb_iter = max_entry;
      max_entry++;
    } else {
      file >> lb_iter;
      file >> lb_name;
      max_entry = std::max(max_entry, lb_iter);
    }
    if (file.peek() != '\n') {
      file >> lb_min;
    }
    if (file.peek() != '\n') {
      file >> lb_max;
    }

    bool valid_lb_found = false;
    for (auto&& elm : lb_names_) {
      if (lb_name == elm.second) {
        valid_lb_found = true;
      }
    }

    if (valid_lb_found) {
      auto spec_index = static_cast<SpecIndex>(lb_iter);
      spec_.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(spec_index),
        std::forward_as_tuple(SpecEntry{spec_index,lb_name,lb_min,lb_max})
      );
      debug_print(
        lb, node,
        "{}: insert: idx={},size={},name={},min={},max={}\n",
        theContext()->getNode(),
        spec_index,
        spec_.size(),
        lb_name,
        lb_min,
        lb_max
      );
    } else {
      auto const& this_node = theContext()->getNode();
      if (this_node == 0 && lb_name != "") {
        vt_print(
          lb,
          "valid LB not found: \"name={},iter={},min={},max={}\"\n",
          this_node, lb_name, lb_iter, lb_min, lb_max
        );
      }
    }

    num_times++;

    if (num_times > max_num_times) {
      break;
    }
  }

  num_entries_ = spec_.size();

  for (auto&& elm : spec_) {
    num_entries_ = std::max(num_entries_, elm.second.getIdx());
  }

  read_complete_ = true;
}

}}}} /* end namespace vt::vrt::collection::balance */
