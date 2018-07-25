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
#include <vector>
#include <thread>
#include <physfs.h>
#include <cxxabi.h>
#include <execinfo.h>

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
  
  static bool using_seed;
  static int seed;
  
  static const char* filename;
  static const char* paramfilename;
  
  static int autosave_interval;
  
  static bool viewing_mode;
  static int view_genome_id;
  
  static string dbfile;
  
  static int current_gen;

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
  
  static int max_db_retry;
  static int db_sleeptime;

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

public:
  static inline void print_stacktrace(FILE *out = stderr, unsigned int max_frames = 63)
  {
    fprintf(out, "stack trace:\n");

    // storage array for stack trace address data
    void* addrlist[max_frames+1];

    // retrieve current stack addresses
    int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

    if (addrlen == 0) {
	fprintf(out, "  <empty, possibly corrupt>\n");
	return;
    }

    // resolve addresses into strings containing "filename(function+address)",
    // this array must be free()-ed
    char** symbollist = backtrace_symbols(addrlist, addrlen);

    // allocate string which will be filled with the demangled function name
    size_t funcnamesize = 256;
    char* funcname = (char*)malloc(funcnamesize);

    // iterate over the returned symbol lines. skip the first, it is the
    // address of this function.
    for (int i = 1; i < addrlen; i++)
    {
	char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

	// find parentheses and +address offset surrounding the mangled name:
	// ./module(function+0x15c) [0x8048a6d]
	for (char *p = symbollist[i]; *p; ++p)
	{
	    if (*p == '(')
		begin_name = p;
	    else if (*p == '+')
		begin_offset = p;
	    else if (*p == ')' && begin_offset) {
		end_offset = p;
		break;
	    }
	}

	if (begin_name && begin_offset && end_offset
	    && begin_name < begin_offset)
	{
	    *begin_name++ = '\0';
	    *begin_offset++ = '\0';
	    *end_offset = '\0';

	    // mangled name is now in [begin_name, begin_offset) and caller
	    // offset in [begin_offset, end_offset). now apply
	    // __cxa_demangle():

	    int status;
	    char* ret = abi::__cxa_demangle(begin_name,
					    funcname, &funcnamesize, &status);
	    if (status == 0) {
		funcname = ret; // use possibly realloc()-ed string
		fprintf(out, "  %s : %s+%s\n",
			symbollist[i], funcname, begin_offset);
	    }
	    else {
		// demangling failed. Output function name as a C function with
		// no arguments.
		fprintf(out, "  %s : %s()+%s\n",
			symbollist[i], begin_name, begin_offset);
	    }
	}
	else
	{
	    // couldn't parse the line? print the whole line.
	    fprintf(out, "  %s\n", symbollist[i]);
	}
    }

    free(funcname);
    free(symbollist);
  }
};

#endif /* HEADER_TUX_EVOLUTION */

/* EOF */