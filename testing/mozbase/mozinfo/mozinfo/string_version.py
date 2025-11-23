# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this file,
# You can obtain one at http://mozilla.org/MPL/2.0/.

from __future__ import absolute_import

import re

class LooseVersion(object):
    component_re = re.compile(r'(\d+ | [a-z]+ | \.)', re.VERBOSE)

    def __init__(self, vstring):
        self.vstring = vstring
        self.parse(vstring)

    def parse(self, vstring):
        # I've given up on the "verbose" part of this regex, but it's
        # still a good idea to keep it around for reference.
        # component_re = re.compile(r'(\d+ | [a-z]+ | \.)', re.VERBOSE)
        if vstring is None:
            raise ValueError("version string cannot be None")
        if isinstance(vstring, (list, tuple)):
            vstring = ".".join(map(str, vstring))
        
        self.vstring = vstring
        components = [x for x in self.component_re.split(vstring)
                              if x and x != '.']
        for i, obj in enumerate(components):
            try:
                components[i] = int(obj)
            except ValueError:
                pass

        self.version = components

    def __str__(self):
        return self.vstring

    def __repr__(self):
        return "LooseVersion ('%s')" % str(self)

    def __cmp__(self, other):
        if isinstance(other, str):
            other = LooseVersion(other)
        elif not isinstance(other, LooseVersion):
            return NotImplemented

        if self.version == other.version:
            return 0
        if self.version < other.version:
            return -1
        if self.version > other.version:
            return 1

    def __lt__(self, other):
        c = self.__cmp__(other)
        if c is NotImplemented:
            return NotImplemented
        return c < 0

    def __le__(self, other):
        c = self.__cmp__(other)
        if c is NotImplemented:
            return NotImplemented
        return c <= 0

    def __eq__(self, other):
        c = self.__cmp__(other)
        if c is NotImplemented:
            return NotImplemented
        return c == 0

    def __ge__(self, other):
        c = self.__cmp__(other)
        if c is NotImplemented:
            return NotImplemented
        return c >= 0

    def __gt__(self, other):
        c = self.__cmp__(other)
        if c is NotImplemented:
            return NotImplemented
        return c > 0

    def __ne__(self, other):
        c = self.__cmp__(other)
        if c is NotImplemented:
            return NotImplemented
        return c != 0


class StringVersion(str):
    """
    A string version that can be compared with comparison operators.
    """

    def __init__(self, vstring):
        super(StringVersion, self).__init__()
        self.version = LooseVersion(vstring)

    def __repr__(self):
        return "StringVersion ('%s')" % self

    def __to_version(self, other):
        if not isinstance(other, StringVersion):
            other = StringVersion(other)
        return other.version

    # rich comparison methods

    def __lt__(self, other):
        return self.version < self.__to_version(other)

    def __le__(self, other):
        return self.version <= self.__to_version(other)

    def __eq__(self, other):
        return self.version == self.__to_version(other)

    def __ne__(self, other):
        return self.version != self.__to_version(other)

    def __gt__(self, other):
        return self.version > self.__to_version(other)

    def __ge__(self, other):
        return self.version >= self.__to_version(other)
