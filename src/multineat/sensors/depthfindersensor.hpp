#ifndef HEADER_DEPTHFINDERSENSOR
#define HEADER_DEPTHFINDERSENSOR

#include "multineat/sensors/sensor.hpp"
#include "supertux/sector.hpp"

class DepthFinderSensor : public Sensor {
  friend class ExperimentParameterParser;
  
protected:
  static int length;
public:
  DepthFinderSensor(Sector* sec, int offsetX);
  DepthFinderSensor(int offsetX);
  
  virtual void update(float elapsed_time) override;
  virtual void draw(DrawingContext& context) override;
};

#endif /* HEADER_DEPTHFINDERSENSOR */
/* EOF */