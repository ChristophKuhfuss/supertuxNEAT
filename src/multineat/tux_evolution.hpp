// Main evolution class for the NEAT algorithm
// Copyright (C) 2017 Christoph Kuhfuss

#ifndef HEADER_TUX_EVOLUTION
#define HEADER_TUX_EVOLUTION

#include "supertux/globals.hpp"
#include "supertux/game_session.hpp"
#include "supertux/savegame.hpp"
#include "util/file_system.hpp"
#include "supertux/gameconfig.hpp"

#include "multineat/Population.h"
#include "multineat/Genome.h"
#include "multineat/NeuralNetwork.h"
#include "multineat/sensors/sensormanager.hpp"
#include "multineat/Substrate.h"
#include <vector>
#include <thread>
#include <physfs.h>
#include <cxxabi.h>
#include <execinfo.h>

using namespace NEAT;
using namespace std;

class SensorManager;

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

struct OutputQuotas {
  double qDown;
  double qUp;
  double qLeft;
  double qRight;
  double qJump;
  double qAction;
};

class TuxEvolution {
  friend class ExperimentParameterParser;
  
public:
  static const bool debug = false;
  
  static int from_genome_id;
  static int to_genome_id;
      
  static int max_gens;

  static int sensor_grid_size;
  static int sensor_grid_padding;
  static bool custom_sensor_grid;
  
  static int num_hidden_start_neurons;
  static int num_hidden_start_neurons_cppn;
  
  static bool using_seed;
  static int seed;
  
  static const char* filename;
  static const char* paramfilename;
  
  static int autosave_interval;
  
  static bool viewing_mode;
  static int view_genome_id;
  
  static string dbfile;
  
  static int current_gen;
  
  static bool hyperneat;

private:
  Parameters params;
  Genome start_genome;
  Population pop;
  double top_fitness;
  int top_fitness_genome_id;
  int gens;
  vector<Genome*> remaining_genomes;
  Genome* cur_genome;
  NeuralNetwork cur_network;
  NeatInputs cur_inputs;
  NeatOutputs cur_outputs;
  vector<Genome*>::iterator it;
  
  Substrate substrate;
  
  static int max_db_retry;
  static int db_sleeptime;
  
  SensorManager* sm;

public:
  TuxEvolution();
  ~TuxEvolution();
    
  bool on_tux_death(float progress, OutputQuotas q);
  void accept_inputs(NeatInputs inputs);
  NeatOutputs get_outputs();
  
  double get_top_fitness();
  int get_generation_number();
  int get_current_genome_id();
  
  void on_level_won();
  
  void generate_substrate(SensorManager* sm);
  
private:
  static Parameters init_params();
  bool tux_epoch();
  double tux_evaluate(float progress);
  void propagate_inputs();
  bool advance_genome();
  void refresh_genome_list();
  void get_genome_from_iterator();
  void print_all_genomes();
  void autosave();
  void save_pop();
  void set_viewing_genome();
  
  void update_db(int genome_id, float fitness, OutputQuotas q);
  static int busy_handler(void* data, int retry);
};

#endif /* HEADER_TUX_EVOLUTION */

/* EOF */