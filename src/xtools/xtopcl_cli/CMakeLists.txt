cmake_minimum_required(VERSION 3.8)

aux_source_directory(./src xtcashcl_cli_src)

# SET TARGET
add_executable(xtcashcl_cli ${xtcashcl_cli_src})
#add_dependencies(xtcashcl_cli xxbase xdata xcrypto xtcashcl)
target_link_libraries(xtcashcl_cli PRIVATE xtcashcl)
