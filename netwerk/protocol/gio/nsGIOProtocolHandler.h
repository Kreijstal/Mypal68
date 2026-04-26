/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsGIOProtocolHandler_h__
#define nsGIOProtocolHandler_h__

#include "mozilla/AlreadyAddRefed.h"
#include "mozilla/StaticPtr.h"
#include "nsCOMPtr.h"
#include "nsIObserver.h"
#include "nsIProtocolHandler.h"
#include "nsString.h"

class nsIPrefBranch;

class nsGIOProtocolHandler final : public nsIProtocolHandler,
                                   public nsIObserver {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSIPROTOCOLHANDLER
  NS_DECL_NSIOBSERVER

  static already_AddRefed<nsGIOProtocolHandler> GetSingleton();

  nsresult Init();
  bool IsSupportedProtocol(const nsCString& aSpec);

 private:
  ~nsGIOProtocolHandler() = default;

  void InitSupportedProtocolsPref(nsIPrefBranch* prefs);

  nsCString mSupportedProtocols;

  static mozilla::StaticRefPtr<nsGIOProtocolHandler> sSingleton;
};

#endif
