/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_ServoComputedDataInlines_h
#define mozilla_ServoComputedDataInlines_h

#include "mozilla/ServoComputedData.h"
#include "nsStyleStruct.h"

#ifndef RUST_BINDGEN
#  define STYLE_STRUCT(name_)                                        \
  const nsStyle##name_* ServoComputedData::GetStyle##name_() const { \
    return &name_.mPtr->gecko.mInner;                                \
  }
#  include "nsStyleStructList.h"
#  undef STYLE_STRUCT
#endif

#endif  // mozilla_ServoComputedDataInlines_h
