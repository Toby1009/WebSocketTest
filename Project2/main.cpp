#include <iostream>
#include <cstdlib>  // 包含 system 函数
#include "Project2/websocket_client.hpp"
using namespace std;

int main() {
    const char* pyServer = "websocket_server.py";
    // 使用 system 函数运行Python脚本
    int result = system((std::string("start /B py ") + pyServer).c_str());

    
    WebSocketClient client("ws://localhost:8765");
    client.run();
    std::this_thread::sleep_for(std::chrono::seconds(1));//that it is connected
    client.setJump(true);//or else
    
    return 0;
}
