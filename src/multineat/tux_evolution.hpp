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
  vector<double> sensors;
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
public:
  static const int SENSOR_GRID_SIZE = 20;
  static const int SENSOR_GRID_PADDING = 23;
  static const bool debug = false;

private:
  Parameters params;
  Genome start_genome;
  Population pop;
  double top_fitness;
  int top_fitness_genome_id;
  int gens;
  int max_gens;
  vector<Genome*> remaining_genomes;
  Genome* cur_genome;
  NeuralNetwork cur_network;
  NeatInputs cur_inputs;
  NeatOutputs cur_outputs;
  vector<Genome*>::iterator it;

public:
  TuxEvolution();
  ~TuxEvolution();
  bool on_tux_death(float progress, int coins);
  void accept_inputs(NeatInputs inputs);
  NeatOutputs get_outputs();
  
  double get_top_fitness();
  int get_generation_number();
  
private:
  static Parameters init_params();
  bool tux_epoch();
  double tux_evaluate(float progress, int coins);
  void propagate_inputs();
  bool advance_genome();
  void refresh_genome_list();
  void get_genome_from_iterator();
  void print_all_genomes();
};

#endif /* HEADER_TUX_EVOLUTION */

/* EOF */