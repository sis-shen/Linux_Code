# 编译器设置
CXX := g++
CXXFLAGS := -Wall -Wextra -pedantic -std=c++17

# 关键变量定义
SRCS := $(wildcard *.cpp)          # 获取所有.cpp文件
OBJS := $(SRCS:.cpp=.o)            # 替换后缀为.o
TARGET := myos               # 最终可执行文件名（按需修改）

# 默认目标：编译并链接
all: $(TARGET)

# 链接目标文件 → 可执行程序
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# 隐式规则: 自动处理.cpp → .o的编译
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理生成文件
clean:
	rm -f $(OBJS) $(TARGET)

# 声明伪目标（避免与同名文件冲突）
.PHONY: all clean
