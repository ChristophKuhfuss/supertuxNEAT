#ifndef SENSOR_MANAGER
#define SENSOR_MANAGER

#include "multineat/sensors/sensor.hpp"
#include "supertux/game_session.hpp"

#include <vector>

class GameSession;

class SensorManager {
  friend class ExperimentParameterParser;
  
private:
  static int TILE_WIDTH;
  
  static int AMOUNT_RANGE_SENSORS;
  
  static int AMOUNT_DEPTH_SENSORS;
  static int SPACING_DEPTH_SENSORS;
  
  // This should be an odd number so we get a center/front pie slice
  static int AMOUNT_PIESLICE_SENSORS;
public:
  SensorManager(GameSession* session);
  ~SensorManager();
  
  void initSensors();
  void clearSensors();
  
  std::vector<std::shared_ptr<Sensor>>* get_cur_sensors();
  
  static int get_total_sensor_count();
protected:
  GameSession* session;
  Sector* sec;
  Player* tux;
  
public:
  std::vector<std::shared_ptr<Sensor>>* cur_sensors;
  void create_rangefinder_sensors();
  void create_depthfinder_sensors();
  void create_pieslice_sensors();
};

#endif /* SENSOR_MANAGER */
/* EOF */