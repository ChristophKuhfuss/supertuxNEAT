#include "evolution_interface.hpp"

EvolutionInterface::EvolutionInterface(GameSession* session){
  this->cur_session = session; 
  this->cur_sector = session->get_current_sector();
  this->tux = cur_sector->player;
  sensorValues = new double[SENSOR_GRID_SIZE * SENSOR_GRID_SIZE];
  outputs = new NeatOutputs();
  neat = new TuxEvolution();
}

void EvolutionInterface::update(float elapsed_time) {
  if (!check_tux_death()) {
    neat->accept_inputs(generate_inputs());
    outputs = neat->get_outputs();
    send_outputs();
  }
}

void EvolutionInterface::draw(DrawingContext& context)
{
  //TODO Draw evolution information to screen
}

void EvolutionInterface::send_inputs(NeatInputs* inputs)
{
  neat->accept_inputs(inputs);
}

void EvolutionInterface::send_outputs() {
  
  //TODO Send outputs to tux
}

bool EvolutionInterface::check_tux_death()
{
  //TODO If tux died or timeout, call on_tux_death() on the TuxEvolution object
  return false;
}



