#include <gflags/gflags.h>
//声明参数
DEFINE_string(ip, "127.0.0.1", "这是服务器的监听IP地址，格式：127.0.0.1");
DEFINE_int32(port, 8080, "这是服务器的监听端口, 格式: 8080");
DEFINE_bool(debug_enable, true, "是否启用调试模式, 格式：true/false");