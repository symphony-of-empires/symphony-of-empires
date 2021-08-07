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

void ui_reform_policies(UI::Widget&, void*) {
    draft_policy = curr_nation->current_policy;
    checkbox_ppv_on = &g_texture_manager->load_texture(Path::get("ui/checkbox_ppv_on.png"));
    checkbox_ppv_off = &g_texture_manager->load_texture(Path::get("ui/checkbox_ppv_off.png"));
    
    UI::Window* reform_win = new UI::Window(mouse_pos.first, mouse_pos.second, 320, 425);
    reform_win->text("Policies");
    
    UI::Button* borders_btn = new UI::Button(9, 43, 303, 38, reform_win);
    borders_btn->text("Population control");
    borders_btn->on_click = [](UI::Widget&, void*) {
        UI::Window* reform_win = new UI::Window(mouse_pos.first, mouse_pos.second, 320, 425);
        reform_win->text("Population control");

        UI::Button* migration_btn = new UI::Button(9, 43, 303, 38, reform_win);
        migration_btn->text("Migration");
        migration_btn->on_click = [](UI::Widget&, void*) {
            UI::Window* reform_win = new UI::Window(mouse_pos.first, mouse_pos.second, 320, 425);
            reform_win->text("Migration");
            
            UI::Button* nobody_btn = new UI::Button(9, 43, 303, 38, reform_win);
            nobody_btn->text("Nobody");
            nobody_btn->on_click = [](UI::Widget&, void*) {
                draft_policy.migration = ALLOW_NOBODY;
            };
            nobody_btn->on_update = [](UI::Widget& w, void*) {
                w.current_texture = (draft_policy.migration == ALLOW_NOBODY)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
            };
            
            UI::Button* accepted_only_btn = new UI::Button(9, 43, 303, 38, reform_win);
            accepted_only_btn->text("Accepted culture");
            accepted_only_btn->below_of((*nobody_btn));
            accepted_only_btn->on_click = [](UI::Widget&, void*) {
                draft_policy.migration = ALLOW_ACCEPTED_CULTURES;
            };
            accepted_only_btn->on_update = [](UI::Widget& w, void*) {
                w.current_texture = (draft_policy.migration == ALLOW_ACCEPTED_CULTURES)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
            };
            
            UI::Button* all_payment_btn = new UI::Button(9, 43, 303, 38, reform_win);
            all_payment_btn->text("All with payment");
            all_payment_btn->below_of((*accepted_only_btn));
            all_payment_btn->on_click = [](UI::Widget&, void*) {
                draft_policy.migration = ALLOW_ALL_PAYMENT;
            };
            all_payment_btn->on_update = [](UI::Widget& w, void*) {
                w.current_texture = (draft_policy.migration == ALLOW_ALL_PAYMENT)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
            };
            
            UI::Button* allow_all_btn = new UI::Button(9, 43, 303, 38, reform_win);
            allow_all_btn->text("All");
            allow_all_btn->below_of((*all_payment_btn));
            allow_all_btn->on_click = [](UI::Widget&, void*) {
                draft_policy.migration = ALLOW_ALL;
            };
            allow_all_btn->on_update = [](UI::Widget& w, void*) {
                w.current_texture = (draft_policy.migration == ALLOW_ALL)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
            };
            
            UI::CloseButton* ok_btn = new UI::CloseButton(9, 0, 303, 38, reform_win);
            ok_btn->text("OK");
            ok_btn->below_of((*allow_all_btn));
        };
        
        // Control the immigration of POPs
        UI::Button* immigration_btn = new UI::Button(9, 43, 303, 38, reform_win);
        immigration_btn->text("Immigration");
        immigration_btn->below_of((*migration_btn));
        immigration_btn->on_click = [](UI::Widget&, void*) {
            UI::Window* reform_win = new UI::Window(mouse_pos.first, mouse_pos.second, 320, 425);
            reform_win->text("Immigration");
            
            UI::Button* nobody_btn = new UI::Button(9, 43, 303, 38, reform_win);
            nobody_btn->text("Nobody");
            nobody_btn->on_click = [](UI::Widget&, void*) {
                draft_policy.immigration = ALLOW_NOBODY;
            };
            nobody_btn->on_update = [](UI::Widget& w, void*) {
                w.current_texture = (draft_policy.immigration == ALLOW_NOBODY)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
            };
            
            UI::Button* accepted_only_btn = new UI::Button(9, 43, 303, 38, reform_win);
            accepted_only_btn->text("Accepted culture");
            accepted_only_btn->below_of((*nobody_btn));
            accepted_only_btn->on_click = [](UI::Widget&, void*) {
                draft_policy.immigration = ALLOW_ACCEPTED_CULTURES;
            };
            accepted_only_btn->on_update = [](UI::Widget& w, void*) {
                w.current_texture = (draft_policy.immigration == ALLOW_ACCEPTED_CULTURES)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
            };
            
            UI::Button* all_payment_btn = new UI::Button(9, 43, 303, 38, reform_win);
            all_payment_btn->text("All with payment");
            all_payment_btn->below_of((*accepted_only_btn));
            all_payment_btn->on_click = [](UI::Widget&, void*) {
                draft_policy.immigration = ALLOW_ALL_PAYMENT;
            };
            all_payment_btn->on_update = [](UI::Widget& w, void*) {
                w.current_texture = (draft_policy.immigration == ALLOW_ALL_PAYMENT)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
            };
            
            UI::Button* allow_all_btn = new UI::Button(9, 43, 303, 38, reform_win);
            allow_all_btn->text("All");
            allow_all_btn->below_of((*all_payment_btn));
            allow_all_btn->on_click = [](UI::Widget&, void*) {
                draft_policy.immigration = ALLOW_ALL;
            };
            allow_all_btn->on_update = [](UI::Widget& w, void*) {
                w.current_texture = (draft_policy.immigration == ALLOW_ALL)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
            };
            
            UI::CloseButton* ok_btn = new UI::CloseButton(9, 0, 303, 38, reform_win);
            ok_btn->text("OK");
            ok_btn->below_of((*allow_all_btn));
        };
        
        // Control the treatment of POPs
        UI::Button* treatment_btn = new UI::Button(9, 43, 303, 38, reform_win);
        treatment_btn->text("Treatment");
        treatment_btn->below_of((*immigration_btn));
        treatment_btn->on_click = [](UI::Widget&, void*) {
            UI::Window* reform_win = new UI::Window(mouse_pos.first, mouse_pos.second, 320, 425);
            reform_win->text("Treatment");
            
            UI::Button* everyone_equal_btn = new UI::Button(9, 43, 303, 38, reform_win);
            everyone_equal_btn->text("Everyone equal");
            everyone_equal_btn->on_click = [](UI::Widget&, void*) {
                draft_policy.treatment = TREATMENT_EVERYONE_EQUAL;
            };
            everyone_equal_btn->on_update = [](UI::Widget& w, void*) {
                w.current_texture = (draft_policy.treatment == TREATMENT_EVERYONE_EQUAL)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
            };
            
            UI::Button* only_accepted_btn = new UI::Button(9, 43, 303, 38, reform_win);
            only_accepted_btn->text("Only accepted");
            only_accepted_btn->below_of((*everyone_equal_btn));
            only_accepted_btn->on_click = [](UI::Widget&, void*) {
                draft_policy.treatment = TREATMENT_ONLY_ACCEPTED;
            };
            only_accepted_btn->on_update = [](UI::Widget& w, void*) {
                w.current_texture = (draft_policy.treatment == TREATMENT_ONLY_ACCEPTED)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
            };
            
            UI::Button* enslaved_btn = new UI::Button(9, 43, 303, 38, reform_win);
            enslaved_btn->text("Enslaved");
            enslaved_btn->below_of((*only_accepted_btn));
            enslaved_btn->on_click = [](UI::Widget&, void*) {
                draft_policy.treatment = TREATMENT_ENSLAVED;
            };
            enslaved_btn->on_update = [](UI::Widget& w, void*) {
                w.current_texture = (draft_policy.treatment == TREATMENT_ENSLAVED)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
            };
            
            UI::Button* exterminate_btn = new UI::Button(9, 43, 303, 38, reform_win);
            exterminate_btn->text("Exterminate");
            exterminate_btn->below_of(*enslaved_btn);
            exterminate_btn->on_click = [](UI::Widget&, void*) {
                draft_policy.treatment = TREATMENT_EXTERMINATE;
            };
            exterminate_btn->on_update = [](UI::Widget& w, void*) {
                w.current_texture = (draft_policy.treatment == TREATMENT_EXTERMINATE)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
            };
            
            UI::CloseButton* ok_btn = new UI::CloseButton(9, 0, 303, 38, reform_win);
            ok_btn->text("OK");
            ok_btn->below_of((*exterminate_btn));
        };
        
        UI::CloseButton* ok_btn = new UI::CloseButton(9, 0, 303, 38, reform_win);
        ok_btn->text("OK");
        ok_btn->below_of(*treatment_btn);
    };
    
    UI::Button* goverment_btn = new UI::Button(9, 0, 303, 38, reform_win);
    goverment_btn->text("Goverment");
    goverment_btn->below_of(*borders_btn);
    goverment_btn->on_click = [](UI::Widget&, void*) {
        UI::Window* reform_win = new UI::Window(mouse_pos.first, mouse_pos.second, 320, 425);
        reform_win->text("Goverment policies");
        
        // Control the media censorship
        UI::Button* censorship_btn = new UI::Button(9, 43, 303, 38, reform_win);
        censorship_btn->text("Free press");
        censorship_btn->on_click = [](UI::Widget&, void*) {
            UI::Window* reform_win = new UI::Window(mouse_pos.first, mouse_pos.second, 320, 425);
            reform_win->text("Free press");
            
            UI::Button* all_censored_btn = new UI::Button(9, 43, 303, 38, reform_win);
            all_censored_btn->text("All censored");
            all_censored_btn->on_click = [](UI::Widget&, void*) {
                draft_policy.censorship = CENSORSHIP_ALL_CENSORED;
            };
            all_censored_btn->on_update = [](UI::Widget& w, void*) {
                w.current_texture = (draft_policy.censorship == CENSORSHIP_ALL_CENSORED)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
            };
            
            UI::Button* only_state_btn = new UI::Button(9, 43, 303, 38, reform_win);
            only_state_btn->text("State only");
            only_state_btn->below_of(*all_censored_btn);
            only_state_btn->on_click = [](UI::Widget&, void*) {
                draft_policy.censorship = CENSORSHIP_ONLY_STATE;
            };
            only_state_btn->on_update = [](UI::Widget& w, void*) {
                w.current_texture = (draft_policy.censorship == CENSORSHIP_ONLY_STATE)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
            };
            
            UI::Button* only_approved_btn = new UI::Button(9, 43, 303, 38, reform_win);
            only_approved_btn->text("Approved only");
            only_approved_btn->below_of((*only_state_btn));
            only_approved_btn->on_click = [](UI::Widget&, void*) {
                draft_policy.censorship = CENSORSHIP_ONLY_APPROVED;
            };
            only_approved_btn->on_update = [](UI::Widget& w, void*) {
                w.current_texture = (draft_policy.censorship == CENSORSHIP_ONLY_APPROVED)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
            };
            
            UI::Button* all_allowed_btn = new UI::Button(9, 43, 303, 38, reform_win);
            all_allowed_btn->text("All allowed");
            all_allowed_btn->below_of(*only_approved_btn);
            all_allowed_btn->on_click = [](UI::Widget&, void*) {
                draft_policy.censorship = CENSORSHIP_ALL_ALLOWED;
            };
            all_allowed_btn->on_update = [](UI::Widget& w, void*) {
                w.current_texture = (draft_policy.censorship == CENSORSHIP_ALL_ALLOWED)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
            };
            
            UI::CloseButton* ok_btn = new UI::CloseButton(9, 0, 303, 38, reform_win);
            ok_btn->text("OK");
            ok_btn->below_of(*all_allowed_btn);
        };
        
        // Control the build of infrastructure done by POPs
        UI::Button* build_infra_btn = new UI::Button(9, 43, 303, 38, reform_win);
        build_infra_btn->text("Infrastructure");
        build_infra_btn->below_of((*censorship_btn));
        build_infra_btn->on_click = [](UI::Widget&, void*) {
            UI::Window* reform_win = new UI::Window(mouse_pos.first, mouse_pos.second, 320, 425);
            reform_win->text("Infrastructure built by POPs");
            
            UI::Button* not_allowed_btn = new UI::Button(9, 43, 303, 38, reform_win);
            not_allowed_btn->text("Not allowed");
            not_allowed_btn->on_click = [](UI::Widget&, void*) {
                draft_policy.build_infrastructure = AUTO_BUILD_NONE;
            };
            not_allowed_btn->on_update = [](UI::Widget& w, void*) {
                w.current_texture = (draft_policy.build_infrastructure == AUTO_BUILD_NONE)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
            };
            
            UI::Button* with_approval_btn = new UI::Button(9, 43, 303, 38, reform_win);
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
            
            UI::Button* allowed_btn = new UI::Button(9, 43, 303, 38, reform_win);
            allowed_btn->text("Allowed");
            allowed_btn->below_of(*with_approval_btn);
            allowed_btn->on_click = [](UI::Widget&, void*) {
                draft_policy.build_infrastructure = AUTO_BUILD_ALLOWED;
            };
            allowed_btn->on_update = [](UI::Widget& w, void*) {
                w.current_texture = (draft_policy.build_infrastructure == AUTO_BUILD_ALLOWED)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
            };
            
            UI::CloseButton* ok_btn = new UI::CloseButton(9, 0, 303, 38, reform_win);
            ok_btn->text("OK");
            ok_btn->below_of(*allowed_btn);
        };
        
        UI::Button* build_factories_btn = new UI::Button(9, 43, 303, 38, reform_win);
        build_factories_btn->text("Industry");
        build_factories_btn->below_of((*build_infra_btn));
        build_factories_btn->on_click = [](UI::Widget&, void*) {
            UI::Window* reform_win = new UI::Window(mouse_pos.first, mouse_pos.second, 320, 425);
            reform_win->text("Industry built by POPs");
            
            UI::Button* not_allowed_btn = new UI::Button(9, 43, 303, 38, reform_win);
            not_allowed_btn->text("Not allowed");
            not_allowed_btn->on_click = [](UI::Widget&, void*) {
                draft_policy.build_factories = AUTO_BUILD_NONE;
            };
            not_allowed_btn->on_update = [](UI::Widget& w, void*) {
                w.current_texture = (draft_policy.build_factories == AUTO_BUILD_NONE)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
            };
            
            UI::Button* with_approval_btn = new UI::Button(9, 43, 303, 38, reform_win);
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
            
            UI::Button* allowed_btn = new UI::Button(9, 43, 303, 38, reform_win);
            allowed_btn->text("Allowed");
            allowed_btn->below_of(*with_approval_btn);
            allowed_btn->on_click = [](UI::Widget&, void*) {
                draft_policy.build_factories = AUTO_BUILD_ALLOWED;
            };
            allowed_btn->on_update = [](UI::Widget& w, void*) {
                w.current_texture = (draft_policy.build_factories == AUTO_BUILD_ALLOWED)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
            };
            
            UI::CloseButton* ok_btn = new UI::CloseButton(9, 0, 303, 38, reform_win);
            ok_btn->text("OK");
            ok_btn->below_of(*allowed_btn);
        };
        
        UI::Checkbox* national_id_check = new UI::Checkbox(9, 43, 303, 38, reform_win);
        national_id_check->text("National ID");
        national_id_check->below_of((*build_factories_btn));
        if(draft_policy.national_id) {
            national_id_check->current_texture = checkbox_ppv_on;
        } else {
            national_id_check->current_texture = checkbox_ppv_off;
        }
        national_id_check->on_click = [](UI::Widget& w, void*) {
            draft_policy.national_id = !draft_policy.national_id;
            if(draft_policy.national_id) {
                w.current_texture = checkbox_ppv_on;
            } else {
                w.current_texture = checkbox_ppv_off;
            }
        };
        
        UI::CloseButton* ok_btn = new UI::CloseButton(9, 0, 303, 38, reform_win);
        ok_btn->text("OK");
        ok_btn->below_of(*national_id_check);
    };
    
    UI::Button* taxes_btn = new UI::Button(9, 0, 303, 38, reform_win);
    taxes_btn->text("Taxes");
    taxes_btn->below_of(*goverment_btn);
    taxes_btn->on_click = [](UI::Widget&, void*) {
        UI::Window* reform_win = new UI::Window(mouse_pos.first, mouse_pos.second, 320, 425);
        reform_win->text("Taxing");
        
        UI::Button* inc_export_tax = new UI::Button(9, 43, 303, 38, reform_win);
        inc_export_tax->text("Export taxes +1%");
        inc_export_tax->on_click = [](UI::Widget&, void*) {
            draft_policy.export_tax += 0.01f;
        };

        UI::Button* dec_export_tax = new UI::Button(9, 43, 303, 38, reform_win);
        dec_export_tax->below_of(*inc_export_tax);
        dec_export_tax->text("Export taxes -1%");
        dec_export_tax->on_click = [](UI::Widget&, void*) {
            draft_policy.export_tax -= 0.01f;
        };

        UI::Label* disp_export_tax = new UI::Label(9, 43, "?", reform_win);
        disp_export_tax->below_of(*dec_export_tax);
        disp_export_tax->on_update = [](UI::Widget& w, void*) {
            char* tmpbuf = new char[255];
            sprintf(tmpbuf, "%8.2f%%", draft_policy.export_tax * 100.f);
            w.text(tmpbuf);
            delete[] tmpbuf;
        };

        UI::Button* inc_import_tax = new UI::Button(9, 43, 303, 38, reform_win);
        inc_import_tax->below_of(*disp_export_tax);
        inc_import_tax->text("Import taxes +1%");
        inc_import_tax->on_click = [](UI::Widget&, void*) {
            draft_policy.import_tax += 0.01f;
        };

        UI::Button* dec_import_tax = new UI::Button(9, 43, 303, 38, reform_win);
        dec_import_tax->below_of(*inc_import_tax);
        dec_import_tax->text("Import taxes -1%");
        dec_import_tax->on_click = [](UI::Widget&, void*) {
            draft_policy.import_tax -= 0.01f;
        };

        UI::Label* disp_import_tax = new UI::Label(9, 43, "?", reform_win);
        disp_import_tax->below_of(*dec_import_tax);
        disp_import_tax->on_update = [](UI::Widget& w, void*) {
            char* tmpbuf = new char[255];
            sprintf(tmpbuf, "%8.2f%%", draft_policy.import_tax * 100.f);
            w.text(tmpbuf);
            delete[] tmpbuf;
        };

        UI::Button* inc_poor_flat_tax = new UI::Button(9, 43, 303, 38, reform_win);
        inc_poor_flat_tax->below_of(*disp_import_tax);
        inc_poor_flat_tax->text("Poor flat taxes +1%");
        inc_poor_flat_tax->on_click = [](UI::Widget&, void*) {
            draft_policy.poor_flat_tax += 0.01f;
        };

        UI::Button* dec_poor_flat_tax = new UI::Button(9, 43, 303, 38, reform_win);
        dec_poor_flat_tax->below_of(*inc_poor_flat_tax);
        dec_poor_flat_tax->text("Poor flat taxes -1%");
        dec_poor_flat_tax->on_click = [](UI::Widget&, void*) {
            draft_policy.poor_flat_tax -= 0.01f;
        };

        UI::Label* disp_poor_flat_tax = new UI::Label(9, 43, "?", reform_win);
        disp_poor_flat_tax->below_of(*dec_poor_flat_tax);
        disp_poor_flat_tax->on_update = [](UI::Widget& w, void*) {
            char* tmpbuf = new char[255];
            sprintf(tmpbuf, "%8.2f%%", draft_policy.poor_flat_tax * 100.f);
            w.text(tmpbuf);
            delete[] tmpbuf;
        };

        UI::Button* inc_med_flat_tax = new UI::Button(9, 43, 303, 38, reform_win);
        inc_med_flat_tax->below_of(*disp_poor_flat_tax);
        inc_med_flat_tax->text("Med. flat taxes +1%");
        inc_med_flat_tax->on_click = [](UI::Widget&, void*) {
            draft_policy.med_flat_tax += 0.01f;
        };

        UI::Button* dec_med_flat_tax = new UI::Button(9, 43, 303, 38, reform_win);
        dec_med_flat_tax->below_of(*inc_med_flat_tax);
        dec_med_flat_tax->text("Med. flat taxes -1%");
        dec_med_flat_tax->on_click = [](UI::Widget&, void*) {
            draft_policy.med_flat_tax -= 0.01f;
        };

        UI::Label* disp_med_flat_tax = new UI::Label(9, 43, "?", reform_win);
        disp_med_flat_tax->below_of(*dec_med_flat_tax);
        disp_med_flat_tax->on_update = [](UI::Widget& w, void*) {
            char* tmpbuf = new char[255];
            sprintf(tmpbuf, "%8.2f%%", draft_policy.med_flat_tax * 100.f);
            w.text(tmpbuf);
            delete[] tmpbuf;
        };

        UI::Button* inc_rich_flat_tax = new UI::Button(9, 43, 303, 38, reform_win);
        inc_rich_flat_tax->below_of(*disp_med_flat_tax);
        inc_rich_flat_tax->text("Rich flat taxes +1%");
        inc_rich_flat_tax->on_click = [](UI::Widget&, void*) {
            draft_policy.rich_flat_tax += 0.01f;
        };

        UI::Button* dec_rich_flat_tax = new UI::Button(9, 43, 303, 38, reform_win);
        dec_rich_flat_tax->below_of(*inc_rich_flat_tax);
        dec_rich_flat_tax->text("Rich flat taxes -1%");
        dec_rich_flat_tax->on_click = [](UI::Widget&, void*) {
            draft_policy.rich_flat_tax -= 0.01f;
        };

        UI::Label* disp_rich_flat_tax = new UI::Label(9, 43, "?", reform_win);
        disp_rich_flat_tax->below_of(*dec_rich_flat_tax);
        disp_rich_flat_tax->on_update = [](UI::Widget& w, void*) {
            char* tmpbuf = new char[255];
            sprintf(tmpbuf, "%8.2f%%", draft_policy.rich_flat_tax * 100.f);
            w.text(tmpbuf);
            delete[] tmpbuf;
        };

        UI::CloseButton* ok_btn = new UI::CloseButton(9, 0, 303, 38, reform_win);
        ok_btn->text("OK");
        ok_btn->below_of(*disp_rich_flat_tax);
    };
    
    UI::Button* property_btn = new UI::Button(9, 0, 303, 38, reform_win);
    property_btn->text("Property");
    property_btn->below_of((*taxes_btn));
    property_btn->on_click = [](UI::Widget&, void*) {
        UI::Window* reform_win = new UI::Window(mouse_pos.first, mouse_pos.second, 320, 425);
        reform_win->text("Property policies");
        
        UI::Checkbox* private_property_check = new UI::Checkbox(9, 43, 303, 38, reform_win);
        private_property_check->text("Private property");
        private_property_check->on_click = [](UI::Widget&, void*) {
            draft_policy.private_property = !draft_policy.private_property;
        };
        private_property_check->on_update = [](UI::Widget& w, void*) {
            w.current_texture =
                (draft_policy.private_property)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
        };

        UI::Checkbox* companies_check = new UI::Checkbox(9, 43, 303, 38, reform_win);
        companies_check->text("Companies");
        companies_check->below_of((*private_property_check));
        companies_check->on_click = [](UI::Widget&, void*) {
            draft_policy.companies_allowed = !draft_policy.companies_allowed;
        };
        companies_check->on_update = [](UI::Widget& w, void*) {
            w.current_texture =
                (draft_policy.companies_allowed)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
        };

        UI::Checkbox* public_education_check = new UI::Checkbox(9, 43, 303, 38, reform_win);
        public_education_check->text("Public education");
        public_education_check->below_of((*companies_check));
        public_education_check->on_click = [](UI::Widget&, void*) {
            draft_policy.public_education = !draft_policy.public_education;
        };
        public_education_check->on_update = [](UI::Widget& w, void*) {
            w.current_texture =
                (draft_policy.public_education)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
        };

        UI::Checkbox* public_healthcare_check = new UI::Checkbox(9, 43, 303, 38, reform_win);
        public_healthcare_check->text("Public education");
        public_healthcare_check->below_of((*companies_check));
        public_healthcare_check->on_click = [](UI::Widget&, void*) {
            draft_policy.public_healthcare = !draft_policy.public_healthcare;
        };
        public_healthcare_check->on_update = [](UI::Widget& w, void*) {
            w.current_texture =
                (draft_policy.public_healthcare)
                ? checkbox_ppv_on
                : checkbox_ppv_off;
        };

        UI::CloseButton* ok_btn = new UI::CloseButton(9, 0, 303, 38, reform_win);
        ok_btn->text("OK");
        ok_btn->below_of((*public_healthcare_check));
    };
    
    UI::Button* enact_btn = new UI::Button(9, 0, 303, 38, reform_win);
    enact_btn->text("Enact policy");
    enact_btn->below_of((*property_btn));
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

        UI::Window* prompt_win = new UI::Window(mouse_pos.first, mouse_pos.second, 320, 425);
        prompt_win->text("New policy enacted");
        UI::CloseButton* ok_btn = new UI::CloseButton(9, 0, 303, 38, prompt_win);
        ok_btn->text("OK");
    };
    
    UI::CloseButton* ok_btn = new UI::CloseButton(9, 0, 303, 38, reform_win);
    ok_btn->text("OK");
    ok_btn->below_of(*enact_btn);
}
