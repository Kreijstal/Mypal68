/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

//! Element an snapshot common logic.

use crate::gecko_bindings::bindings;
use crate::gecko_bindings::structs::{self, nsAtom};
use crate::string_cache::WeakAtom;
use crate::values::AtomIdent;
use crate::Atom;
use crate::CaseSensitivityExt;
use selectors::attr::CaseSensitivity;

/// A function that, given an element of type `T`, allows you to get a single
/// class or a class list.
enum Class<'a> {
    None,
    One(*const nsAtom),
    More(&'a [structs::RefPtr<nsAtom>]),
}

#[inline(always)]
unsafe fn ptr<T>(attr: &structs::nsAttrValue) -> *const T {
    (attr.mBits & !structs::NS_ATTRVALUE_BASETYPE_MASK) as *const T
}

#[inline(always)]
unsafe fn get_class_or_part_from_attr(attr: &structs::nsAttrValue) -> Class {
    debug_assert!(bindings::Gecko_AssertClassAttrValueIsSane(attr));
    let atom_count = bindings::Gecko_AttrValue_AtomCount(attr);
    if atom_count == 1 {
        return Class::One(ptr::<nsAtom>(attr));
    }
    if atom_count > 1 {
        let atoms = std::slice::from_raw_parts(
            bindings::Gecko_AttrValue_AtomAt(attr, 0) as *const structs::RefPtr<nsAtom>,
            atom_count as usize,
        );
        return Class::More(atoms);
    }
    Class::None
}

#[inline(always)]
unsafe fn get_id_from_attr(attr: &structs::nsAttrValue) -> &WeakAtom {
    debug_assert_eq!(bindings::Gecko_AttrValue_AtomCount(attr), 1);
    WeakAtom::new(ptr::<nsAtom>(attr))
}

/// Find an attribute value with a given name and no namespace.
#[inline(always)]
pub fn find_attr<'a>(
    attrs: &'a [structs::InternalAttr],
    name: &Atom,
) -> Option<&'a structs::nsAttrValue> {
    attrs
        .iter()
        .find(|attr| attr.mName.mBits == name.as_ptr() as usize)
        .map(|attr| &attr.mValue)
}

/// Finds the id attribute from a list of attributes.
#[inline(always)]
pub fn get_id(attrs: &[structs::InternalAttr]) -> Option<&WeakAtom> {
    Some(unsafe { get_id_from_attr(find_attr(attrs, &atom!("id"))?) })
}

#[inline(always)]
pub(super) fn each_exported_part(
    attrs: &[structs::InternalAttr],
    name: &AtomIdent,
    mut callback: impl FnMut(&AtomIdent),
) {
    let attr = match find_attr(attrs, &atom!("exportparts")) {
        Some(attr) => attr,
        None => return,
    };
    each_exported_part_from_attr(attr, name, callback);
}

#[inline(always)]
pub(super) fn each_exported_part_from_attr(
    attr: &structs::nsAttrValue,
    name: &AtomIdent,
    mut callback: impl FnMut(&AtomIdent),
) {
    let mut length = 0;
    let atoms = unsafe { bindings::Gecko_Element_ExportedParts(attr, name.as_ptr(), &mut length) };
    if atoms.is_null() {
        return;
    }

    unsafe {
        for atom in std::slice::from_raw_parts(atoms, length) {
            AtomIdent::with(*atom, &mut callback)
        }
    }
}

#[inline(always)]
pub(super) fn imported_part(
    attrs: &[structs::InternalAttr],
    name: &AtomIdent,
) -> Option<AtomIdent> {
    let attr = find_attr(attrs, &atom!("exportparts"))?;
    let atom = unsafe { bindings::Gecko_Element_ImportedPart(attr, name.as_ptr()) };
    if atom.is_null() {
        return None;
    }
    Some(AtomIdent(unsafe { Atom::from_raw(atom) }))
}

/// Given a class or part name, a case sensitivity, and an array of attributes,
/// returns whether the attribute has that name.
#[inline(always)]
pub fn has_class_or_part(
    name: &AtomIdent,
    case_sensitivity: CaseSensitivity,
    attr: &structs::nsAttrValue,
) -> bool {
    unsafe {
        bindings::Gecko_AttrValue_Contains(
            attr,
            name.as_ptr(),
            matches!(case_sensitivity, CaseSensitivity::AsciiCaseInsensitive),
        )
    }
}

/// Given an item, a callback, and a getter, execute `callback` for each class
/// or part name this `item` has.
#[inline(always)]
pub fn each_class_or_part<F>(attr: &structs::nsAttrValue, mut callback: F)
where
    F: FnMut(&AtomIdent),
{
    unsafe {
        let atom_count = bindings::Gecko_AttrValue_AtomCount(attr);
        for i in 0..atom_count {
            AtomIdent::with(bindings::Gecko_AttrValue_AtomAt(attr, i), &mut callback)
        }
    }
}
