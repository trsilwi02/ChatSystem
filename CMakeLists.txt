cmake_minimum_required(VERSION 3.16)
project(ChatSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)  # Disable AUTOUIC - using programmatic UI instead

find_package(Qt6 COMPONENTS 
    Core 
    Gui 
    Widgets 
    Network 
    Concurrent
    REQUIRED)

# Common library
add_library(ChatCommon STATIC
    common/protocol.cpp
    common/message.cpp
)

target_include_directories(ChatCommon PUBLIC common)

# Server executable
add_executable(ChatServer
    server/main.cpp
    server/server.cpp
    server/client_handler.cpp
    server/user_manager.cpp
)

target_link_libraries(ChatServer 
    PRIVATE 
    ChatCommon 
    ws2_32
)

target_include_directories(ChatServer PRIVATE server common)

# Client GUI executable
add_executable(ChatClient
    client/main.cpp
    client/chat_window.cpp
    client/socket_client.cpp
    client/ui_components.cpp
)

target_link_libraries(ChatClient 
    PRIVATE 
    ChatCommon
    Qt6::Core 
    Qt6::Gui 
    Qt6::Widgets 
    Qt6::Network
    Qt6::Concurrent
    ws2_32
)

target_include_directories(ChatClient PRIVATE client common)
