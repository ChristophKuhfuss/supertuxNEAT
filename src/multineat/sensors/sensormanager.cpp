#include "multineat/sensors/sensormanager.hpp"

#include "multineat/sensors/rangefindersensor.hpp"
#include "multineat/sensors/depthfindersensor.hpp"
#include "multineat/sensors/pieslicesensor.hpp"

#include <math.h>

int SensorManager::TILE_WIDTH = 32;

int SensorManager::AMOUNT_RANGE_SENSORS = 5;

int SensorManager::AMOUNT_DEPTH_SENSORS = 7;
int SensorManager::SPACING_DEPTH_SENSORS = 64;

int SensorManager::AMOUNT_PIESLICE_SENSORS = 5;


SensorManager::SensorManager(GameSession* session) : session(session), cur_sensors(new std::vector<std::shared_ptr<Sensor>>)
{
}

SensorManager::~SensorManager()
{
  
}

void SensorManager::initSensors()
{
  sec = session->get_current_sector();
  tux = sec->player;
  
  create_rangefinder_sensors();
  create_depthfinder_sensors();
  create_pieslice_sensors();
}

void SensorManager::clearSensors()
{
  cur_sensors->clear();
}

vector<shared_ptr<Sensor>>* SensorManager::get_cur_sensors()
{
  return cur_sensors;
}


int SensorManager::get_total_sensor_count()
{
  return AMOUNT_RANGE_SENSORS + AMOUNT_DEPTH_SENSORS + AMOUNT_PIESLICE_SENSORS;
}


void SensorManager::create_rangefinder_sensors()
{
  int spacing = 0;
  
  for (int i = 0; i < AMOUNT_RANGE_SENSORS; i++) {
    std::shared_ptr<RangeFinderSensor> sensor = std::make_shared<RangeFinderSensor>(sec, spacing);
    sec->add_object(sensor);
    cur_sensors->push_back(sensor);
    spacing -= TILE_WIDTH;
  }
}

void SensorManager::create_depthfinder_sensors()
{
  int spacing = 0;
  
  for (int i = 0; i < AMOUNT_DEPTH_SENSORS; i++) {
    std::shared_ptr<DepthFinderSensor> sensor = std::make_shared<DepthFinderSensor>(sec, spacing);
    sec->add_object(sensor);
    cur_sensors->push_back(sensor);
    spacing += SPACING_DEPTH_SENSORS;
  }
}

void SensorManager::create_pieslice_sensors()
{
  double cur_angle = - M_PI / 2;
  double angle_step = M_PI / AMOUNT_PIESLICE_SENSORS;
  
  for (int i = 0; i < AMOUNT_PIESLICE_SENSORS; i++) {
    std::shared_ptr<PieSliceSensor> sensor = std::make_shared<PieSliceSensor>(sec, cur_angle, cur_angle + angle_step);
    sec->add_object(sensor);
    cur_sensors->push_back(sensor);
    cur_angle += angle_step;
  }
}
