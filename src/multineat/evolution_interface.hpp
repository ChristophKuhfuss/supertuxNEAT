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

using namespace NEAT;


class EvolutionInterface 
{
public:
  static constexpr float TIMEOUT = 2000;
  static constexpr float SEND_THRESHOLD = 0.9;
private:
  GameSession* cur_session;
  Sector* cur_sector;
  Player* tux;
  CodeController* controller;
  double* sensorValues;
  vector<Sensor> sensors;
  NeatOutputs* outputs;
  TuxEvolution* neat;
  const Vector* last_known_playerpos;
  float idle;
public:
  EvolutionInterface(GameSession* session);
  ~EvolutionInterface();
  void update(float elapsed_time);
  void draw(DrawingContext& context);
private:
  void send_inputs(NeatInputs* inputs);
  NeatInputs* generate_inputs();
  void send_outputs();
  bool check_tux_death();
  void update_idle(float elapsed_time);
  void timeout();
};

#endif /* HEADER_EVOLUTION_INTERFACE */
/* EOF */