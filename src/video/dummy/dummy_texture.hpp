#ifndef HEADER_SUPERTUX_DUMMY_TEXTURE_HPP
#define HEADER_SUPERTUX_DUMMY_TEXTURE_HPP

#include "video/texture.hpp"

#include <SDL.h>

class DummyTexture : public Texture {
public:
  DummyTexture() {};
  DummyTexture(SDL_Surface *image) : w(image->w), h(image->h) {};
  
  ~DummyTexture() {};

  unsigned int get_texture_width() const override { return w; };
  unsigned int get_texture_height() const override { return h; };
  unsigned int get_image_width() const override { return w; };
  unsigned int get_image_height() const override { return h; };
  
private:
  unsigned int w;
  unsigned int h;
};

#endif