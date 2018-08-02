#include "multineat/sensors/sensormanager.hpp"

#include <math.h>

int SensorManager::TILE_WIDTH = 32;

int SensorManager::AMOUNT_RANGE_SENSORS = 5;

int SensorManager::AMOUNT_DEPTH_SENSORS = 7;
int SensorManager::SPACING_DEPTH_SENSORS = 64;

int SensorManager::AMOUNT_PIESLICE_SENSORS = 5;

SensorManager* SensorManager::instance;


SensorManager::SensorManager(GameSession* session) : session(session), cur_sensors(new std::vector<std::shared_ptr<Sensor>>), 
  cur_rangefinder_sensors(new std::vector<std::shared_ptr<RangeFinderSensor>>),
  cur_depthfinder_sensors(new std::vector<std::shared_ptr<DepthFinderSensor>>),
  cur_pieslice_sensors(new std::vector<std::shared_ptr<PieSliceSensor>>)
{
  instance = this;
  
  if (TuxEvolution::hyperneat)
    initSensors(false);
}

SensorManager::~SensorManager()
{
  
}

void SensorManager::initSensors(bool add_to_sector)
{
  if (add_to_sector) {
    sec = session->get_current_sector();
    tux = sec->player;
  }
  cur_sensors->clear();
  
  create_rangefinder_sensors(add_to_sector);
  create_depthfinder_sensors(add_to_sector);
  create_pieslice_sensors(add_to_sector);
}

void SensorManager::clearSensors()
{
  cur_sensors->clear();
  cur_rangefinder_sensors->clear();
  cur_depthfinder_sensors->clear();
  cur_pieslice_sensors->clear();
}

vector<shared_ptr<Sensor>>* SensorManager::get_cur_sensors()
{
  return cur_sensors;
}


int SensorManager::get_total_sensor_count()
{
  return AMOUNT_RANGE_SENSORS + AMOUNT_DEPTH_SENSORS + AMOUNT_PIESLICE_SENSORS;
}


void SensorManager::create_rangefinder_sensors(bool add_to_sector)
{
  int spacing = 0;
  
  for (int i = 0; i < AMOUNT_RANGE_SENSORS; i++) {
    std::shared_ptr<RangeFinderSensor> sensor = std::make_shared<RangeFinderSensor>(sec, spacing);
    if (add_to_sector) sec->add_object(sensor);
    cur_sensors->push_back(sensor);
    cur_rangefinder_sensors->push_back(sensor);
    spacing -= TILE_WIDTH;
  }
}

void SensorManager::create_depthfinder_sensors(bool add_to_sector)
{
  int spacing = 0;
  
  for (int i = 0; i < AMOUNT_DEPTH_SENSORS; i++) {
    std::shared_ptr<DepthFinderSensor> sensor = std::make_shared<DepthFinderSensor>(sec, spacing);
    if (add_to_sector) sec->add_object(sensor);
    cur_sensors->push_back(sensor);
    cur_depthfinder_sensors->push_back(sensor);
    spacing += SPACING_DEPTH_SENSORS;
  }
}

void SensorManager::create_pieslice_sensors(bool add_to_sector)
{
  double cur_angle = - M_PI / 2;
  double angle_step = M_PI / AMOUNT_PIESLICE_SENSORS;
  
  for (int i = 0; i < AMOUNT_PIESLICE_SENSORS; i++) {
    std::shared_ptr<PieSliceSensor> sensor = std::make_shared<PieSliceSensor>(sec, cur_angle, cur_angle + angle_step);
    if (add_to_sector) sec->add_object(sensor);
    cur_sensors->push_back(sensor);
    cur_pieslice_sensors->push_back(sensor);
    cur_angle += angle_step;
  }
}
