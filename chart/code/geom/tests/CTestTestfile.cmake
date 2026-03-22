# CMake generated Testfile for 
# Source directory: F:/cycle/trae/chart/code/geom/tests
# Build directory: F:/cycle/trae/chart/code/geom/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(ogc_geometry_tests "F:/cycle/trae/chart/code/geom/tests/Release/ogc_geometry_tests.exe")
  set_tests_properties(ogc_geometry_tests PROPERTIES  _BACKTRACE_TRIPLES "F:/cycle/trae/chart/code/geom/tests/CMakeLists.txt;26;add_test;F:/cycle/trae/chart/code/geom/tests/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(ogc_geometry_tests "F:/cycle/trae/chart/code/geom/tests/RelWithDebInfo/ogc_geometry_tests.exe")
  set_tests_properties(ogc_geometry_tests PROPERTIES  _BACKTRACE_TRIPLES "F:/cycle/trae/chart/code/geom/tests/CMakeLists.txt;26;add_test;F:/cycle/trae/chart/code/geom/tests/CMakeLists.txt;0;")
else()
  add_test(ogc_geometry_tests NOT_AVAILABLE)
endif()
