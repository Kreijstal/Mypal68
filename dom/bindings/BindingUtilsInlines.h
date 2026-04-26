/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_BindingUtilsInlines_h__
#define mozilla_dom_BindingUtilsInlines_h__

#include "mozilla/dom/DOMJSClass.h"

namespace mozilla {
namespace dom {

inline bool IsDOMIfaceAndProtoClass(const JSClass* clasp) {
  return clasp->flags & JSCLASS_IS_DOMIFACEANDPROTOJSCLASS;
}

inline const DOMJSClass* GetDOMClass(const JSClass* clasp) {
  return IsDOMClass(clasp) ? DOMJSClass::FromJSClass(clasp) : nullptr;
}

inline const DOMJSClass* GetDOMClass(JSObject* obj) {
  return GetDOMClass(JS::GetClass(obj));
}

inline nsISupports* UnwrapDOMObjectToISupports(JSObject* aObject) {
  const DOMJSClass* clasp = GetDOMClass(aObject);
  if (!clasp || !clasp->mDOMObjectIsISupports) {
    return nullptr;
  }

  return UnwrapPossiblyNotInitializedDOMObject<nsISupports>(aObject);
}

inline void AllocateProtoAndIfaceCache(JSObject* obj,
                                       ProtoAndIfaceCache::Kind aKind) {
  MOZ_ASSERT(JS::GetClass(obj)->flags & JSCLASS_DOM_GLOBAL);
  MOZ_ASSERT(JS::GetReservedSlot(obj, DOM_PROTOTYPE_SLOT).isUndefined());

  ProtoAndIfaceCache* protoAndIfaceCache = new ProtoAndIfaceCache(aKind);

  JS::SetReservedSlot(obj, DOM_PROTOTYPE_SLOT,
                      JS::PrivateValue(protoAndIfaceCache));
}

inline void TraceProtoAndIfaceCache(JSTracer* trc, JSObject* obj) {
  MOZ_ASSERT(JS::GetClass(obj)->flags & JSCLASS_DOM_GLOBAL);

#ifdef DEBUG
  if (trc->kind() == JS::TracerKind::VerifyTraceProtoAndIface) {
    static_cast<VerifyTraceProtoAndIfaceCacheCalledTracer*>(trc)->ok = true;
    return;
  }
#endif

  if (!DOMGlobalHasProtoAndIFaceCache(obj)) {
    return;
  }
  ProtoAndIfaceCache* protoAndIfaceCache = GetProtoAndIfaceCache(obj);
  protoAndIfaceCache->Trace(trc);
}

inline void DestroyProtoAndIfaceCache(JSObject* obj) {
  MOZ_ASSERT(JS::GetClass(obj)->flags & JSCLASS_DOM_GLOBAL);

  if (!DOMGlobalHasProtoAndIFaceCache(obj)) {
    return;
  }

  ProtoAndIfaceCache* protoAndIfaceCache = GetProtoAndIfaceCache(obj);

  delete protoAndIfaceCache;
}

inline bool XrayGetNativeProto(JSContext* cx, JS::Handle<JSObject*> obj,
                               JS::MutableHandle<JSObject*> protop) {
  JS::Rooted<JSObject*> global(cx, JS::GetNonCCWObjectGlobal(obj));
  {
    JSAutoRealm ar(cx, global);
    const DOMJSClass* domClass = GetDOMClass(obj);
    if (domClass) {
      ProtoHandleGetter protoGetter = domClass->mGetProto;
      if (protoGetter) {
        protop.set(protoGetter(cx));
      } else {
        protop.set(JS::GetRealmObjectPrototype(cx));
      }
    } else if (JS_ObjectIsFunction(obj)) {
      MOZ_ASSERT(JS_IsNativeFunction(obj, Constructor));
      protop.set(JS::GetRealmFunctionPrototype(cx));
    } else {
      const JSClass* clasp = JS::GetClass(obj);
      MOZ_ASSERT(IsDOMIfaceAndProtoClass(clasp));
      ProtoGetter protoGetter =
          DOMIfaceAndProtoJSClass::FromJSClass(clasp)->mGetParentProto;
      protop.set(protoGetter(cx));
    }
  }

  return JS_WrapObject(cx, protop);
}

inline bool IsDOMConstructor(JSObject* obj) {
  if (JS_IsNativeFunction(obj, dom::Constructor)) {
    return true;
  }

  const JSClass* clasp = JS::GetClass(obj);
  return dom::IsDOMIfaceAndProtoClass(clasp) &&
         dom::DOMIfaceAndProtoJSClass::FromJSClass(clasp)->mType ==
             dom::eInterface;
}

#ifdef DEBUG
inline bool HasConstructor(JSObject* obj) {
  return JS_IsNativeFunction(obj, Constructor) ||
         JS::GetClass(obj)->getConstruct();
}
#endif

}  // namespace dom
}  // namespace mozilla

#endif /* mozilla_dom_BindingUtilsInlines_h__ */
