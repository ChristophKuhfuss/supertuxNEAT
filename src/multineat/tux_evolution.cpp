#include "tux_evolution.hpp"
#include <boost/concept_check.hpp>
#include "Genes.h"
#include "multineat/sensors/sensormanager.hpp"
#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <execinfo.h>

#include <sqlite3.h>

int TuxEvolution::from_genome_id = -1;
int TuxEvolution::to_genome_id = -1;

int TuxEvolution::max_gens = 100;

int TuxEvolution::sensor_grid_size = 15;
int TuxEvolution::sensor_grid_padding = 18;
bool TuxEvolution::custom_sensor_grid = false;

bool TuxEvolution::using_seed = false;
int TuxEvolution::seed;

const char* TuxEvolution::filename = "";
const char* TuxEvolution::paramfilename = "";
  
int TuxEvolution::autosave_interval;

bool TuxEvolution::viewing_mode;
int TuxEvolution::view_genome_id;

string TuxEvolution::dbfile = "";
int TuxEvolution::current_gen = 0;
int TuxEvolution::max_db_retry = 100;
int TuxEvolution::db_sleeptime = 50;

TuxEvolution::TuxEvolution() : params(init_params()),
  start_genome(0, SensorManager::get_total_sensor_count() + 1, 10, 
	       6, false, UNSIGNED_SIGMOID, UNSIGNED_SIGMOID, 1, params),
  pop(strcmp(filename, "") ? Population(filename) : Population(start_genome, params, true, 2.0, (using_seed ? seed : (int) time(0)))),
  top_fitness(0),
  top_fitness_genome_id(-1),
  cur_outputs(),
  cur_inputs()
{
  //print_stacktrace();
  pop.m_RNG.Seed(seed);
  
  if (!viewing_mode) {
    gens = 0;

    refresh_genome_list();
    it = remaining_genomes.begin();
    get_genome_from_iterator();
  } else {
    set_viewing_genome();
  }
}

TuxEvolution::~TuxEvolution()
{
}


void TuxEvolution::accept_inputs(NeatInputs inputs)
{
  this->cur_inputs = inputs;
}

// Clears the network and gives it the current inputs
void TuxEvolution::propagate_inputs()
{
  cur_network.Flush();
  vector<double> inputs = vector<double>(cur_inputs.sensors);
  inputs.push_back(1);
  cur_network.Input(inputs);
  
  // Activate depth times to ensure full input propagation
  for (unsigned int i = 0; i < cur_genome->GetDepth(); i++) {
    cur_network.Activate();
  }
}

NeatOutputs TuxEvolution::get_outputs()
{
  propagate_inputs();
  std::vector<double> net_outputs = cur_network.Output(); 
  cur_outputs.direction_up = net_outputs.at(0);
  cur_outputs.direction_down = net_outputs.at(1);
  cur_outputs.direction_left = net_outputs.at(2);
  cur_outputs.direction_right = net_outputs.at(3);
  cur_outputs.jump = net_outputs.at(4);
  cur_outputs.action = net_outputs.at(5);
  return cur_outputs;
}

// Advances the population to the next generation
// The whole update() system of SuperTux makes the whole structure of the EA a little more complicated
bool TuxEvolution::tux_epoch()
{
  std::cout << "Generation #" << (gens + 1) << " finished." << " Max fitness: " << top_fitness << std::endl;
  if (autosave_interval) {
    autosave();
  }
  gens++;
  if (gens < max_gens) {    
    pop.Epoch();
    refresh_genome_list();
    it = remaining_genomes.begin();
    get_genome_from_iterator();
    top_fitness = 0;
    std::cout << "Starting generation #" << (gens + 1) << " with genome #" << cur_genome->GetID() << "..." << std::endl;
    return true;
  } else {
    std::cout << "Finished evolution." << std::endl;
    return false;
  }
}

// Calculates fitness of current genome and marks the evaluated flag
// Returns the result of advance_genome(), which is false if the simulation finished
bool TuxEvolution::on_tux_death(float progress)
{
  double fitness = tux_evaluate(progress);
//   std::cout << "Organism #" << cur_genome->GetID() << " achieved a fitness of " << fitness << "." << std::endl;
  
  if (!viewing_mode) {
    //Only set fitness - adjfitness is set by species on pop.Epoch()
    cur_genome->SetFitness(fitness);
    return advance_genome();
  } else {
    cur_network.Flush();
    return true;
  }
}

double TuxEvolution::tux_evaluate(float progress)
{
  float fitness = progress;
  if (fitness > top_fitness) {
    top_fitness = fitness;
  }
  return fitness;
}

