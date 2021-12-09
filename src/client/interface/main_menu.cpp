#include "client/interface/main_menu.hpp"
#include "client/client_network.hpp"
#include "world.hpp"
#include "print.hpp"
#include "path.hpp"
#include "client/map.hpp"
#include "client/render/texture.hpp"

using namespace Interface;

MainMenu::MainMenu(GameState& _gs)
    : gs{ _gs },
    UI::Window(-(512 / 2), -(200 / 2), 512, 200)
{
    this->origin = CENTER_SCREEN;
    this->is_pinned = true;
    this->is_scroll = false;
    this->text("Symphony of Empires");
    
    //auto* title_img = new UI::Image(0, 0, 256, 128, &g_texture_manager->load_texture(Path::get("ui/title.png")), this);

    auto* single_btn = new UI::Button(0, 24, 128, 24, this);
    single_btn->text("Singleplayer");
    single_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenu&>(*w.parent);
        GameState& gs = o.gs;
        gs.world = new World();
        gs.client = new Client(gs, "127.0.0.1", 1836);
        gs.client->username = "SpPlayer";
        gs.client->wait_for_snapshot();
        gs.map = new Map(*gs.world, gs.width, gs.height);
        gs.in_game = true;
    });

    auto* mp_btn = new UI::Button(0, 24, 128, 24, this);
    mp_btn->text("Multiplayer");
    mp_btn->right_side_of(*single_btn);
    mp_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<MainMenu&>(*w.parent);
        if(o.connect_window != nullptr)
            delete o.connect_window;
        o.connect_window = new MainMenuConnectServer(o.gs);
    });

    auto* cfg_btn = new UI::Button(0, 24, 128, 24, this);
    cfg_btn->text("Settings");
    cfg_btn->right_side_of(*mp_btn);

    auto* exit_btn = new UI::Button(0, 24, 128, 24, this);
    exit_btn->text("Exit");
    exit_btn->right_side_of(*cfg_btn);
}

MainMenu::~MainMenu() {
    if (connect_window != nullptr)
        delete connect_window;
}

MainMenuConnectServer::MainMenuConnectServer(GameState& _gs)
    : gs{ _gs },
    in_game{ false },
    UI::Window(-512 / 2, -128 / 2, 512, 128, nullptr)
{
    this->origin = CENTER_SCREEN;
    this->is_pinned = true;
    this->is_scroll = false;
    this->is_float = true;
    this->text("Internet multiplayer");

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
        gs.map = new Map(*gs.world, gs.width, gs.height);
        gs.in_game = true;
    });
}