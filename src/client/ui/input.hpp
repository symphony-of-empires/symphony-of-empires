#pragma once

#include <string>

#include "client/ui/widget.hpp"

namespace UI {
	class Context;
	class Input: public Widget {
	public:
		static void on_click_default(Widget& w, void*) {
			Input& input = static_cast<Input&>(w);
			input.is_selected = true;
		}
		static void on_click_outside_default(Widget& w, void*) {
			Input& input = static_cast<Input&>(w);
			if(input.is_selected) {
				input.text(input.buffer);
			}
			input.is_selected = false;
		}
		static void on_update_default(Widget& w, void*) {
			Input& input = static_cast<Input&>(w);
			input.timer = (input.timer + 1) % 60;
			std::string cursor = input.timer >= 30 ? "_" : "";
			if(input.is_selected && input.timer % 30 == 0) {
				if(!input.buffer.empty()) {
					input.text(input.buffer + cursor);
				}
				else if(!cursor.empty()) {
					input.text(cursor);
				}
			}
		}
		Input(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
		virtual ~Input() override {};

		std::function<void(Input&, const char*, void*)> on_textinput;
		std::string buffer = "";
		bool is_selected = false;
	private:
		int timer; // TODO: Needs to not be frame dependand
	};
};