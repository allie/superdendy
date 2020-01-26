#include "core/graphics.h"
#include <stdexcept>

using namespace SuperDendy::Core;

Graphics::Graphics(
	Logger& logger,
	Config& config
) :
	logger(logger),
	config(config)
{
}

Graphics::~Graphics() {
	SDL_DestroyTexture(font);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void Graphics::create_window(Dim dimensions) {
	dim = dimensions;

	window = SDL_CreateWindow(
		"SuperDendy",
		config.window_pos.x,
		config.window_pos.y,
		dim.w * config.window_scale,
		dim.h * config.window_scale,
		SDL_WINDOW_OPENGL
	);

	if (window == nullptr) {
		logger.fatal("Error creating window: " + (std::string)SDL_GetError());
		throw std::runtime_error("Failed to create window");
	}

	renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (renderer == nullptr) {
		logger.fatal("Error creating renderer: " + (std::string)SDL_GetError());
		throw std::runtime_error("Failed to initialize renderer");
	}

	// const int char font_tex[] = { ... };
	#include "font.partial.cc"

	font = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STATIC, 128, 128);
	SDL_UpdateTexture(font, NULL, font_tex, 128 * 3);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	SDL_RenderSetLogicalSize(renderer, dim.w, dim.h);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

void Graphics::render_buffer(RGB* buffer, int width, int height, int x, int y) {
	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
		(void*)buffer,
		width,
		height,
		24,
		3 * width,
		(Dword)0x0000FF,
		(Dword)0x00FF00,
		(Dword)0xFF0000,
		0
	);

	if (surface == NULL) {
		logger.fatal("Creating surface failed: " + (std::string)SDL_GetError());
		throw std::runtime_error("Failed to create surface");
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	SDL_Rect src = {0, 0, width, height};
	SDL_Rect dst = {x, y, width, height};

	SDL_RenderCopy(renderer, texture, &src, &dst);
}

void Graphics::render_buffer(RGBA* buffer, int width, int height, int x, int y) {
	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
		(void*)buffer,
		width,
		height,
		32,
		4 * width,
		(Dword)0x000000FF,
		(Dword)0x0000FF00,
		(Dword)0x00FF0000,
		(Dword)0xFF000000
	);

	if (surface == NULL) {
		logger.fatal("Creating surface failed: " + (std::string)SDL_GetError());
		throw std::runtime_error("Failed to create surface");
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	SDL_Rect src = {0, 0, width, height};
	SDL_Rect dst = {x, y, width, height};

	SDL_RenderCopy(renderer, texture, &src, &dst);
}

void Graphics::render_texture(SDL_Texture* texture, SDL_Rect* src, SDL_Rect* dst) {
	SDL_RenderCopy(renderer, texture, src, dst);
}

void Graphics::set_font_colour(int r, int g, int b) {
	SDL_SetTextureColorMod(font, r, g, b);
}

void Graphics::render_string(const std::string& text, int x, int y) {
	for (int i = 0; i < text.length(); i++) {
		char c = text[i];
		int tx = c % 16;
		int ty = c / 16;
		SDL_Rect src = {tx * 8, ty * 8, 8, 8};
		SDL_Rect dst = {x * 8 + i * 8, y * 8, 8, 8};
		SDL_RenderCopy(renderer, font, &src, &dst);
	}
}

void Graphics::render_hex(int long val, int bytes, int x, int y) {
	char hex[17];
	snprintf(hex, bytes * 2 + 1, "%0*lX", bytes * 2, val);
	render_string(hex, x, y);
}

void Graphics::increase_scale() {
	set_scale(config.window_scale + 1);
}

void Graphics::decrease_scale() {
	set_scale(config.window_scale - 1 < 0 ? 0 : config.window_scale - 1);
}

void Graphics::set_scale(int new_scale) {
	config.window_scale = new_scale;
	SDL_SetWindowSize(window, dim.w * config.window_scale, dim.h * config.window_scale);
}

void Graphics::clear() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
	SDL_RenderClear(renderer);
}

void Graphics::present() {
	SDL_RenderPresent(renderer);
}
