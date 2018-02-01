#ifndef HEADER_SUPERTUX_VIDEO_DUMMY_SURFACE_DATA_HPP
#define HEADER_SUPERTUX_VIDEO_DUMMY_SURFACE_DATA_HPP

#include <config.h>

#include "video/surface.hpp"
#include "video/surface_data.hpp"

class DummySurfaceData : public SurfaceData
{
private:

public:
  DummySurfaceData(const Surface &surface) { }
};

#endif