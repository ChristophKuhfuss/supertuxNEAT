// Evolutionary interface to the SuperTux source code
// Updates the evolution once per frame
// Copyright (C) 2017 Christoph Kuhfuss

#ifndef HEADER_EVOLUTION_INTERFACE
#define HEADER_EVOLUTION_INTERFACE

#include "tux_evolution.hpp"
#include "sensor.hpp"
#include "supertux/game_object.hpp"
#include "supertux/sector.hpp"
#include "supertux/game_session.hpp"
#include "control/codecontroller.hpp"
#include "object/player.hpp"
#include "scripting/functions.hpp"

using namespace NEAT;

class GameSession;

class EvolutionInterface : public GameObject
{
public:
  static constexpr float TIMEOUT = 1.0f;
  static constexpr float FITNESS_TIMEOUT = 3.0f;
  static constexpr float SEND_THRESHOLD = 0.6;
private:
  GameSession* cur_session;
  Sector* cur_sector;
  Player* tux;
  std::unique_ptr<CodeController> controller;
  double* sensorValues;
  vector<std::shared_ptr<Sensor>> sensors;
  NeatOutputs outputs;
  TuxEvolution neat;
  Vector last_known_playerpos;
  float idle;
  float fitness_idle;
  float max_x;
public:
  EvolutionInterface(GameSession* session);
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
};

#endif /* HEADER_EVOLUTION_INTERFACE */
/* EOF */