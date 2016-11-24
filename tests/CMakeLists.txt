find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(aisdiMapsTests test_main.cpp TreeMapTests.cpp HashMapTests.cpp)
add_executable(aisdiHashMapTests test_main.cpp HashMapTests.cpp)
add_executable(aisdiTreeMapTests test_main.cpp TreeMapTests.cpp)

target_link_libraries(aisdiMapsTests ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(aisdiHashMapTests ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
target_link_libraries(aisdiTreeMapTests ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(boostUnitTestsRun aisdiMapsTests)
add_test(boostUnitTestsRun aisdiHashMapTests)
add_test(boostUnitTestsRun aisdiTreeMapTests)

if (CMAKE_CONFIGURATION_TYPES)
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
      --force-new-ctest-process --output-on-failure
      --build-config "$<CONFIGURATION>"
      DEPENDS aisdiMapsTests)
else()
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
      --force-new-ctest-process --output-on-failure
      DEPENDS aisdiMapsTests)
endif()
