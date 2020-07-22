#!/usr/bin/python

"""
Represents a custom value
"""


class Custom(object):
    __bzd_custom__ = True

    def __init__(self, customType, value, *args):
        self.type = customType
        self.value = value
        self.args = args
        self.reprCallback = None
        self.manifest = None

    def __repr__(self):
        if self.reprCallback and self.manifest.renderer:
            callback = self.reprCallback(self.manifest.renderer)
            if callback:
                return callback(self.value, *self.args)
        return str(self.value)

    def setReprCallback(self, manifest, callback):
        assert self.reprCallback == None, "Repr callback has already been set for this value: {}.".format(
            str(self.value))
        self.reprCallback = callback
        self.manifest = manifest
