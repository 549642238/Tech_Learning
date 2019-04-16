ADD_EXECUTABLE(main cJSON.cpp  CMakeLists.txt  main.cpp  server.cpp)
TARGET_LINK_LIBRARIES(main libmysqlclient.a m z dl pthread)
