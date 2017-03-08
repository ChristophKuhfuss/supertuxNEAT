// Evolutionary interface to the SuperTux source code
// Updates the evolution once per frame
// Copyright (C) 2017 Christoph Kuhfuss

#ifndef HEADER_EVOLUTION_INTERFACE
#define HEADER_EVOLUTION_INTERFACE

#include "TuxEvolution.hpp"
#include "worldmap/worldmap.hpp"
#include "supertux/game_object.hpp"

class EvolutionInterface : GameObject {
private:
  WorldMap* worldMap;
  std::shared_ptr<Tux> tux;
  double** sensorValues;
  NeatOutputs* outputs;
  TuxEvolution* neat;
public:
  EvolutionInterface();
  ~EvolutionInterface();
  void update(float elapsed_time);
  void draw(DrawingContext& context);
private:
  void send_inputs(NeatInputs* inputs);
  NeatInputs* generate_inputs();
  void send_outputs();
  void check_tux_death();
}

#endif /* HEADER_EVOLUTION_INTERFACE */
/* EOF */