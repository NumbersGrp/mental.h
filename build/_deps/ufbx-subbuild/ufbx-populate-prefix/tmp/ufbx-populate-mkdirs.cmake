# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/twelvefacedjanus/Documents/mental.h/build/_deps/ufbx-src")
  file(MAKE_DIRECTORY "/Users/twelvefacedjanus/Documents/mental.h/build/_deps/ufbx-src")
endif()
file(MAKE_DIRECTORY
  "/Users/twelvefacedjanus/Documents/mental.h/build/_deps/ufbx-build"
  "/Users/twelvefacedjanus/Documents/mental.h/build/_deps/ufbx-subbuild/ufbx-populate-prefix"
  "/Users/twelvefacedjanus/Documents/mental.h/build/_deps/ufbx-subbuild/ufbx-populate-prefix/tmp"
  "/Users/twelvefacedjanus/Documents/mental.h/build/_deps/ufbx-subbuild/ufbx-populate-prefix/src/ufbx-populate-stamp"
  "/Users/twelvefacedjanus/Documents/mental.h/build/_deps/ufbx-subbuild/ufbx-populate-prefix/src"
  "/Users/twelvefacedjanus/Documents/mental.h/build/_deps/ufbx-subbuild/ufbx-populate-prefix/src/ufbx-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/twelvefacedjanus/Documents/mental.h/build/_deps/ufbx-subbuild/ufbx-populate-prefix/src/ufbx-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/twelvefacedjanus/Documents/mental.h/build/_deps/ufbx-subbuild/ufbx-populate-prefix/src/ufbx-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
