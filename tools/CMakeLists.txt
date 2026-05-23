add_executable(gen_graph gen_graph.cpp)
target_link_libraries(gen_graph PRIVATE mst mst_warnings)

add_executable(gen_grid gen_grid.cpp)
target_link_libraries(gen_grid PRIVATE mst mst_warnings)

add_executable(bench bench.cpp)
target_link_libraries(bench PRIVATE mst mst_warnings)

add_executable(dump_mst dump_mst.cpp)
target_link_libraries(dump_mst PRIVATE mst mst_warnings)

add_executable(profile_omp profile_omp.cpp)
target_link_libraries(profile_omp PRIVATE mst mst_warnings)

add_executable(gen_knn gen_knn.cpp)
target_link_libraries(gen_knn PRIVATE mst mst_warnings)
