cmake_minimum_required(VERSION 3.14)
project(drogon_simple)

# 强制要求C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Drogon依赖
find_package(Drogon REQUIRED COMPOMENTS jsoncpp)

# 添加可执行文件
add_executable(${PROJECT_NAME} main.cpp)

# 连接库文件
target_link_libraries(
    ${PROJECT_NAME}
    Drogon::Drogon 
    ${JSONCPP_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    pthread
)