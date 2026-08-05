"""Public providers for the bdl rules."""

load("//private:providers.bzl", _BdlCompositionInfo = "BdlCompositionInfo", _BdlInfo = "BdlInfo", _BdlTargetInfo = "BdlTargetInfo")

BdlInfo = _BdlInfo
BdlCompositionInfo = _BdlCompositionInfo
BdlTargetInfo = _BdlTargetInfo
