#include "evolution_interface.hpp"

EvolutionInterface::EvolutionInterface(GameSession* session) :
cur_session(session),
sensorValues(new double[TuxEvolution::SENSOR_GRID_SIZE * TuxEvolution::SENSOR_GRID_SIZE]),
neat(),
controller(new CodeController),
max_x()
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
    tux->set_controller(controller.get());
  }
}


void EvolutionInterface::update(float elapsed_time) 
{
  update_idle(elapsed_time);
  if (idle < TIMEOUT && fitness_idle < FITNESS_TIMEOUT) {
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
    std::vector<std::shared_ptr<Sensor>>::iterator it = sensors.begin();
    
    for (it = sensors.begin(); it != sensors.end(); ++it) {
      inputs.push_back((*it)->getValue());
      //std::cout << (*it)->getValue() << " ";
    }
    
    //std::cout << std::endl;
    
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
  controller->update();
  
  if (outputs.direction_up >= SEND_THRESHOLD) {
    if (TuxEvolution::debug) {
      std::cout << "Controller press: UP" << std::endl;
    }
    controller->press(Controller::UP);
  }
  
  if (outputs.direction_down >= SEND_THRESHOLD) {
    if (TuxEvolution::debug) {
      std::cout << "Controller press: DOWN" << std::endl;
    }
    controller->press(Controller::DOWN);
  }
  
  if (outputs.direction_left >= SEND_THRESHOLD) {
    if (TuxEvolution::debug) {
      std::cout << "Controller press: LEFT" << std::endl;
    }

    controller->press(Controller::LEFT);
  }
  
  if (outputs.direction_right >= SEND_THRESHOLD) {
    if (TuxEvolution::debug) {
      std::cout << "Controller press: RIGHT" << std::endl;
    }

    controller->press(Controller::RIGHT);
  }
  
  if (outputs.action >= SEND_THRESHOLD) {
    if (TuxEvolution::debug) {
      std::cout << "Controller press: ACTION" << std::endl;
    }

    controller->press(Controller::ACTION);
  }
  
  if (outputs.action >= SEND_THRESHOLD) {
    if (TuxEvolution::debug) {
      std::cout << "Controller press: JUMP" << std::endl;
    }

    controller->press(Controller::JUMP);
  }
}

void EvolutionInterface::on_tux_death()
{
  std::cout << "Sensor vector size: " << sensors.size() << std::endl;

  sensors.clear();
  
  if (!neat.on_tux_death(tux->get_pos().x, tux->get_coins())) {
    cur_session->toggle_pause();
  }
  
  idle = 0;
  fitness_idle = 0;
  max_x = 0;
}

void EvolutionInterface::update_idle(float elapsed_time) 
{
  if (last_known_playerpos != tux->get_pos()) {
      idle = 0;
      last_known_playerpos = tux->get_pos();
  } else {
    idle += elapsed_time;
  }
  
  if (tux->get_pos().x > max_x) {
    max_x = tux->get_pos().x;
    fitness_idle = 0;
  } else {
    fitness_idle += elapsed_time;
  }
}

void EvolutionInterface::timeout() 
{
  on_tux_death();
  
  if (TuxEvolution::debug) {
    std::cout << "Timeout occured!" << std::endl;
  }
  
  cur_session->restart_level(false);
}

void EvolutionInterface::add_sensor(std::shared_ptr<Sensor> s)
{
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

