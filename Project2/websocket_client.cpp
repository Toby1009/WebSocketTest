#include "Project2/websocket_client.hpp"



connection_metadata::connection_metadata(websocketpp::connection_hdl hdl, std::string uri)
        : m_hdl(hdl)
        , m_status("Connecting")
        , m_uri(uri)
        , m_server("N/A")
    {}

void connection_metadata::on_open(client* c, websocketpp::connection_hdl hdl) {
    m_status = "Open";

    client::connection_ptr con = c->get_con_from_hdl(hdl);
    m_server = con->get_response_header("Server");
}

void connection_metadata::on_fail(client* c, websocketpp::connection_hdl hdl) {
    m_status = "Failed";

    client::connection_ptr con = c->get_con_from_hdl(hdl);
    m_server = con->get_response_header("Server");
    m_error_reason = con->get_ec().message();
}

void connection_metadata::on_close(client* c, websocketpp::connection_hdl hdl) {
    m_status = "Closed";
    client::connection_ptr con = c->get_con_from_hdl(hdl);
    std::stringstream s;
    s << "close code: " << con->get_remote_close_code() << " ("
        << websocketpp::close::status::get_string(con->get_remote_close_code())
        << "), close reason: " << con->get_remote_close_reason();
    m_error_reason = s.str();
}

websocketpp::connection_hdl connection_metadata::get_hdl() const {
    return m_hdl;
}

int connection_metadata::get_id() const {
    return m_id;
}

std::string connection_metadata::get_status() const {
    return m_status;
}

std::ostream& operator<< (std::ostream& out, connection_metadata const& data) {
    out << "> URI: " << data.m_uri << "\n"
        << "> Status: " << data.m_status << "\n"
        << "> Remote Server: " << (data.m_server.empty() ? "None Specified" : data.m_server) << "\n"
        << "> Error/close reason: " << (data.m_error_reason.empty() ? "N/A" : data.m_error_reason);

    return out;
}


void on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg) {
    std::cout << "Received message from server: " << msg->get_payload() << std::endl;
}

void on_open(client* c, websocketpp::connection_hdl hdl) {
    std::cout << "Connection opened" << std::endl;
}


WebSocketClient::WebSocketClient(string uri)
{
    this->uri = uri;
    c.clear_access_channels(websocketpp::log::alevel::all);
    c.clear_error_channels(websocketpp::log::elevel::all);
    c.init_asio();
    c.start_perpetual();
    m_Thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&client::run, &c);
}

WebSocketClient::~WebSocketClient() {
    c.stop_perpetual();
	m_Thread->join();
}

void WebSocketClient::sendAction() {
	json j;
    for (const auto& action : actionMap) {
        j[action.first] = action.second;
    }
    websocketpp::lib::error_code ec;
    if (m_ConnectionMetadataPtr == nullptr) {
        cout << "not connection";
		return;
	}

    c.send(m_ConnectionMetadataPtr->get_hdl(), j.dump(), websocketpp::frame::opcode::text, ec);
    if (ec) {
		std::cout << "Send failed: " << ec.message() << std::endl;
		return;
	}
}

void WebSocketClient::setJump(bool flag) {
    actionMap["JUMP"] = flag;
    sendAction();
}
void WebSocketClient::setRightHand(bool flag) {
	actionMap["RIGHTHAND"] = flag;
    sendAction();
}
void WebSocketClient::setLeftHand(bool flag) {
    actionMap["LEFTHAND"] = flag;
    sendAction();
}
void WebSocketClient::setSquat(bool flag) {
	actionMap["SQUAT"] = flag;
    sendAction();
}
void WebSocketClient::setForward(bool flag) {
    actionMap["FORWARD"] = flag;
    sendAction();
}
void WebSocketClient::setBackward(bool flag) {
    actionMap["BACKWARD"] = flag;
    sendAction();
}

void WebSocketClient::run() {
    try {
        websocketpp::lib::error_code ec;
        con = c.get_connection(uri, ec);
        if (ec) {
            std::cout << "Could not create connection: " << ec.message() << std::endl;
            return ;
        }
        hdl = con->get_handle();
        connection_metadata::ptr metadata_ptr = websocketpp::lib::make_shared<connection_metadata>(con->get_handle(), uri);
        m_ConnectionMetadataPtr = metadata_ptr;

        con->set_open_handler(websocketpp::lib::bind(
            &connection_metadata::on_open,
            metadata_ptr,
            &c,
            websocketpp::lib::placeholders::_1
        ));
        con->set_fail_handler(websocketpp::lib::bind(
            &connection_metadata::on_fail,
            metadata_ptr,
            &c,
            websocketpp::lib::placeholders::_1
        ));
        con->set_close_handler(websocketpp::lib::bind(
            &connection_metadata::on_close,
            metadata_ptr,
            &c,
            websocketpp::lib::placeholders::_1
        ));
        c.connect(con);
        cout<<"connect success"<<endl;
        //c.run();
    }
    catch (websocketpp::exception const& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }

}

void WebSocketClient::start() {
   m_Thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&WebSocketClient::run, this);
}



