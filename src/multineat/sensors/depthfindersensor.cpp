#include "multineat/sensors/depthfindersensor.hpp"
#include "video/color.hpp"
#include "math/rectf.hpp"

int DepthFinderSensor::length = 512;

DepthFinderSensor::DepthFinderSensor(Sector* sec, int offsetX) : Sensor(sec, offsetX, 0)
{
}

DepthFinderSensor::DepthFinderSensor(int offsetX) : Sensor(offsetX, 0)
{
}

void DepthFinderSensor::update(float elapsed_time)
{
  int posX = tux->get_bbox().get_middle().x + offset.x;
  int posY = tux->get_bbox().get_middle().y;
  
  bool collided = false;
  
  for (int i = posY; i <= posY + length; i += 32) {
    Rectf point;
    point.p1.x = posX;
    point.p1.y = i;
    point.p2.x = posX + 1;
    point.p2.y = i + 1;
      
    // Checking for tiles only doesn't do the job because bonus blocks are static "moving" objects
    // Hurting collisions are a seperate case because spikes, lava etc don't count as solid tiles
    if (!sec->is_free_of_statics(point)) {
      collided = true;
      break;
    } else if (!sec->is_free_of_hurting_tiles(point)) {
      break;
    }
  }
    
  if (collided) { 		// There is safe ground below Tux
    value = 0;
  } else {			// Danger!
    value = 1;
  }
}

void DepthFinderSensor::draw(DrawingContext& context)
{
  if (DRAW_SENSORS) {
    int posX = tux->get_bbox().get_middle().x + offset.x;
    int posY = tux->get_bbox().get_middle().y + offset.y;
    context.draw_line(Vector(posX, posY), Vector(posX, posY + length), Color(0, 0, 1.0 * value), 401);
  }
}
