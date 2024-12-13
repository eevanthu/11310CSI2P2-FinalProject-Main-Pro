#include "FontCenter.h"
#include <iostream>
#include <allegro5/allegro_ttf.h>

// fixed settings
namespace FontSetting {
	const char caviar_dreams_font_path[] = "./assets/font/Caviar_Dreams_Bold.ttf";
	const char courier_new_font_path[] = "./assets/font/courbd.ttf";
	const char boxy_bold_font_path[] = "./assets/font/Boxy-Bold.ttf";
	const char Arial_font_path[] = "./assets/font/Arial_Rounded_Bold.ttf";
}

void
FontCenter::init() {
	for(const int &fs : FontSize::list) {
		caviar_dreams[fs] = al_load_ttf_font(FontSetting::caviar_dreams_font_path, fs, 0);
		courier_new[fs] = al_load_ttf_font(FontSetting::courier_new_font_path, fs, 0);
		boxy_bold[fs] = al_load_ttf_font(FontSetting::boxy_bold_font_path, fs, 0);
		arial_bold[fs] = al_load_ttf_font(FontSetting::Arial_font_path, fs, 0);
		if (arial_bold[fs]) {
			std::cerr << "Failed to load font: " << FontSetting::Arial_font_path << " with size " << fs << std::endl;
		}
		if (!boxy_bold[fs]) {
            std::cerr << "Failed to load font: " << FontSetting::boxy_bold_font_path << " with size " << fs << std::endl;
        }
	}
}

FontCenter::~FontCenter() {
	for(auto &[size, font] : caviar_dreams)
		al_destroy_font(font);
	for(auto &[size, font] : courier_new)
		al_destroy_font(font);
	for(auto &[size, font] : boxy_bold)
		al_destroy_font(font);
	for(auto &[size, font] : arial_bold)
		al_destroy_font(font);
}
