# CMake generated Testfile for 
# Source directory: F:/cycle/trae/chart/code/feature/tests
# Build directory: F:/cycle/trae/chart/code/feature/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(ogc_feature_tests "F:/cycle/trae/chart/code/feature/tests/Release/ogc_feature_tests.exe")
  set_tests_properties(ogc_feature_tests PROPERTIES  _BACKTRACE_TRIPLES "F:/cycle/trae/chart/code/feature/tests/CMakeLists.txt;29;add_test;F:/cycle/trae/chart/code/feature/tests/CMakeLists.txt;0;")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(ogc_feature_tests "F:/cycle/trae/chart/code/feature/tests/RelWithDebInfo/ogc_feature_tests.exe")
  set_tests_properties(ogc_feature_tests PROPERTIES  _BACKTRACE_TRIPLES "F:/cycle/trae/chart/code/feature/tests/CMakeLists.txt;29;add_test;F:/cycle/trae/chart/code/feature/tests/CMakeLists.txt;0;")
else()
  add_test(ogc_feature_tests NOT_AVAILABLE)
endif()
