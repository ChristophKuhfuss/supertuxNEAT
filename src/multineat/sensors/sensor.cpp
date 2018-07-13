#include "sensor.hpp"
#include <object/brick.hpp>

Sensor::Sensor(Sector* sec, int offsetX, int offsetY) :
sec(sec),
tux(sec->player),
sprite(/*SpriteManager::current()->create("images/objects/sensor/sensor.png")*/),
offset(Vector(offsetX, offsetY)),
value(0)
{
}

void Sensor::update(float elapsed_time) {
  Rectf lookahead = tux->get_bbox();
  lookahead.p2.x += offset.x - 10;
  lookahead.p2.y += offset.y - 10;
  lookahead.p1.x = lookahead.p2.x - 1;
  lookahead.p1.y = lookahead.p2.y - 1;
  
  if (!sec->is_free_of_statics(lookahead)) { 			// Static obstacle
    value = 1;
    sprite->set_color(Color(0,0.8,0.2));
  } else if (!sec->is_free_of_enemies(lookahead, tux)) {	// Second parameter is for ignoring tux
    value = -1;
    sprite->set_color(Color(0.8,0.2,0.2));
  } else {							// Else: nothing we currently care for
    value = 0;
    sprite->set_color(Color(0,0,0));
  }
}

void Sensor::draw(DrawingContext& context) {
//   sprite->draw(context, Vector(tux->get_bbox().p2.x + offset.x - 10, tux->get_bbox().p2.y + offset.y - 10), 401);
}
