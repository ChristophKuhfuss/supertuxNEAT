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
  Network* net = cur_org->net;
  net->load_sensors(cur_inputs->sensors);
  net->activate();
}

NeatOutputs* TuxEvolution::get_outputs()
{
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
    pop->epoch();
    it = pop->organisms.begin();
    return true;
  } else {
    return false;
  }
}

bool TuxEvolution::on_tux_death(double progress, double score)
{
  cur_org->fitness=tux_evaluate(progress, score);
  if (it != pop->organisms.end()) {
    cur_org = ++it;
    return true;
  } else {
    return tux_epoch();
  }
}

double TuxEvolution::tux_evaluate(double progress, double score)
{
  // TODO: this is a dummy
  return progress + score;
}

double TuxEvolution::get_top_fitness()
{
  return top_fitness;
}

int TuxEvolution::get_generation_number()
{
  return gens;
}



