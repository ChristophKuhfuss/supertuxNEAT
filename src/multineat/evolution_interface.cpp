#include "evolution_interface.hpp"

EvolutionInterface::EvolutionInterface(GameSession* session) :
cur_session(session),
/*cur_sector(cur_session->get_current_sector()),
tux(cur_sector->player),*/
sensorValues(new double[TuxEvolution::SENSOR_GRID_SIZE * TuxEvolution::SENSOR_GRID_SIZE]),
neat()
{
}

EvolutionInterface::~EvolutionInterface()
{
  delete[] sensorValues;
}

void EvolutionInterface::init()
{
  if (!initialized) {
    cur_sector = cur_session->get_current_sector();
    tux = cur_sector->player;
  }
}


void EvolutionInterface::update(float elapsed_time) 
{
  update_idle(elapsed_time);
  if (idle < TIMEOUT) {
    neat.accept_inputs(generate_inputs());
    outputs = neat.get_outputs();
    send_outputs();
  } else {
    timeout();
  }
}

NeatInputs EvolutionInterface::generate_inputs()
{
    vector<double> inputs;
    for (std::vector<Sensor>::iterator it = sensors.begin(); it != sensors.end(); ++it) {
      inputs.push_back(it->getValue());
    }
    
    NeatInputs res;
    res.sensors = inputs;
    
    return res;
}


void EvolutionInterface::draw(DrawingContext& context) 
{
  //TODO Draw evolution information to screen
}

void EvolutionInterface::send_inputs(NeatInputs inputs)
{
  neat.accept_inputs(inputs);
}

//Maps outputs to controller buttons
void EvolutionInterface::send_outputs() 
{
  
  if (outputs.direction_up >= SEND_THRESHOLD) {
    controller->press(Controller::UP);
  }
  
  if (outputs.direction_down >= SEND_THRESHOLD) {
    controller->press(Controller::DOWN);
  }
  
  if (outputs.direction_left >= SEND_THRESHOLD) {
    controller->press(Controller::LEFT);
  }
  
  if (outputs.direction_right >= SEND_THRESHOLD) {
    controller->press(Controller::RIGHT);
  }
  
  if (outputs.action >= SEND_THRESHOLD) {
    controller->press(Controller::ACTION);
  }
  
  if (outputs.action >= SEND_THRESHOLD) {
    controller->press(Controller::JUMP);
  }
}

void EvolutionInterface::on_tux_death()
{
  if (!neat.on_tux_death(tux->get_pos().x, tux->get_coins())) {
    cur_session->toggle_pause();
  }
  idle = 0;
}

void EvolutionInterface::update_idle(float elapsed_time) 
{
  if (last_known_playerpos != tux->get_pos()) {
      idle = 0;
      last_known_playerpos = tux->get_pos();
  } else {
    idle += elapsed_time;
  }
}

void EvolutionInterface::timeout() 
{
  if (TuxEvolution::debug) {
    std::cout << "Timeout occured!" << std::endl;
  }
  cur_session->restart_level(false);
  on_tux_death();
}

void EvolutionInterface::add_sensor(Sensor s)
{
  if (TuxEvolution::debug) {
    std::cout << "Adding sensor..." << std::endl;
  }
  sensors.push_back(s);
}


ObjectSettings EvolutionInterface::get_settings()
{
  return GameObject::get_settings();
}

void EvolutionInterface::save(Writer& writer)
{
  GameObject::save(writer);
}

