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

#include <vector>

class Sensor : public GameObject {
  friend class ExperimentParameterParser;
  
public:
  Sensor(Sector* sec, int offsetX, int offsetY);
  
protected:
  Sector* sec;
  Player* tux;
  SpritePtr sprite;
  Vector offset;
  double value;
  
public:
  virtual void update(float elapsed_time);
  virtual void draw(DrawingContext& context);
  double getValue() { return value; };
  void save(Writer& writer) { GameObject::save(writer); };
  ObjectSettings get_settings() { return GameObject::get_settings(); };
};

#endif /* HEADER_SENSOR */
/* EOF */