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
  static const int SENSOR_GRID_SIZE = 15;
  static const int SENSOR_GRID_PADDING = 26;
  static const bool debug = false;
  
  static bool using_seed;
  static int seed;
  
  static const char* filename;
  static const char* paramfilename;
  
  static int autosave;
  
  static bool viewing_mode;
  static int view_genome_id;

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
  int get_current_genome_id();
  
private:
  static Parameters init_params();
  bool tux_epoch();
  double tux_evaluate(float progress, int coins);
  void propagate_inputs();
  bool advance_genome();
  void refresh_genome_list();
  void get_genome_from_iterator();
  void print_all_genomes();
  void autosave_pop();
  void set_viewing_genome();
};

#endif /* HEADER_TUX_EVOLUTION */

/* EOF */