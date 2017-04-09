#include "tux_evolution.hpp"
#include <boost/concept_check.hpp>
#include "Genes.h"
#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>


TuxEvolution::TuxEvolution() : params(init_params()),
  start_genome(0, SENSOR_GRID_SIZE * SENSOR_GRID_SIZE, 0, 
	       6, false, UNSIGNED_SIGMOID, UNSIGNED_SIGMOID, 0, params),
  pop(start_genome, params, true, 1.0, (int) time(0)),
  top_fitness(0),
  cur_outputs(),
  cur_inputs()
{
  gens = 0;
  max_gens = 10;
  refresh_genome_list();
  it = remaining_genomes.begin();
  if (debug) {
    std::cout << "TuxEvolution object created" << std::endl;
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
  cur_network.Input(cur_inputs.sensors);
  
  // Activate depth times to ensure full input propagation
  for (unsigned int i = 0; i < cur_genome.GetDepth(); i++) {
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
  std::cout << "Gen #" << gens << " finished." << std::endl;
  if (gens < max_gens) {
    pop.Epoch();
    refresh_genome_list();
    it = remaining_genomes.begin();
    cur_genome = *it;
    gens++;
    std::cout << "Starting gen #" << gens << " with genome #" << cur_genome.GetID() << "..." << std::endl;
    std::cout << "Max fitness: " << top_fitness << std::endl;
    return true;
  } else {
    return false;
  }
}

// Calculates fitness of current genome and marks the evaluated flag
// Returns the result of advance_genome(), which is false if the simulation finished
bool TuxEvolution::on_tux_death(float progress, int coins)
{
  //Only set fitness - adjfitness is set by species on pop.Epoch()
  cur_genome.SetFitness(tux_evaluate(progress, coins));
  cur_genome.SetEvaluated();
  std::cout << "Organism #" << cur_genome.GetID() << " achieved a fitness of " << cur_genome.GetFitness() << " (" << cur_genome.GetAdjFitness() << ")." << std::endl;;
  return advance_genome();
}

double TuxEvolution::tux_evaluate(float progress, int coins)
{
  float fitness = progress + coins;
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
    cur_genome = *it;
    std::cout << "Continuing evolution with genome #" << cur_genome.GetID() << "." << std::endl;
    return true;
  } else {
    return tux_epoch();
  }
}

// Clear the remaining genome list and iterate over all species' genomes to add them again
// To be called after pop.Epoch()
void TuxEvolution::refresh_genome_list()
{
  remaining_genomes.clear();
  
  for (unsigned int i = 0; i < pop.m_Species.size(); i++) {
    Species cur = pop.m_Species[i];
    for (unsigned int j = 0; j < cur.m_Individuals.size(); j++) {
      remaining_genomes.push_back(cur.m_Individuals[j]);
    }
  }
}

Parameters TuxEvolution::init_params()
{
  Parameters res;
  res.PopulationSize = 2;
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
