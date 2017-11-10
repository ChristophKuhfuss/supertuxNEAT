#include "tux_evolution.hpp"
#include <boost/concept_check.hpp>
#include "Genes.h"
#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>

int TuxEvolution::max_gens = 100;

int TuxEvolution::sensor_grid_size = 15;
int TuxEvolution::sensor_grid_padding = 18;
bool TuxEvolution::custom_sensor_grid = false;

bool TuxEvolution::using_seed;
int TuxEvolution::seed;
  
const char* TuxEvolution::filename = "";
const char* TuxEvolution::paramfilename = "";
  
int TuxEvolution::autosave_interval;

bool TuxEvolution::viewing_mode;
int TuxEvolution::view_genome_id;

TuxEvolution::TuxEvolution() : params(init_params()),
  start_genome(0, sensor_grid_size * sensor_grid_size + 1, 5, 
	       6, false, UNSIGNED_SIGMOID, UNSIGNED_SIGMOID, 1, params),
  pop(strcmp(filename, "") ? Population(filename) : Population(start_genome, params, true, 2.0, (using_seed ? seed : (int) time(0)))),
  top_fitness(0),
  top_fitness_genome_id(-1),
  cur_outputs(),
  cur_inputs()
{
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
  //Flushing every time makes recurrent connections useless
  //cur_network.Flush();
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
  if (!viewing_mode) {
    //Only set fitness - adjfitness is set by species on pop.Epoch()
    cur_genome->SetFitness(tux_evaluate(progress));
    cur_genome->SetEvaluated();
    std::cout << "Organism #" << cur_genome->GetID() << " achieved a fitness of " << cur_genome->GetFitness() << "." << std::endl;;
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
// Returns false if all generations finished
bool TuxEvolution::advance_genome()
{
  ++it;
  if (it != remaining_genomes.end()) {
    get_genome_from_iterator();
    //std::cout << "Continuing evolution with genome #" << cur_genome->GetID() << "." << std::endl;
    return true;
  } else {
    //print_all_genomes();
    return tux_epoch();
  }
}

// Clear the remaining genome list and iterate over all species' genomes to add them again
// To be called after pop.Epoch()
void TuxEvolution::refresh_genome_list()
{
  remaining_genomes.clear();
  
  for (unsigned int i = 0; i < pop.m_Species.size(); i++) {
    Species* cur = &pop.m_Species[i];
    std::cout << "Species #" << cur->ID() << " has " << cur->m_Individuals.size() << " individuals" << std::endl;
    for (unsigned int j = 0; j < cur->m_Individuals.size(); j++) {
      remaining_genomes.push_back(&cur->m_Individuals[j]);
    }
  }
}

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

void TuxEvolution::on_level_won()
{
  save_pop();
}