// Advances the current genome to the next one
// Returns false if all generations finished OR we're in parallel mode
bool TuxEvolution::advance_genome()
{
  ++it;
  if (it != remaining_genomes.end()) {
    get_genome_from_iterator();
    return true;
  } else if (from_genome_id == -1 && to_genome_id == -1) {
    return tux_epoch();
  } else {
    // We're done! Update db and let nature have its course
    update_db();
    return false;
  }
}

// Clear the remaining genome list and iterate over all species' genomes to add them again
// To be called after pop.Epoch()
void TuxEvolution::refresh_genome_list()
{
  remaining_genomes.clear();
  
  std::vector<Genome*> genomes_to_add;
  
  for (std::vector<Species>::iterator it2 = pop.m_Species.begin(); it2 != pop.m_Species.end(); ++it2) {
    Species* cur = &(*it2);
    
    for (std::vector<Genome>::iterator it3 = cur->m_Individuals.begin(); it3 != cur->m_Individuals.end(); ++it3) {
      genomes_to_add.push_back(&(*it3));
    }
  }
  
  
  // If from_genome_id and to_genome_id are set, we're in parallel mode!
  // Only add the genomes we need
  if (from_genome_id != -1 && to_genome_id != -1) {
    int i;
    for (i = from_genome_id; i <= to_genome_id; i++) {
      remaining_genomes.push_back(genomes_to_add[i]);
    }
  } else {
    remaining_genomes = genomes_to_add;
  }
}

// For debugging purposes
void TuxEvolution::print_all_genomes()
{
  for (unsigned int i = 0; i < pop.m_Species.size(); i++) { 
    Species s = pop.m_Species[i];
    for (unsigned int j = 0; j < s.m_Individuals.size(); j++) {
      std::cout << "Genome #" << s.m_Individuals[j].GetID() << ", Fitness: " << s.m_Individuals[j].GetFitness() << std::endl;
    }
  }
}


Parameters TuxEvolution::init_params()
{
  Parameters res;
  
  if (strcmp(paramfilename, "") != 0) { 
    res.Load(paramfilename);
  }
  
  return res;
}


double TuxEvolution::get_top_fitness()
{
  return top_fitness;
}

int TuxEvolution::get_generation_number()
{
  return gens;
}

// Takes the next genome from the iterator and builds the network accordingly
void TuxEvolution::get_genome_from_iterator()
{
  cur_genome = *it;
  cur_genome->BuildPhenotype(cur_network);
  cur_genome->CalculateDepth();
}

int TuxEvolution::get_current_genome_id()
{
  return cur_genome->GetID();
}

void TuxEvolution::autosave()
{
  if ((gens + 1) % autosave_interval == 0) {
    save_pop();
  }
}

void TuxEvolution::save_pop()
{
    std::ostringstream ss;
    ss << "./neat_gen" << (gens + 1);
    pop.Save(ss.str().c_str());
}


void TuxEvolution::set_viewing_genome()
{
  refresh_genome_list();
  
  it = remaining_genomes.begin();
  
  while (it != remaining_genomes.end() && (*it)->GetID() != view_genome_id) ++it;
  
  if (it == remaining_genomes.end()) {
    std::ostringstream ss;
    ss << "Couldn't find genome with ID " << view_genome_id;
    
    throw std::runtime_error(ss.str());
  } else {
    get_genome_from_iterator();
    std::cout << "Found genome. Starting playback..." << std::endl;
  }
}

// Just to make sure we don't lose winners
void TuxEvolution::on_level_won()
{
  save_pop();
}

// For each genome, update the row in the current db table with the right fitness value
void TuxEvolution::update_db()
{
  sqlite3* db;  
  
  sqlite3_open("neat.db", &db);
  sqlite3_busy_handler(db, busy_handler, (void*) nullptr);
  
  char* err;
  
  for (std::vector<Genome*>::iterator it = remaining_genomes.begin(); it != remaining_genomes.end(); ++it) {
    std::stringstream ss;
    
    ss << "UPDATE gen" << current_gen << " SET fitness = " << (*it)->GetFitness() << " WHERE id = " << (*it)->GetID() << ";";
        
    sqlite3_exec(db, ss.str().c_str(), 0, 0, &err);
  }
  
  sqlite3_close(db);
}

int TuxEvolution::busy_handler(void* data, int retry)
{
  if (retry < max_db_retry) {
    sqlite3_sleep(db_sleeptime);
    return 1;
  } else {
    return 0;
  }
}