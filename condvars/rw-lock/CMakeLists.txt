set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
add_gtest(test_rwlock test.cpp)
add_benchmark(bench_rwlock run.cpp)
