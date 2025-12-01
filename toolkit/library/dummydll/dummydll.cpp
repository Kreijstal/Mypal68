/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Include C++ standard library headers first to ensure templates are
// processed before windows.h's extern "C" blocks
#include <new>
#include <cstdlib>

// Now safe to include windows.h
#include <windows.h>

BOOL WINAPI DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpvReserved) {
  if (dwReason == DLL_PROCESS_ATTACH) {
    ::DisableThreadLibraryCalls((HMODULE)hModule);
  }
  return TRUE;
}
