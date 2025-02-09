"""Public Starlark API for the bdl rules."""

load("//private:bdl.bzl", _BdlSystemJsonInfo = "BdlSystemJsonInfo", _bdl_application_factory = "bdl_application_factory", _bdl_library = "bdl_library", _bdl_system = "bdl_system", _bdl_system_test = "bdl_system_test", _bdl_target = "bdl_target", _bdl_target_platform = "bdl_target_platform")

BdlSystemJsonInfo = _BdlSystemJsonInfo
bdl_library = _bdl_library
bdl_system = _bdl_system
bdl_system_test = _bdl_system_test
bdl_target = _bdl_target
bdl_target_platform = _bdl_target_platform
bdl_application_factory = _bdl_application_factory
