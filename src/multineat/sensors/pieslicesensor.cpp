#include "multineat/sensors/pieslicesensor.hpp"

#include <math.h>

#include <iostream>

#include <vector>

int PieSliceSensor::radius = 256;

PieSliceSensor::PieSliceSensor(Sector* sec, double angle1, double angle2) : Sensor(sec, 0, 0),
angle1(angle1 < 0 ? angle1 + 2 * M_PI : angle1),
angle2(angle2 < 0 ? angle2 + 2 * M_PI : angle2)
{
  offset = rotate_point(Vector(radius, 0), angle1);
  offset2 = rotate_point(Vector(radius, 0), angle2);
}

PieSliceSensor::PieSliceSensor(double angle1, double angle2) : Sensor(0, 0), 
angle1(angle1 < 0 ? angle1 + 2 * M_PI : angle1),
angle2(angle2 < 0 ? angle2 + 2 * M_PI : angle2)
{
  offset = rotate_point(Vector(radius, 0), angle1);
  offset2 = rotate_point(Vector(radius, 0), angle2);
}

void PieSliceSensor::update(float elapsed_time) 
{
  value = 0;
  
  std::vector<BadGuy*> badguys = sec->get_nearby_enemies(tux->get_bbox().get_middle(), radius);
  
  for (BadGuy* bg : badguys) {
    double angle_e = get_rad(bg->get_pos() - tux->get_pos(), Vector(1, 0));
        
    // The second case is special because angle1 is only greater than angle2 if the sensor overlaps the x axis
    if (angle_e >= angle1 && angle_e <= angle2 || (angle1 > angle2 && (angle_e > angle1 || angle_e < angle2))) {
      value += 1 - (get_dist(bg->get_bbox().get_middle(), tux->get_bbox().get_middle()) / radius);
    }
  }
  
  if (value > 1) value = 1;
}

void PieSliceSensor::draw(DrawingContext& context)
{
  if (DRAW_SENSORS) {
    Vector pos1(tux->get_pos().x, tux->get_pos().y);
    Vector pos2(tux->get_pos().x + radius, tux->get_pos().y);
    
    Color color(std::min(1 * value, 1.0), 0, 0);
    
    int layer = 401;
    if (value > 0) layer++;
    
    context.draw_line(tux->get_bbox().get_middle(), tux->get_bbox().get_middle() + offset, color, layer);
    context.draw_line(tux->get_bbox().get_middle(), tux->get_bbox().get_middle() + offset2, color, layer);
    context.draw_line(tux->get_bbox().get_middle() + offset, tux->get_bbox().get_middle() + offset2, color, layer);
  }
}

Vector PieSliceSensor::rotate_point(const Vector& point, double rad)
{
  return Vector(point.x * cos(rad) - point.y * sin(rad), point.y * cos(rad) + point.x * sin(rad));
}

double PieSliceSensor::get_rad(const Vector& v1, const Vector& v2)
{
  double angle = atan2(v1.x * v2.y + v1.y * v2.x, v1.x * v2.x - v1.y * v2.y);
  
  if (angle < 0)
    angle += 2 * M_PI;
  
  return angle;
}

double PieSliceSensor::get_dist(const Vector& v1, const Vector& v2)
{
  return sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
}

