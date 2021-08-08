#include "render/texture.hpp"
#include "ui.hpp"
#include "path.hpp"

extern TextureManager* g_texture_manager;
const Texture* checkbox_ppv_on,* checkbox_ppv_off;

extern std::pair<int, int> mouse_pos;

#include "nation.hpp"
extern Nation* curr_nation;

#include "serializer.hpp"
#include "io_impl.hpp"
#include "network.hpp"

static Policies draft_policy;
extern char* tmpbuf;

void ui_reform_policies(UI::Widget&, void*) {
    draft_policy = curr_nation->current_policy;
    checkbox_ppv_on = &g_texture_manager->load_texture(Path::get("ui/checkbox_ppv_on.png"));
    checkbox_ppv_off = &g_texture_manager->load_texture(Path::get("ui/checkbox_ppv_off.png"));
    
    UI::Window* reform_win = new UI::Window(mouse_pos.first, mouse_pos.second, 800, 400);
    reform_win->text("Policies");

    int x_off = 0, y_off = 0;
    y_off += 24;

    // Population - Migration
    {
        UI::Label* section_lab = new UI::Label(x_off, y_off, "Migration", reform_win);
        UI::Checkbox* nobody_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        nobody_btn->text("Nobody");
        nobody_btn->below_of(*section_lab);
        nobody_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.migration = ALLOW_NOBODY;
        };
        nobody_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.migration == ALLOW_NOBODY)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
                
        UI::Checkbox* accepted_only_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        accepted_only_btn->text("Accepted culture");
        accepted_only_btn->below_of(*nobody_btn);
        accepted_only_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.migration = ALLOW_ACCEPTED_CULTURES;
        };
        accepted_only_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.migration == ALLOW_ACCEPTED_CULTURES)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
                
        UI::Checkbox* all_payment_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        all_payment_btn->text("All with payment");
        all_payment_btn->below_of(*accepted_only_btn);
        all_payment_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.migration = ALLOW_ALL_PAYMENT;
        };
        all_payment_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.migration == ALLOW_ALL_PAYMENT)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
                
        UI::Checkbox* allow_all_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        allow_all_btn->text("All");
        allow_all_btn->below_of((*all_payment_btn));
        allow_all_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.migration = ALLOW_ALL;
        };
        allow_all_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.migration == ALLOW_ALL)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
    }

    // Population - Immigration
    x_off += 200;
    {
        UI::Label* section_lab = new UI::Label(x_off, y_off, "Immigration", reform_win);
        UI::Checkbox* nobody_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        nobody_btn->text("Nobody");
        nobody_btn->below_of(*section_lab);
        nobody_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.immigration = ALLOW_NOBODY;
        };
        nobody_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.immigration == ALLOW_NOBODY)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
                
        UI::Checkbox* accepted_only_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        accepted_only_btn->text("Accepted culture");
        accepted_only_btn->below_of(*nobody_btn);
        accepted_only_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.immigration = ALLOW_ACCEPTED_CULTURES;
        };
        accepted_only_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.immigration == ALLOW_ACCEPTED_CULTURES)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
                
        UI::Checkbox* all_payment_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        all_payment_btn->text("All with payment");
        all_payment_btn->below_of(*accepted_only_btn);
        all_payment_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.immigration = ALLOW_ALL_PAYMENT;
        };
        all_payment_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.immigration == ALLOW_ALL_PAYMENT)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
                
        UI::Checkbox* allow_all_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        allow_all_btn->text("All");
        allow_all_btn->below_of((*all_payment_btn));
        allow_all_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.immigration = ALLOW_ALL;
        };
        allow_all_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.immigration == ALLOW_ALL)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
    }

    // Population - Treatment
    x_off += 200;
    {
        UI::Label* section_lab = new UI::Label(x_off, y_off, "Treatment", reform_win);
        UI::Checkbox* everyone_equal_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        everyone_equal_btn->text("Everyone equal");
        everyone_equal_btn->below_of(*section_lab);
        everyone_equal_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.treatment = TREATMENT_EVERYONE_EQUAL;
        };
        everyone_equal_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.treatment == TREATMENT_EVERYONE_EQUAL)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
            
        UI::Checkbox* only_accepted_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        only_accepted_btn->text("Only accepted");
        only_accepted_btn->below_of((*everyone_equal_btn));
        only_accepted_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.treatment = TREATMENT_ONLY_ACCEPTED;
        };
        only_accepted_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.treatment == TREATMENT_ONLY_ACCEPTED)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
            
        UI::Checkbox* enslaved_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        enslaved_btn->text("Enslaved");
        enslaved_btn->below_of((*only_accepted_btn));
        enslaved_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.treatment = TREATMENT_ENSLAVED;
        };
        enslaved_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.treatment == TREATMENT_ENSLAVED)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
            
        UI::Checkbox* exterminate_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        exterminate_btn->text("Exterminate");
        exterminate_btn->below_of(*enslaved_btn);
        exterminate_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.treatment = TREATMENT_EXTERMINATE;
        };
        exterminate_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.treatment == TREATMENT_EXTERMINATE)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
    }

    // Goverment - Censorship
    x_off = 0;
    y_off += 24 * 5;
    {
        UI::Label* section_lab = new UI::Label(x_off, y_off, "Censorship", reform_win);
        UI::Checkbox* all_censored_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        all_censored_btn->text("All censored");
        all_censored_btn->below_of(*section_lab);
        all_censored_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.censorship = CENSORSHIP_ALL_CENSORED;
        };
        all_censored_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.censorship == CENSORSHIP_ALL_CENSORED)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
            
        UI::Checkbox* only_state_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        only_state_btn->text("State only");
        only_state_btn->below_of(*all_censored_btn);
        only_state_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.censorship = CENSORSHIP_ONLY_STATE;
        };
        only_state_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.censorship == CENSORSHIP_ONLY_STATE)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
            
        UI::Checkbox* only_approved_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        only_approved_btn->text("Approved only");
        only_approved_btn->below_of((*only_state_btn));
        only_approved_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.censorship = CENSORSHIP_ONLY_APPROVED;
        };
        only_approved_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.censorship == CENSORSHIP_ONLY_APPROVED)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
            
        UI::Checkbox* all_allowed_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        all_allowed_btn->text("All allowed");
        all_allowed_btn->below_of(*only_approved_btn);
        all_allowed_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.censorship = CENSORSHIP_ALL_ALLOWED;
        };
        all_allowed_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.censorship == CENSORSHIP_ALL_ALLOWED)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
    }

    // Government - Infrastructure
    x_off += 200;
    {
        UI::Label* section_lab = new UI::Label(x_off, y_off, "Infrastructure", reform_win);
        UI::Checkbox* not_allowed_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        not_allowed_btn->text("Not allowed");
        not_allowed_btn->below_of(*section_lab);
        not_allowed_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.build_infrastructure = AUTO_BUILD_NONE;
        };
        not_allowed_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.build_infrastructure == AUTO_BUILD_NONE)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
            
        UI::Checkbox* with_approval_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        with_approval_btn->text("With approval");
        with_approval_btn->below_of(*not_allowed_btn);
        with_approval_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.build_infrastructure = AUTO_BUILD_ONLY_APPROVED;
        };
        with_approval_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.build_infrastructure == AUTO_BUILD_ONLY_APPROVED)
            ? checkbox_ppv_on
            : checkbox_ppv_off;
        };
            
        UI::Checkbox* allowed_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        allowed_btn->text("Allowed");
        allowed_btn->below_of(*with_approval_btn);
        allowed_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.build_infrastructure = AUTO_BUILD_ALLOWED;
        };
        allowed_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.build_infrastructure == AUTO_BUILD_ALLOWED)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
    }

    // Government - Industry
    x_off += 200;
    {
        UI::Label* section_lab = new UI::Label(x_off, y_off, "Industry", reform_win);
        UI::Checkbox* not_allowed_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        not_allowed_btn->text("Not allowed");
        not_allowed_btn->below_of(*section_lab);
        not_allowed_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.build_factories = AUTO_BUILD_NONE;
        };
        not_allowed_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.build_factories == AUTO_BUILD_NONE)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
            
        UI::Checkbox* with_approval_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        with_approval_btn->text("With approval");
        with_approval_btn->below_of(*not_allowed_btn);
        with_approval_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.build_factories = AUTO_BUILD_ONLY_APPROVED;
        };
        with_approval_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.build_factories == AUTO_BUILD_ONLY_APPROVED)
            ? checkbox_ppv_on
            : checkbox_ppv_off;
        };
            
        UI::Checkbox* allowed_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        allowed_btn->text("Allowed");
        allowed_btn->below_of(*with_approval_btn);
        allowed_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.build_factories = AUTO_BUILD_ALLOWED;
        };
        allowed_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.build_factories == AUTO_BUILD_ALLOWED)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
    }

    // Government - National ID
    x_off += 200;
    {
        UI::Label* section_lab = new UI::Label(x_off, y_off, "Miscellaneous", reform_win);
        UI::Checkbox* national_id_btn = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        national_id_btn->text("National ID");
        national_id_btn->below_of(*section_lab);
        national_id_btn->on_click = [](UI::Widget&, void*) {
            draft_policy.national_id = !draft_policy.national_id;
        };
        national_id_btn->on_update = [](UI::Widget& w, void*) {
            w.current_texture = (draft_policy.national_id)
            ? checkbox_ppv_on : checkbox_ppv_off;
        };
    }

    // Methods - Laws
    x_off = 0;
    y_off += 24 * 5;
    {
        UI::Label* section_lab = new UI::Label(x_off, y_off, "Laws", reform_win);
        UI::Checkbox* private_property_check = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        private_property_check->text("Private property");
        private_property_check->below_of(*section_lab);
        private_property_check->on_click = [](UI::Widget&, void*) {
            draft_policy.private_property = !draft_policy.private_property;
        };
        private_property_check->on_update = [](UI::Widget& w, void*) {
            w.current_texture =
                (draft_policy.private_property)
                ? checkbox_ppv_on : checkbox_ppv_off;
        };

        UI::Checkbox* companies_check = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        companies_check->text("Private Corporations");
        companies_check->below_of(*private_property_check);
        companies_check->on_click = [](UI::Widget&, void*) {
            draft_policy.companies_allowed = !draft_policy.companies_allowed;
        };
        companies_check->on_update = [](UI::Widget& w, void*) {
            w.current_texture =
                (draft_policy.companies_allowed)
                ? checkbox_ppv_on : checkbox_ppv_off;
        };
    }
    
    // Methods - Rights
    x_off += 200;
    {
        UI::Label* section_lab = new UI::Label(x_off, y_off, "Laws", reform_win);
        UI::Checkbox* public_education_check = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        public_education_check->text("Public education");
        public_education_check->below_of(*section_lab);
        public_education_check->on_click = [](UI::Widget&, void*) {
            draft_policy.public_education = !draft_policy.public_education;
        };
        public_education_check->on_update = [](UI::Widget& w, void*) {
            w.current_texture =
                (draft_policy.public_education)
                ? checkbox_ppv_on : checkbox_ppv_off;
        };

        UI::Checkbox* public_healthcare_check = new UI::Checkbox(x_off, 0, 196, 24, reform_win);
        public_healthcare_check->text("Public healthcare");
        public_healthcare_check->below_of(*public_education_check);
        public_healthcare_check->on_click = [](UI::Widget&, void*) {
            draft_policy.public_healthcare = !draft_policy.public_healthcare;
        };
        public_healthcare_check->on_update = [](UI::Widget& w, void*) {
            w.current_texture =
                (draft_policy.public_healthcare)
                ? checkbox_ppv_on : checkbox_ppv_off;
        };
    }

    // Taxes - Exterior
    x_off = 0;
    y_off += 24 * 4;
    {
        UI::Label* section_lab = new UI::Label(x_off, y_off, "Exterior", reform_win);
        UI::Slider* import_tax = new UI::Slider(x_off, 0, 800, 24, 0.f, 200.f, reform_win);
        import_tax->below_of(*section_lab);
        import_tax->value = 50.f;
        import_tax->on_click = ([](UI::Widget& w, void*) {
            UI::Slider& wc = dynamic_cast<UI::Slider&>(w);
            draft_policy.import_tax = wc.value - 100.f;
            sprintf(tmpbuf, "Import (%.2f%%)", draft_policy.import_tax);
            w.text(tmpbuf);
        });

        UI::Slider* export_tax = new UI::Slider(x_off, 0, 800, 24, 0.f, 200.f, reform_win);
        export_tax->below_of(*import_tax);
        export_tax->value = 50.f;
        export_tax->on_click = ([](UI::Widget& w, void*) {
            UI::Slider& wc = dynamic_cast<UI::Slider&>(w);
            draft_policy.export_tax = wc.value - 100.f;
            sprintf(tmpbuf, "Export (%.2f%%)", draft_policy.export_tax);
            w.text(tmpbuf);
        });
    }

    // Taxes - Domestic
    x_off = 0;
    y_off += 24 * 3;
    {
        UI::Label* section_lab = new UI::Label(x_off, y_off, "Domestic", reform_win);
        UI::Slider* domestic_import_tax = new UI::Slider(x_off, 0, 800, 24, 0.f, 200.f, reform_win);
        domestic_import_tax->below_of(*section_lab);
        domestic_import_tax->value = 50.f;
        domestic_import_tax->on_click = ([](UI::Widget& w, void*) {
            UI::Slider& wc = dynamic_cast<UI::Slider&>(w);
            draft_policy.domestic_import_tax = wc.value - 100.f;
            sprintf(tmpbuf, "Import (%.2f%%)", draft_policy.domestic_import_tax);
            w.text(tmpbuf);
        });

        UI::Slider* domestic_export_tax = new UI::Slider(x_off, 0, 800, 24, 0.f, 200.f, reform_win);
        domestic_export_tax->below_of(*domestic_import_tax);
        domestic_export_tax->value = 50.f;
        domestic_export_tax->on_click = ([](UI::Widget& w, void*) {
            UI::Slider& wc = dynamic_cast<UI::Slider&>(w);
            draft_policy.domestic_export_tax = wc.value - 100.f;
            sprintf(tmpbuf, "Export (%.2f%%)", draft_policy.domestic_export_tax);
            w.text(tmpbuf);
        });
    }

    // Taxes - Classes
    x_off = 0;
    y_off += 24 * 3;
    {
        UI::Label* section_lab = new UI::Label(x_off, y_off, "Classes", reform_win);
        UI::Slider* poor_flat_tax = new UI::Slider(x_off, 0, 800, 24, 0.f, 200.f, reform_win);
        poor_flat_tax->below_of(*section_lab);
        poor_flat_tax->value = 50.f;
        poor_flat_tax->on_click = ([](UI::Widget& w, void*) {
            UI::Slider& wc = dynamic_cast<UI::Slider&>(w);
            draft_policy.poor_flat_tax = wc.value - 100.f;
            sprintf(tmpbuf, "Poor flat tax (%.2f%%)", draft_policy.poor_flat_tax);
            w.text(tmpbuf);
        });

        UI::Slider* med_flat_tax = new UI::Slider(x_off, 0, 800, 24, 0.f, 200.f, reform_win);
        med_flat_tax->below_of(*poor_flat_tax);
        med_flat_tax->value = 50.f;
        med_flat_tax->on_click = ([](UI::Widget& w, void*) {
            UI::Slider& wc = dynamic_cast<UI::Slider&>(w);
            draft_policy.med_flat_tax = wc.value - 100.f;
            sprintf(tmpbuf, "Med. flat tax (%.2f%%)", draft_policy.med_flat_tax);
            w.text(tmpbuf);
        });

        UI::Slider* rich_flat_tax = new UI::Slider(x_off, 0, 800, 24, 0.f, 200.f, reform_win);
        rich_flat_tax->below_of(*med_flat_tax);
        rich_flat_tax->value = 50.f;
        rich_flat_tax->on_click = ([](UI::Widget& w, void*) {
            UI::Slider& wc = dynamic_cast<UI::Slider&>(w);
            draft_policy.rich_flat_tax = wc.value - 100.f;
            sprintf(tmpbuf, "Rich flat tax (%.2f%%)", draft_policy.rich_flat_tax);
            w.text(tmpbuf);
        });
    }

    // Action and close buttons
    {
        UI::Button* enact_btn = new UI::Button(0, 590, 196, 24, reform_win);
        enact_btn->text("Draft bill");
        enact_btn->on_click = [](UI::Widget&, void*) {
            g_client->packet_mutex.lock();
            Packet packet = Packet(g_client->get_fd());
            Archive ar = Archive();
            
            enum ActionType action = ACTION_NATION_ENACT_POLICY;
            ::serialize(ar, &action);
            ::serialize(ar, &draft_policy); // PoliciesObj
            
            packet.data(ar.get_buffer(), ar.size());
            g_client->packet_queue.push_back(packet);
            g_client->packet_mutex.unlock();

            UI::Window* prompt_win = new UI::Window(mouse_pos.first, mouse_pos.second, 256, 64);
            prompt_win->text("Bill passed");
            UI::CloseButton* ok_btn = new UI::CloseButton(9, 24, 64, 24, prompt_win);
            ok_btn->text("OK");
        };

        UI::CloseButton* ok_btn = new UI::CloseButton(0, 590, 196, 24, reform_win);
        ok_btn->text("OK");
        ok_btn->right_side_of(*enact_btn);
    }
}
