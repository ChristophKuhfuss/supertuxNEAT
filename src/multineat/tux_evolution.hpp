// Main evolution class for the NEAT algorithm
// Copyright (C) 2017 Christoph Kuhfuss

#ifndef HEADER_TUX_EVOLUTION
#define HEADER_TUX_EVOLUTION

#include "multineat/Population.h"
#include "multineat/Genome.h"
#include "multineat/NeuralNetwork.h"
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
  vector<Genome*> remaining_genomes;
  Genome* cur_genome;
  NeuralNetwork* cur_network;
  NeatInputs cur_inputs;
  NeatOutputs cur_outputs;
  vector<Genome*>::iterator it;

public:
  TuxEvolution();
  TuxEvolution(char* paramfile);
  ~TuxEvolution();
  bool on_tux_death(double progress, double score);
  void accept_inputs();
  NeatOutputs* get_outputs();
  
  double get_top_fitness();
  int get_generation_number();
  
private:
  bool tux_epoch();
  double tux_evaluate(double progress, double score);
  void propagate_inputs(NeatInputs inputs);
  bool advance_genome();
  void refresh_genome_list();
};

#endif /* HEADER_TUX_EVOLUTION */

/* EOF */