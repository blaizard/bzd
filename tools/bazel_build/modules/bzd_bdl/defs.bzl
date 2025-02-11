"""Public Starlark API for the bdl rules."""

load("//private:application_factory.bzl", _bdl_application_factory = "bdl_application_factory")
load("//private:library.bzl", _bdl_library = "bdl_library")
load("//private:providers.bzl", _BdlSystemJsonInfo = "BdlSystemJsonInfo")
load("//private:system.bzl", _bdl_system = "bdl_system", _bdl_system_test = "bdl_system_test")
load("//private:target.bzl", _bdl_target = "bdl_target")

BdlSystemJsonInfo = _BdlSystemJsonInfo
bdl_library = _bdl_library
bdl_system = _bdl_system
bdl_system_test = _bdl_system_test
bdl_target = _bdl_target
bdl_application_factory = _bdl_application_factory
