cmake_minimum_required(VERSION 3.8)

aux_source_directory(./src tool_src)

add_executable(tcash_binary_data_decoder ${tool_src})

add_dependencies(tcash_binary_data_decoder xdata xcommon xbasic)
target_link_libraries(tcash_binary_data_decoder xdata xcommon xbasic xbase)
