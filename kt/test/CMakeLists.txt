add_definitions(-DMODULE=\"test_keytree\")
add_definitions(-DBACKEND_PATH=\"${CMAKE_CURRENT_BINARY_DIR}/../src/keytree\")
add_definitions(-DRES_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/res\")

add_test(keytree 01)


