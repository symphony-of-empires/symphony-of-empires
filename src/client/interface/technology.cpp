#include "client/interface/technology.hpp"
#include "client/map.hpp"
#include "unified_render/path.hpp"
#include "unified_render/texture.hpp"
#include "nation.hpp"
#include "world.hpp"
#include "client/game_state.hpp"

using namespace Interface;

TechTreeStrategicTab::TechTreeStrategicTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    unsigned int i = 0;
    for(const auto& technology : gs.world->technologies) {
        if(technology->type == TechnologyType::STRATEGIC) {
            auto* btn = new TechnologyInfo(gs, 0, 48 * i, technology, this);
            btn->text(technology->ref_name);
            i++;
        }
    }
}

TechTreeMilitaryTab::TechTreeMilitaryTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    unsigned int i = 0;
    for(const auto& technology : gs.world->technologies) {
        if(technology->type == TechnologyType::MILITARY) {
            auto* btn = new TechnologyInfo(gs, 0, 48 * i, technology, this);
            btn->text(technology->ref_name);
            i++;
        }
    }
}

TechTreeNavyTab::TechTreeNavyTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    unsigned int i = 0;
    for(const auto& technology : gs.world->technologies) {
        if(technology->type == TechnologyType::NAVY) {
            auto* btn = new TechnologyInfo(gs, 0, 48 * i, technology, this);
            btn->text(technology->ref_name);
            i++;
        }
    }
}

TechTreeSocialTab::TechTreeSocialTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    unsigned int i = 0;
    for(const auto& technology : gs.world->technologies) {
        if(technology->type == TechnologyType::SOCIAL) {
            auto* btn = new TechnologyInfo(gs, 0, 48 * i, technology, this);
            btn->text(technology->ref_name);
            i++;
        }
    }
}

TechTreeEconomicTab::TechTreeEconomicTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    unsigned int i = 0;
    for(const auto& technology : gs.world->technologies) {
        if(technology->type == TechnologyType::ECONOMIC) {
            auto* btn = new TechnologyInfo(gs, 0, 48 * i, technology, this);
            btn->text(technology->ref_name);
            i++;
        }
    }
}

TechTreePoliticsTab::TechTreePoliticsTab(GameState& _gs, int x, int y, UI::Widget* parent)
    : UI::Group(x, y, parent->width - x, parent->height - y, parent),
    gs{ _gs }
{
    unsigned int i = 0;
    for(const auto& technology : gs.world->technologies) {
        if(technology->type == TechnologyType::POLITICS) {
            auto* btn = new TechnologyInfo(gs, 0, 48 * i, technology, this);
            btn->text(technology->ref_name);
            i++;
        }
    }
}

TechTreeView::TechTreeView(GameState& _gs)
    : UI::Window(-400, 0, 400, _gs.height),
    gs{ _gs }
{
    if(gs.right_side_panel != nullptr) {
        gs.right_side_panel->kill();
    }
    gs.right_side_panel = this;

    this->is_scroll = false;
    this->text("Research");

    this->strategic_tab = new TechTreeStrategicTab(gs, 128 + 24, 0, this);
    this->strategic_tab->is_render = true;
    auto* strategic_btn = new UI::Button(0, 0, 128, 24, this);
    strategic_btn->text("Strategic");
    strategic_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<TechTreeView&>(*w.parent);

        o.strategic_tab->is_render = true;
        o.military_tab->is_render = false;
        o.navy_tab->is_render = false;
        o.social_tab->is_render = false;
        o.economic_tab->is_render = false;
        o.politics_tab->is_render = false;
    });

    this->military_tab = new TechTreeMilitaryTab(gs, 128 + 24, 0, this);
    this->military_tab->is_render = false;
    auto* military_btn = new UI::Button(0, 0, 128, 24, this);
    military_btn->below_of(*strategic_btn);
    military_btn->text("Military");
    military_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<TechTreeView&>(*w.parent);

        o.strategic_tab->is_render = false;
        o.military_tab->is_render = true;
        o.navy_tab->is_render = false;
        o.social_tab->is_render = false;
        o.economic_tab->is_render = false;
        o.politics_tab->is_render = false;
    });

    this->navy_tab = new TechTreeNavyTab(gs, 128 + 24, 0, this);
    this->navy_tab->is_render = false;
    auto* navy_btn = new UI::Button(0, 0, 128, 24, this);
    navy_btn->below_of(*military_btn);
    navy_btn->text("Navy");
    navy_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<TechTreeView&>(*w.parent);

        o.strategic_tab->is_render = false;
        o.military_tab->is_render = false;
        o.navy_tab->is_render = true;
        o.social_tab->is_render = false;
        o.economic_tab->is_render = false;
        o.politics_tab->is_render = false;
    });

    this->social_tab = new TechTreeSocialTab(gs, 128 + 24, 0, this);
    this->social_tab->is_render = false;
    auto* social_btn = new UI::Button(0, 0, 128, 24, this);
    social_btn->below_of(*navy_btn);
    social_btn->text("Social");
    social_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<TechTreeView&>(*w.parent);

        o.strategic_tab->is_render = false;
        o.military_tab->is_render = false;
        o.navy_tab->is_render = false;
        o.social_tab->is_render = true;
        o.economic_tab->is_render = false;
        o.politics_tab->is_render = false;
    });

    this->economic_tab = new TechTreeEconomicTab(gs, 128 + 24, 0, this);
    this->economic_tab->is_render = false;
    auto* economic_btn = new UI::Button(0, 0, 128, 24, this);
    economic_btn->below_of(*social_btn);
    economic_btn->text("Economic");
    economic_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<TechTreeView&>(*w.parent);

        o.strategic_tab->is_render = false;
        o.military_tab->is_render = false;
        o.navy_tab->is_render = false;
        o.social_tab->is_render = false;
        o.economic_tab->is_render = true;
        o.politics_tab->is_render = false;
    });

    this->politics_tab = new TechTreePoliticsTab(gs, 128 + 24, 0, this);
    this->politics_tab->is_render = false;
    auto* politics_btn = new UI::Button(0, 0, 128, 24, this);
    politics_btn->below_of(*economic_btn);
    politics_btn->text("Politics");
    politics_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<TechTreeView&>(*w.parent);

        o.strategic_tab->is_render = false;
        o.military_tab->is_render = false;
        o.navy_tab->is_render = false;
        o.social_tab->is_render = false;
        o.economic_tab->is_render = false;
        o.politics_tab->is_render = true;
    });

    auto* close_btn = new UI::CloseButton(0, 0, 128, 24, this);
    close_btn->below_of(*politics_btn);
    close_btn->text("Close");
    close_btn->on_click = ([](UI::Widget& w, void*) {
        auto& o = static_cast<TechTreeView&>(*w.parent);
        o.gs.right_side_panel->kill();
        o.gs.right_side_panel = nullptr;
    });
}