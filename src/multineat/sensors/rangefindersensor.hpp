#ifndef HEADER_RANGEFINDERSENSOR
#define HEADER_RANGEFINDERSENSOR

#include "multineat/sensors/sensor.hpp"
#include "supertux/sector.hpp"

class RangeFinderSensor : public Sensor {
  friend class ExperimentParameterParser;
  
protected:
  static int length;
public:
  RangeFinderSensor(Sector* sec, int offsetY);
  
  virtual void update(float elapsed_time) override;
  virtual void draw(DrawingContext& context) override;
};

#endif /* HEADER_RANGEFINDERSENSOR */
/* EOF */