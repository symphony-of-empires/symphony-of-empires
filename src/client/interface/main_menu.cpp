#include "client/interface/main_menu.hpp"
#include "client/client_network.hpp"
#include "print.hpp"

MainMenuConnectServer::MainMenuConnectServer(GameState& _gs)
    : gs{ _gs },
    in_game{ false },
    UI::Window(0, 0, 512, 128)
{
    this->is_scroll = false;
    this->text("Connect to a server");

    ip_addr_inp = new UI::Input(0, 24, 512, 24, this);
    ip_addr_inp->buffer = "127.0.0.1";
    ip_addr_inp->text(ip_addr_inp->buffer);

    username_inp = new UI::Input(0, 48, 512, 24, this);
    username_inp->buffer = "Default";
    username_inp->text(username_inp->buffer);

    conn_btn = new UI::Button(0, 72, 128, 24, this);
    conn_btn->user_data = this;
    conn_btn->text("Connect");
    conn_btn->on_click = ([](UI::Widget& w, void* data) {
        MainMenuConnectServer* state = (MainMenuConnectServer*)data;

        std::string server_addr = state->ip_addr_inp->buffer;
        print_info("Okey, connecting to [%s]", server_addr.c_str());

        GameState& gs = state->gs;
        gs.world = new World();
        gs.client = new Client(gs, server_addr, 1836);
        gs.client->username = state->username_inp->buffer;
        gs.client->wait_for_snapshot();
        gs.map = new Map(*gs.world);
        state->in_game = true;
    });
}