#include "tux_evolution.hpp"
#include <boost/concept_check.hpp>
#include <vector>

using namespace std;

TuxEvolution::TuxEvolution()
{
  pop = 0;
  top_fitness = 0;                                                                                                                      
  cur_inputs = new NeatInputs();
  cur_outputs = new NeatOutputs();
}

TuxEvolution::TuxEvolution(char* paramfile)
{

}

TuxEvolution::~TuxEvolution()
{
  delete pop;
  delete remaining_organisms;
}


void TuxEvolution::accept_inputs(NeatInputs inputs)
{
  this->cur_inputs = inputs;
}

void TuxEvolution::propagate_inputs()
{
  NeuralNetwork* net = cur_org->net;
  net->load_sensors(cur_inputs->sensors);
  net->activate();
}

NeatOutputs* TuxEvolution::get_outputs()
{
  NeuralNetwork* net = cur_org->net;
  propagate_inputs();
  std::vector<NNode*> net_outputs = net->outputs; 
  cur_outputs.direction_up = (*(net_outputs.at(0)))->activation;
  cur_outputs.direction_down = (*(net_outputs.at(1)))->activation;
  cur_outputs.direction_left = (*(net_outputs.at(2)))->activation;
  cur_outputs.direction_right = (*(net_outputs.at(3)))->activation;
  cur_outputs.jump = (*(net_outputs.at(4)))->activation;
  cur_outputs.action = (*(net_outputs.at(5)))->activation;
  
  return cur_outputs;
}

bool TuxEvolution::tux_epoch()
{
  if (gens < num_gens) {
    cur_genome = 0;
    pop->Epoch();
    it = advance_genome();
    return true;
  } else {
    return false;
  }
}

bool TuxEvolution::on_tux_death(double progress, double score)
{
  //Only set fitness - adjfitness is set by species on pop.Epoch()
  cur_genome->SetFitness(tux_evaluate(progress, score));
  cur_genome->SetEvaluated();
  return advance_genome();
}

double TuxEvolution::tux_evaluate(double progress, double score)
{
  // TODO: this is a dummy
  return progress + score;
}

//Advances the current genome to the next one
//Returns false if all generations finished
bool TuxEvolution::advance_genome()
{
  if (it != remaining_genomes.end()) {
    cur_genome = *(++it);
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
  
  for (int i = 0; i < pop->m_Species.size(); i++) {
    Species cur = pop->m_Species[i];
    for (int j = 0; j < cur.m_Individuals.size(); j++) {
      remaining_genomes.push_back(&cur.m_Individuals[j]);
    }
  }
}

double TuxEvolution::get_top_fitness()
{
  return top_fitness;
}

int TuxEvolution::get_generation_number()
{
  return gens;
}
