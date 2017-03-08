// Main evolution class for the NEAT algorithm
// Copyright (C) 2017 Christoph Kuhfuss

#ifndef HEADER_TUX_EVOLUTION
#define HEADER_TUX_EVOLUTION

#include "population.h"
#include "genome.h"

using namespace NEAT;

struct NeatInputs  {
  double[][] sensors;
}

struct NeatOutputs {
    double direction_up;
    double direction_down;
    double direction_left;
    double direction_right;
    double jump;
    double action;
}

class TuxEvolution {
private:
  Population* pop;
  double top_fitness;
  int gens;
  int num_gens;
  Genome* cur_genome;
  
public:
  TuxEvolution();
  ~TuxEvolution();
  void tux_epoch();
  void on_tux_death(double progress, double score);
  void accept_inputs(NeatInputs inputs);
  NeatOutputs get_outputs();
  
  double get_top_fitness();
  int get_generation_number();
  
private:
  void tux_evaluate();
  void propagate_inputs(NeatInputs inputs);
}

#endif /* HEADER_TUX_EVOLUTION */

/* EOF */