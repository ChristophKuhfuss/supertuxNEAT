#include "sensor.hpp"

Sensor::Sensor(Player* tux, int offsetX, int offsetY) {
  this->Tux = tux;
  this->offsetX = offsetX;
  this->offsetY = offsetY;
  this->value = 0;
}

void Sensor::update(float elapsed_time) {
  //TODO Update position 
}

void Sensor::draw(DrawingContext& context) {
  //TODO Draw something so we can actually see the sensors
}

HitResponse Sensor::collision(GameObject& other, const CollisionHit& hit) {
  return ABORT_MOVE; 
}

double Sensor::getValue() {
  return value; 
}