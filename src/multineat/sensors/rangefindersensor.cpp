#include "multineat/sensors/rangefindersensor.hpp"
#include "video/color.hpp"
#include "math/rectf.hpp"

int RangeFinderSensor::length = 256;

RangeFinderSensor::RangeFinderSensor(Sector* sec, int offsetY) : 
  Sensor(sec, 0, offsetY)
{
}

void RangeFinderSensor::update(float elapsed_time)
{
  value = 0;
  
  int posX = tux->get_bbox().get_middle().x;
  int posY = tux->get_bbox().get_middle().y + offset.y;
  
  for (int i = posX; i <= posX + length; i += 32) {
    Rectf point;
    point.p1.x = i;
    point.p1.y = posY;
    point.p2.x = i + 1;
    point.p2.y = posY + 1;
    
    if (!sec->is_free_of_statics(point)) {
      value = 1 - ((double) (point.p1.x - posX) / length);	// The closer the obstacle gets, the higher the input activation should be
      break;
    }
  }
  
//   if (value > 0)
//     std::cerr << "RangeFinderSensor value: " << value << std::endl;
}

void RangeFinderSensor::draw(DrawingContext& context)
{
  int posX = tux->get_bbox().get_middle().x;
  int posY = tux->get_bbox().get_middle().y + offset.y;
  
  int layer = 401;
  
  if (value > 0)
    layer++;
  
  context.draw_line(Vector(posX, posY), Vector(posX + length, posY), Color(0, 1.0 * value, 0), layer);
}
