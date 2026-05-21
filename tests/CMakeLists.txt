include(GoogleTest)

function(mst_add_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE mst GTest::gtest_main mst_warnings)
    gtest_discover_tests(${name})
endfunction()

mst_add_test(test_smoke)
mst_add_test(test_csr)
mst_add_test(test_union_find)
mst_add_test(test_union_find_atomic)
mst_add_test(test_boruvka_seq)
mst_add_test(test_kruskal)
mst_add_test(test_prim)
mst_add_test(test_boruvka_omp)
mst_add_test(test_determinism)

add_executable(test_property_vs_boost test_property_vs_boost.cpp)
target_link_libraries(test_property_vs_boost
    PRIVATE mst GTest::gtest_main Boost::graph mst_warnings)
gtest_discover_tests(test_property_vs_boost)
