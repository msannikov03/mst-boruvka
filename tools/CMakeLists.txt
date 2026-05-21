add_executable(gen_graph gen_graph.cpp)
target_link_libraries(gen_graph PRIVATE mst mst_warnings)

add_executable(bench bench.cpp)
target_link_libraries(bench PRIVATE mst mst_warnings)
