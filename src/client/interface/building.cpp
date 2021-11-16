#include "client/interface/building.hpp"

using namespace Interface;

BuildingSelectCompanyTab::BuildingSelectCompanyTab(GameState& _gs, int x, int y, UI::Widget* _parent)
    : gs{_gs},
    UI::Group(x, y, _parent->width - x, _parent->height - y, _parent)
{
    uint i = 0;
    for(const auto& company : gs.world->companies) {
        auto* btn = new CompanyButton(gs, 0, 24 * i, company, this);
        btn->on_click = ([](UI::Widget& w, void*) {
            auto& o = static_cast<BuildingBuildView&>(*w.parent);
            o.company = static_cast<CompanyButton&>(w).company;
        });
        i++;
    }
}

BuildingSelectProvinceTab::BuildingSelectProvinceTab(GameState& _gs, int x, int y, UI::Widget* _parent)
    : gs{_gs},
    UI::Group(x, y, _parent->width - x, _parent->height - y, _parent)
{
    uint i = 0;
    for(const auto& province : gs.world->provinces) {
        auto* btn = new ProvinceButton(gs, 0, 24 * i, province, this);
        btn->on_click = ([](UI::Widget& w, void*) {
            auto& o = static_cast<BuildingBuildView&>(*w.parent);
            o.province = static_cast<ProvinceButton&>(w).province;
        });
        i++;
    }
}

BuildingSelectNationTab::BuildingSelectNationTab(GameState& _gs, int x, int y, UI::Widget* _parent)
    : gs{_gs},
    UI::Group(x, y, _parent->width - x, _parent->height - y, _parent)
{
    uint i = 0;
    for(const auto& nation : gs.world->nations) {
        auto* btn = new NationButton(gs, 0, 24 * i, nation, this);
        btn->on_click = ([](UI::Widget& w, void*) {
            auto& o = static_cast<BuildingBuildView&>(*w.parent);
            o.nation = static_cast<NationButton&>(w).nation;
        });
        i++;
    }
}

#include "client/client_network.hpp"
#include "serializer.hpp"
#include "actions.hpp"
#include "io_impl.hpp"
BuildingBuildView::BuildingBuildView(GameState& _gs, int _tx, int _ty, bool _in_tile, Nation* _nation, Province* _province, Company* _company)
    : gs{_gs},
    tx(_tx),
    ty(_ty),
    in_tile(_in_tile),
    nation(_nation),
    province(_province),
    company(_company),
    UI::Window(0, 0, 512, 512)
{
    this->is_scroll = false;
    this->text("Build a new building");

    this->company_tab = new BuildingSelectCompanyTab(gs, 128, 24, this);
    this->company_tab->is_render = false;

    this->province_tab = new BuildingSelectProvinceTab(gs, 128, 24, this);
    this->province_tab->is_render = false;

    this->nation_tab = new BuildingSelectNationTab(gs, 128, 24, this);
    this->nation_tab->is_render = false;

    auto* company_btn = new UI::Button(0, 24, 128, 24, this);
    company_btn->text("Company");
    company_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<BuildingBuildView&>(*w.parent);
        o.company_tab->is_render = true;
        o.province_tab->is_render = false;
        o.nation_tab->is_render = false;
    });

    auto* province_btn = new UI::Button(0, 0, 128, 24, this);
    province_btn->below_of(*company_btn);
    province_btn->text("Province");
    province_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<BuildingBuildView&>(*w.parent);
        o.company_tab->is_render = false;
        o.province_tab->is_render = true;
        o.nation_tab->is_render = false;
    });

    auto* nation_btn = new UI::Button(0, 0, 128, 24, this);
    nation_btn->below_of(*province_btn);
    nation_btn->text("Nation");
    nation_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<BuildingBuildView&>(*w.parent);
        o.company_tab->is_render = false;
        o.province_tab->is_render = false;
        o.nation_tab->is_render = true;
    });

    auto* build_btn = new UI::Button(0, 0, 128, 24, this);
    build_btn->below_of(*nation_btn);
    build_btn->text("BUILD_STUFF");
    build_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<BuildingBuildView&>(*w.parent);
        
        // Tell the server about this "new" building
        g_client->packet_mutex.lock();
        Packet packet = Packet();
        Archive ar = Archive();
        ActionType action = ActionType::BUILDING_ADD;
        ::serialize(ar, &action);

        auto building = Building();
        building.owner = o.nation != nullptr ? o.nation : o.gs.curr_nation;
        building.corporate_owner = o.company != nullptr ? o.company : o.gs.world->companies[rand() % o.gs.world->companies.size()];
        building.type = o.gs.world->building_types[0];
        building.x = o.tx;
        building.y = o.ty;
        ::serialize(ar, &building); // BuildingObj

        packet.data(ar.get_buffer(), ar.size());
        g_client->packet_queue.push_back(packet);
        g_client->packet_mutex.unlock(); 
    });

    auto* close_btn = new UI::CloseButton(0, 0, 128, 24, this);
    close_btn->below_of(*build_btn);
    close_btn->text("Close");
}