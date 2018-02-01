#ifndef HEADER_SUPERTUX_VIDEO_DUMMY_DUMMY_VIDEO_SYSTEM_HPP
#define HEADER_SUPERTUX_VIDEO_DUMMY_DUMMY_VIDEO_SYSTEM_HPP

#include <memory>
#include <SDL.h>

#include "video/video_system.hpp"
#include "video/texture_manager.hpp"
#include "video/dummy/dummy_texture.hpp"
#include "video/dummy/dummy_surface_data.hpp"


class DummyVideoSystem : public VideoSystem {
private:
  std::unique_ptr<TextureManager> m_texture_manager;
  std::unique_ptr<Renderer> m_renderer;
  std::unique_ptr<Lightmap> m_lightmap;

public:
  DummyVideoSystem();
  ~DummyVideoSystem();

  Renderer& get_renderer() const override { return *m_renderer; };
  Lightmap& get_lightmap() const override { return *m_lightmap; };
  TexturePtr new_texture(SDL_Surface *image) override { return TexturePtr(new DummyTexture(image)); };
  SurfaceData* new_surface_data(const Surface& surface) override { return new DummySurfaceData(surface); };
  void free_surface_data(SurfaceData* surface_data) override { delete surface_data; };

  void apply_config() override {};
  void resize(int w, int h) override {};
};

#endif

/* EOF */