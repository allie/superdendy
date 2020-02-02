#pragma once

#include "common/data.h"
#include "config.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

namespace SuperDendy::Core {
	class Graphics {
	private:
		Config& config;
		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Texture* font;
		Dim dim;

	public:
		Graphics(Config& config);
		~Graphics();

		void create_window(Dim dimensions);
		void render_buffer(RGB* buffer, int width, int height, int x, int y);
		void render_buffer(RGBA* buffer, int width, int height, int x, int y);
		void render_texture(SDL_Texture* texture, SDL_Rect* src, SDL_Rect* dst);
		void set_font_colour(int r, int g, int b);
		void render_string(const std::string& text, int x, int y);
		void render_hex(int long val, int bytes, int x, int y);
		void increase_scale();
		void decrease_scale();
		void set_scale(int new_scale);
		void clear();
		void present();
	};
};
