#ifndef HEADER_SENSOR
#define HEADER_SENSOR

#include "badguy/badguy.hpp"
#include "supertux/sector.hpp"
#include "sprite/sprite.hpp"
#include "sprite/sprite_manager.hpp"
#include "sprite/sprite_ptr.hpp"
#include "math/rectf.hpp"
#include "object/player.hpp"
#include "supertux/collision_hit.hpp"
#include "supertux/game_object.hpp"

class Sensor : public GameObject {
public:
  Sensor(Sector* sec, int offsetX, int offsetY);
  
private:
  Sector* sec;
  Player* tux;
  SpritePtr sprite;
  int offsetX;
  int offsetY;
  double value;
  
public:
  void update(float elapsed_time);
  void draw(DrawingContext& context);
  double getValue();
  void save(Writer& writer);
  ObjectSettings get_settings();
};

#endif /* HEADER_SENSOR */
/* EOF */