cmd:main.cpp
	g++ -o $@ $^ -std=c++17 -lpthread -lfmt

.PHONY:clean
clean:
	rm -f cmd

# # 基本编译配置
# CXX := g++
# TARGET := spdlog_example
# SRC := main.cpp
# CXXFLAGS := -std=c++11 -Wall -Wextra -I/path/to/spdlog/include  # 替换实际spdlog路径
# LDFLAGS := -lpthread  # 异步日志需要pthread支持

# # 默认构建规则
# $(TARGET): $(SRC)
# 	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# # 清理规则
# clean:
# 	rm -f $(TARGET) logs/*.txt  # 清理生成文件和日志

# .PHONY: clean