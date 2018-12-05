#ifndef HEADER_PIESLICESENSORSPECIAL
#define HEADER_PIESLICESENSORSPECIAL

#include "multineat/sensors/pieslicesensor.hpp"

// Second PieSliceSensor class to detect "special" enemies that require different behavior than usual
// For example jumping spikeys, can't be killed, shouldn't be jumped over
// Simply exist so the networks can react differently to these enemies when encountered
class PieSliceSensorSpecial : public PieSliceSensor {
 friend class ExperimentParameterParser;

protected:
  static int radius_special;
  
public:
  PieSliceSensorSpecial(Sector* sec, double angle1, double angle2) : PieSliceSensor(sec, angle1, angle2) 
  {
    offset = rotate_point(Vector(radius_special, 0), angle1);
    offset2 = rotate_point(Vector(radius_special, 0), angle2);
  }
  
  PieSliceSensorSpecial(double angle1, double angle2) : PieSliceSensor(angle1, angle2) 
  {
    offset = rotate_point(Vector(radius_special, 0), angle1);
    offset2 = rotate_point(Vector(radius_special, 0), angle2);
  }
  
  virtual void update(float elapsed_time) override;
  virtual void draw(DrawingContext& context) override;
};

#endif /* HEADER_PIESLICESENSORSPECIAL */
/* EOF */