set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
add_gtest(test_unbuffered_channel test.cpp)
add_benchmark(bench_unbuffered_channel bench.cpp)
