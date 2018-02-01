#ifndef HEADER_SUPERTUX_VIDEO_DUMMY_RENDERER_HPP
#define HEADER_SUPERTUX_VIDEO_DUMMY_RENDERER_HPP

#include "SDL.h"
#include "video/renderer.hpp"
#include <vector>

class DummyRenderer : public Renderer
{
public:
  DummyRenderer();
  ~DummyRenderer();

  void start_draw() override { };
  void end_draw() override { };
  void draw_surface(const DrawingRequest& request) { };
  void draw_surface_part(const DrawingRequest& request) { };
  void draw_gradient(const DrawingRequest& request) { };
  void draw_filled_rect(const DrawingRequest& request) { };
  void draw_inverse_ellipse(const DrawingRequest& request) { };
  void draw_line(const DrawingRequest& request) { };
  void draw_triangle(const DrawingRequest& request) { };
  void do_take_screenshot() { };
  void flip() { };
  void resize(int w, int h) { };
  void apply_config() { };
  Vector to_logical(int physical_x, int physical_y) const { return Vector(0, 0); };
  void set_gamma(float gamma) { };
  SDL_Window* get_window() const { };
};

#endif