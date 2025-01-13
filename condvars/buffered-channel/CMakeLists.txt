set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
add_gtest(test_buffered_channel test.cpp)
add_benchmark(bench_buffered_channel bench.cpp)
