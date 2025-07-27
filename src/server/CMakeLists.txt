project(mindshift-server)

add_library(${PROJECT_NAME} STATIC 
  http_server.cpp
  http_server.h
  request_handler.cpp
  request_handler.h
  http_session.h
  http_session.cpp
  request_handler.h
  request_handler.cpp
  router.h
  router.cpp
  auth.h
  auth.cpp
)

target_include_directories(mindshift-server PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${Boost_INCLUDE_DIRS}
)

target_link_libraries(mindshift-server PUBLIC
  database
  openai-api 
  curl
  # mindshift-utils
  ${Boost_LIBRARIES}
  Threads::Threads
)

target_compile_definitions(mindshift-server PUBLIC
  BOOST_BEAST_USE_STD_STRING_VIEW
)
