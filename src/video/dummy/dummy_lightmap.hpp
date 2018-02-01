#ifndef HEADER_SUPERTUX_VIDEO_DUMMY_LIGHTMAP_HPP
#define HEADER_SUPERTUX_VIDEO_DUMMY_LIGHTMAP_HPP

#include "video/lightmap.hpp"

#include "SDL.h"

class Color;
struct DrawingRequest;

class DummyLightmap : public Lightmap
{
public:
  DummyLightmap();
  ~DummyLightmap();

  void start_draw(const Color &ambient_color) override {};
  void end_draw() override {};
  void do_draw() override {};
  void draw_surface(const DrawingRequest& request) override {};
  void draw_surface_part(const DrawingRequest& request) override {};
  void draw_gradient(const DrawingRequest& request) override {};
  void draw_filled_rect(const DrawingRequest& request) override {};
  void draw_inverse_ellipse(const DrawingRequest& request) override {};
  void draw_line(const DrawingRequest& request) override {};
  void draw_triangle(const DrawingRequest& request) override {};
  void get_light(const DrawingRequest& request) const override {};
};

#endif