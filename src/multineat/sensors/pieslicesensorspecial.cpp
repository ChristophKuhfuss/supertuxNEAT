#include "multineat/sensors/pieslicesensorspecial.hpp"

int PieSliceSensorSpecial::radius_special = 320;

void PieSliceSensorSpecial::update(float elapsed_time)
{
  value = 0;
  
  std::vector<BadGuy*> badguys = sec->get_nearby_enemies(tux->get_bbox().get_middle(), radius);
  
  for (BadGuy* bg : badguys) {
    if (bg->get_class() == "jumpy") {
      double angle_e = get_rad(bg->get_pos() - tux->get_pos(), Vector(1, 0));
	  
      // The second case is special because angle1 is only greater than angle2 if the sensor overlaps the x axis
      if (angle_e >= angle1 && angle_e <= angle2 || (angle1 > angle2 && (angle_e > angle1 || angle_e < angle2))) {
	value += 1 - (get_dist(bg->get_bbox().get_middle(), tux->get_bbox().get_middle()) / radius);
      }
    }
  }
  
  if (value > 1) value = 1;
}

void PieSliceSensorSpecial::draw(DrawingContext& context)
{
  if (DRAW_SENSORS) {
    Vector pos1(tux->get_pos().x, tux->get_pos().y);
    Vector pos2(tux->get_pos().x + radius, tux->get_pos().y);
    
    Color color(std::min(1 * value, 1.0), 0, std::min(1 * value, 1.0));
    
    int layer = 401;
    if (value > 0) layer++;
    
    context.draw_line(tux->get_bbox().get_middle(), tux->get_bbox().get_middle() + offset, color, layer);
    context.draw_line(tux->get_bbox().get_middle(), tux->get_bbox().get_middle() + offset2, color, layer);
    context.draw_line(tux->get_bbox().get_middle() + offset, tux->get_bbox().get_middle() + offset2, color, layer);
  }
}
