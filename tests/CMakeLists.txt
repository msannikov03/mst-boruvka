include(GoogleTest)

function(mst_add_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE mst GTest::gtest_main mst_warnings)
    gtest_discover_tests(${name})
endfunction()

mst_add_test(test_smoke)
