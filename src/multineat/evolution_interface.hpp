// Evolutionary interface to the SuperTux source code
// Updates the evolution once per frame
// Copyright (C) 2017 Christoph Kuhfuss

#ifndef HEADER_EVOLUTION_INTERFACE
#define HEADER_EVOLUTION_INTERFACE

#include "tux_evolution.hpp"
#include "worldmap/worldmap.hpp"
#include "supertux/game_object.hpp"
#include "worldmap/tux.hpp"

using namespace NEAT;
using namespace worldmap;



class EvolutionInterface 
{
private:
  WorldMap* worldMap;
  std::shared_ptr<Tux> tux;
  double** sensorValues;
  NeatOutputs* outputs;
  TuxEvolution* neat;
public:
  EvolutionInterface();
  EvolutionInterface(char* paramfile);
  ~EvolutionInterface();
  void update(float elapsed_time);
  void draw(DrawingContext& context);
private:
  void send_inputs(NeatInputs* inputs);
  NeatInputs* generate_inputs();
  void send_outputs();
  void check_tux_death();
};

#endif /* HEADER_EVOLUTION_INTERFACE */
/* EOF */