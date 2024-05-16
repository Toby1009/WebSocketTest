#pragma once
#include <iostream>
#include <string>
#include <map>
#include <thread> 
#include <locale>
#include <codecvt>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include "Project2/json.hpp"



using namespace std;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;


using json = nlohmann::json; // 使用 nlohmann/json 命名空间

typedef websocketpp::client<websocketpp::config::asio_client> client;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

typedef websocketpp::client<websocketpp::config::asio_client> client;

class connection_metadata {
public:
    typedef websocketpp::lib::shared_ptr<connection_metadata> ptr;

    connection_metadata(websocketpp::connection_hdl hdl, std::string uri);

    void on_open(client* c, websocketpp::connection_hdl hdl);

    void on_fail(client* c, websocketpp::connection_hdl hdl);

    void on_close(client* c, websocketpp::connection_hdl hdl);

    websocketpp::connection_hdl get_hdl() const;

    int get_id() const;

    std::string get_status() const;

    friend std::ostream& operator<< (std::ostream& out, connection_metadata const& data);
private:
    int m_id;
    websocketpp::connection_hdl m_hdl;
    std::string m_status;
    std::string m_uri;
    std::string m_server;
    std::string m_error_reason;
};

std::ostream& operator<< (std::ostream& out, connection_metadata const& data);


void on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg);

void on_open(client* c, websocketpp::connection_hdl hdl);

enum ACTION {
    JUMP,
    RIGHTHAND,
    LEFTHAND,
    SQUAT,
    FORWARD,
    BACKWARD,
    NUM_ACTION,
};

class WebSocketClient {
public:
    client c;
    std::string uri = "ws://localhost:8765"; // 使用 ws:// 表示普通连接，不加密
    websocketpp::connection_hdl hdl; // 连接句柄
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_Thread; 
    connection_metadata::ptr m_ConnectionMetadataPtr;
    client::connection_ptr con;

    map<string, bool> actionMap = {
        {"JUMP", false},
        {"RIGHTHAND", false},
        {"LEFTHAND", false},
        {"SQUAT", false},
        {"FORWARD", false},
        {"BACKWARD", false},
    };
    string actionName[NUM_ACTION] = { "JUMP", "RIGHTHAND", "LEFTHAND", "SQUAT", "FORWARD", "BACKWARD" };
    WebSocketClient(string uri);
    ~WebSocketClient();
    void sendAction();

    void setJump(bool flag);
    void setRightHand(bool flag);
    void setLeftHand(bool flag);
    void setSquat(bool flag);
    void setForward(bool flag);
    void setBackward(bool flag);

    void run();
    void start(); // 新增这个方法来启动WebSocket客户端
};

