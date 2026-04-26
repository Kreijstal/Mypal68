/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MOZILLA_GFX_FONTVARIATION_H_
#define MOZILLA_GFX_FONTVARIATION_H_

#include <cstddef>  // offsetof()
#include <stdint.h>
#include "mozilla/FloatingPoint.h"

namespace mozilla::gfx {

// An OpenType variation tag and value pair
struct FontVariation {
  uint32_t mTag;
  float mValue;

  bool operator==(const FontVariation& aOther) const {
    return mTag == aOther.mTag &&
           NumbersAreBitwiseIdentical(mValue, aOther.mValue);
  }
};

#define GFX_FONT_STATIC_ASSERT_TYPE_LAYOUTS_MATCH(T1, T2)  \
  static_assert(sizeof(T1) == sizeof(T2),                  \
                "Size mismatch between " #T1 " and " #T2); \
  static_assert(alignof(T1) == alignof(T2),                \
                "Align mismatch between " #T1 " and " #T2);

#define GFX_FONT_STATIC_ASSERT_FIELD_OFFSET_MATCHES(T1, T2, field) \
  static_assert(offsetof(T1, field) == offsetof(T2, field),        \
                "Field offset mismatch of " #field " between " #T1 \
                " and " #T2);

/**
 * <div rustbindgen="true" replaces="mozilla::gfx::FontVariation">
 */
struct FontVariation_Simple {
  uint32_t mTag;
  float mValue;
};

GFX_FONT_STATIC_ASSERT_TYPE_LAYOUTS_MATCH(FontVariation, FontVariation_Simple);
GFX_FONT_STATIC_ASSERT_FIELD_OFFSET_MATCHES(FontVariation,
                                            FontVariation_Simple, mTag);
GFX_FONT_STATIC_ASSERT_FIELD_OFFSET_MATCHES(FontVariation,
                                            FontVariation_Simple, mValue);

#undef GFX_FONT_STATIC_ASSERT_TYPE_LAYOUTS_MATCH
#undef GFX_FONT_STATIC_ASSERT_FIELD_OFFSET_MATCHES

}  // namespace mozilla::gfx

#endif /* MOZILLA_GFX_FONTVARIATION_H_ */
