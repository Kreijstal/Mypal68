/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "gfxGdkNativeRenderer.h"
#include "gfxContext.h"
#include "X11UndefineNone.h"
#include "gfxPlatformGtk.h"
#include "X11UndefineNone.h"

#ifdef MOZ_X11
#  include <gdk/gdkx.h>
#  include "X11UndefineNone.h"
#  include "cairo-xlib.h"
#  include "gfxXlibSurface.h"

#endif
