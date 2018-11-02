#ifndef SENSOR_MANAGER
#define SENSOR_MANAGER

#include "multineat/sensors/sensor.hpp"
#include "multineat/sensors/rangefindersensor.hpp"
#include "multineat/sensors/depthfindersensor.hpp"
#include "multineat/sensors/pieslicesensor.hpp"
#include "supertux/game_session.hpp"

#include <vector>

class GameSession;

class SensorManager {
  friend class ExperimentParameterParser; 	// Dirty Workaround, fix me (generate offsets in SensorManager and stop rotating in PieSliceSensor altogether)
  
private:
  static int TILE_WIDTH;
  
  static int AMOUNT_RANGE_SENSORS;
  
  static int AMOUNT_DEPTH_SENSORS;
  static int SPACING_DEPTH_SENSORS;
  
  // This should be an odd number so we get a center/front pie slice
  static int AMOUNT_PIESLICE_SENSORS;
  
  static bool DRAW_SENSORS;
public:
  SensorManager(GameSession* session);
  ~SensorManager();
  
  void initSensors(bool add_to_sector);
  void clearSensors();
  
  std::vector<std::shared_ptr<Sensor>>* get_cur_sensors();
  
  std::vector<std::shared_ptr<RangeFinderSensor>>* get_cur_rangefinder_sensors() { return cur_rangefinder_sensors; };
  std::vector<std::shared_ptr<DepthFinderSensor>>* get_cur_depthfinder_sensors() { return cur_depthfinder_sensors; };
  std::vector<std::shared_ptr<PieSliceSensor>>*	   get_cur_pieslice_sensors()    { return cur_pieslice_sensors; };
  
  static SensorManager* instance;
  
  static int get_total_sensor_count();
protected:
  GameSession* session;
  Sector* sec;
  Player* tux;
  
public:
  std::vector<std::shared_ptr<Sensor>>* cur_sensors;
  std::vector<std::shared_ptr<RangeFinderSensor>>* cur_rangefinder_sensors;
  std::vector<std::shared_ptr<DepthFinderSensor>>* cur_depthfinder_sensors;
  std::vector<std::shared_ptr<PieSliceSensor>>*	   cur_pieslice_sensors;
  void create_rangefinder_sensors(bool add_to_sector);
  void create_depthfinder_sensors(bool add_to_sector);
  void create_pieslice_sensors(bool add_to_sector);
};

#endif /* SENSOR_MANAGER */
/* EOF */