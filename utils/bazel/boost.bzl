HDRS_PATH = "boost"

HDRS_PATTERNS = [
    HDRS_PATH + "/%s.h",
    HDRS_PATH + "/%s_fwd.h",
    HDRS_PATH + "/%s.hpp",
    HDRS_PATH + "/%s_fwd.hpp",
    HDRS_PATH + "/%s/*.h",
    HDRS_PATH + "/%s/*.hpp",
    HDRS_PATH + "/%s/*.ipp",
    HDRS_PATH + "/%s/**/*.h",
    HDRS_PATH + "/%s/**/*.hpp",
    HDRS_PATH + "/%s/**/*.ipp",
]

SRCS_PATH = "libs"

SRCS_PATTERNS = [
    SRCS_PATH + "/%s/src/*.cpp",
    SRCS_PATH + "/%s/src/*.hpp",
    SRCS_PATH + "/%s/src/*.ipp",
]

# Building boost results in many warnings for unused values. Downstream users
# won't be interested, so just disable the warning.
default_copts = ["-Wno-unused-value"]

def srcs_list(library_name):
  return native.glob([p % (library_name,) for p in SRCS_PATTERNS])

def hdrs_list(library_name):
  return native.glob([p % (library_name,) for p in HDRS_PATTERNS])

def includes_list(library_name):
  return ["."]

def boost_library(name,
                  includes=None,
                  hdrs=None,
                  srcs=None,
                  exclude_srcs=None,
                  copts=None,
                  **kwargs):
  if includes == None:
    includes = []

  if hdrs == None:
    hdrs = []

  if srcs == None:
    srcs = []

  if exclude_srcs == None:
    exclude_srcs = []

  if copts == None:
    copts = []

  return native.cc_library(
    name = name,
    includes = includes_list(name) + includes,
    hdrs = hdrs_list(name) + hdrs,
    srcs = [src for src in srcs_list(name) if src not in exclude_srcs] + srcs,
    copts = default_copts + copts,
    **kwargs
  )
