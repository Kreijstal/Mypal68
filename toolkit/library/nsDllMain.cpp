/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Include C++ standard library headers first to ensure templates are
// processed before windows.h's extern "C" blocks
#include <new>
#include <cstdlib>

// Now safe to include windows.h
#include <windows.h>
#include "nsToolkit.h"

#if defined(__GNUC__)
// If DllMain gets name mangled, it won't be seen.
extern "C" {
#endif

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD reason, LPVOID lpReserved) {
  switch (reason) {
    case DLL_PROCESS_ATTACH:
      nsToolkit::Startup((HINSTANCE)hModule);
      break;

    case DLL_THREAD_ATTACH:
      break;

    case DLL_THREAD_DETACH:
      break;

    case DLL_PROCESS_DETACH:
      nsToolkit::Shutdown();
      break;
  }

  return TRUE;
}

#if defined(__GNUC__)
}  // extern "C"
#endif
