project(mindshift-backend-project)

add_executable(${PROJECT_NAME}
  main.cpp
)

target_link_libraries(mindshift-backend-project PUBLIC
  # mindshift-database
  # mindshift-external
  # mindshift-utils
  mindshift-server
  Threads::Threads
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_subdirectory(database)
add_subdirectory(external_apis)
add_subdirectory(server)
add_subdirectory(utils)
