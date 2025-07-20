project(database)

add_library(${PROJECT_NAME} STATIC
  database_client.h
  database_client.cpp
  connection_pool.cpp
)
