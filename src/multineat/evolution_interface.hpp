// Evolutionary interface to the SuperTux source code
// Updates the evolution once per frame
// Copyright (C) 2017 Christoph Kuhfuss

#ifndef HEADER_EVOLUTION_INTERFACE
#define HEADER_EVOLUTION_INTERFACE

#include "tux_evolution.hpp"
#include "sensors/sensor.hpp"
#include "sensors/sensormanager.hpp"
#include "supertux/game_object.hpp"
#include "supertux/sector.hpp"
#include "supertux/game_session.hpp"
#include "control/codecontroller.hpp"
#include "object/player.hpp"
#include "scripting/functions.hpp"

using namespace NEAT;

class GameSession;
class SensorManager;

class EvolutionInterface : public GameObject
{
  friend class ExperimentParameterParser;
  friend class CommandLineArguments;
  
private:
  static float TIMEOUT;
  static float FITNESS_TIMEOUT;
  static float SEND_THRESHOLD;
  
private:
  GameSession* cur_session;
  Sector* cur_sector;
  Player* tux;
  std::shared_ptr<SensorManager> sensor_manager;
  std::unique_ptr<CodeController> controller;
  double* sensorValues;
  std::vector<std::shared_ptr<Sensor>>* sensors;
  NeatOutputs outputs;
  TuxEvolution neat;
  Vector last_known_playerpos;
  float idle;
  float fitness_idle;
  float max_x;
  
  int ticks_total;
  int ticks_down;
  int ticks_up;
  int ticks_left;
  int ticks_right;
  int ticks_jump;
  int ticks_action;
  
public:
  EvolutionInterface(GameSession* session, std::shared_ptr<SensorManager> sensor_manager);
  ~EvolutionInterface();
  void update(float elapsed_time);
  void draw(DrawingContext& context);
  void on_tux_death();
  void on_level_won();
  void save(Writer& writer);
  ObjectSettings get_settings();
  void add_sensor(std::shared_ptr<Sensor> s);
  void init();
  
private:
  void send_inputs(NeatInputs inputs);
  NeatInputs generate_inputs();
  void send_outputs();
  void update_idle(float elapsed_time);
  void timeout();
  void debug_print();
};

#endif /* HEADER_EVOLUTION_INTERFACE */
/* EOF */