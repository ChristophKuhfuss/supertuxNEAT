#ifndef HEADER_SENSOR
#define HEADER_SENSOR

#include "object/player.hpp"
#include "supertux/moving_object.hpp"
#include "supertux/collision_hit.hpp"

class Sensor : public MovingObject {
public:
  Sensor(Player* Tux, int offsetX, int offsetY);
  
private:
  Player* Tux;
  int offsetX;
  int offsetY;
  double value;
  
public:
  void update(float elapsed_time);
  void draw(DrawingContext& context);
  HitResponse collision(GameObject& other, const CollisionHit& hit);
  double getValue();
}

#endif /* HEADER_SENSOR */
/* EOF */