project(openai-api)

add_library(${PROJECT_NAME} STATIC
  openai_client.h
  openai_client.cpp
)
