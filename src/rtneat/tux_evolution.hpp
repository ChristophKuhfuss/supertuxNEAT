// Main evolution class for the NEAT algorithm
// Copyright (C) 2017 Christoph Kuhfuss

#ifndef HEADER_TUX_EVOLUTION
#define HEADER_TUX_EVOLUTION

#include "population.h"
#include "organism.h"
#include <vector>

using namespace NEAT;
using namespace std;

struct NeatInputs  {
  double sensors[10][10];
};

struct NeatOutputs {
    double direction_up;
    double direction_down;
    double direction_left;
    double direction_right;
    double jump;
    double action;
};

class TuxEvolution {
private:
  Population* pop;
  double top_fitness;
  int gens;
  int num_gens;
  vector<Organism*> remaining_organisms;
  Organism* cur_org;
  NeatInputs cur_inputs;
  NeatOutputs cur_outputs;
public:
  TuxEvolution();
  TuxEvolution(char* paramfile);
  ~TuxEvolution();
  void on_tux_death(double progress, double score);
  void accept_inputs();
  NeatOutputs* get_outputs();
  
  double get_top_fitness();
  int get_generation_number();
  
private:
  vector<Organism*>::iterator it;
  bool tux_epoch();
  double tux_evaluate(double progress, double score);
  void propagate_inputs(NeatInputs inputs);
};

#endif /* HEADER_TUX_EVOLUTION */

/* EOF */