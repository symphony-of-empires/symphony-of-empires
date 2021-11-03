#include "client/interface/nation_view.hpp"
#include "io_impl.hpp"

using namespace Interface;

NationView::NationView(GameState& _gs, Nation* _nation)
    : gs{ _gs },
    nation{ _nation },
    UI::Window(0, 0, 256, 512)
{
    this->is_scroll = false;

    this->flag_img = new UI::Image(0, 24, 128, 96, nullptr, this);
    this->flag_img->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<NationView&>(*w.parent);
        w.current_texture = &o.gs.get_nation_flag(*o.nation);
    });

    this->name_lab = new UI::Label(0, 0, "?", this);
    this->name_lab->below_of(*this->flag_img);
    this->name_lab->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<NationView&>(*w.parent);
        w.text(o.nation->get_client_hint().alt_name);
    });

    this->ideology_lab = new UI::Label(0, 0, "?", this);
    this->ideology_lab->below_of(*this->name_lab);
    this->ideology_lab->on_each_tick = ([](UI::Widget& w, void*) {
        auto& o = static_cast<NationView&>(*w.parent);
        w.text(o.nation->get_client_hint().ideology->name);
    });

    this->close_btn = new UI::CloseButton(0, 0 * 3, this->width, 24, this);
    this->close_btn->text("Close");
    this->close_btn->below_of(*this->ideology_lab);
}