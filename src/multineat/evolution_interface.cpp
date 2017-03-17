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

//Maps outputs to controller buttons
void EvolutionInterface::send_outputs() {
  
  if (outputs->direction_up >= SEND_THRESHOLD) {
    controller->press(Controller::UP);
  }
  
  if (outputs->direction_down >= SEND_THRESHOLD) {
    controller->press(Controller::DOWN);
  }
  
  if (outputs->direction_left >= SEND_THRESHOLD) {
    controller->press(Controller::LEFT);
  }
  
  if (outputs->direction_right >= SEND_THRESHOLD) {
    controller->press(Controller::RIGHT);
  }
  
  if (outputs->action >= SEND_THRESHOLD) {
    controller->press(Controller::ACTION);
  }
  
  if (outputs->action >= SEND_THRESHOLD) {
    controller->press(Controller::JUMP);
  }
}

bool EvolutionInterface::check_tux_death()
{
  //TODO If tux died or timeout, call on_tux_death() on the TuxEvolution object
  return false;
}



