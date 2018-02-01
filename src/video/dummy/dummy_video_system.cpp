#include "video/dummy/dummy_video_system.hpp"

#include "video/dummy/dummy_renderer.hpp"
#include "video/dummy/dummy_lightmap.hpp"

DummyVideoSystem::DummyVideoSystem() : 
  m_texture_manager(new TextureManager),
  m_renderer(new DummyRenderer),
  m_lightmap(new DummyLightmap)
{
}

DummyVideoSystem::~DummyVideoSystem()
{

}


/* EOF */