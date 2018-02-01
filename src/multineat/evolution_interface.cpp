#include "evolution_interface.hpp"
#include <supertux/gameconfig.hpp>

EvolutionInterface::EvolutionInterface(GameSession* session) :
cur_session(session),
sensorValues(new double[TuxEvolution::sensor_grid_size * TuxEvolution::sensor_grid_size]),
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
  cur_sector = cur_session->get_current_sector();
  tux = cur_sector->player;
  tux->set_controller(controller.get());
}


void EvolutionInterface::update(float elapsed_time) 
{  
  if (TuxEvolution::debug)
    debug_print();
  
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
    }
        
    NeatInputs res;
    res.sensors = inputs;
    
    return res;
}


void EvolutionInterface::draw(DrawingContext& context) 
{
  //TODO Draw evolution information to screen (maybe, maybe not, probably not)
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
      std::cerr << "Button:U" << std::endl;
    }
    controller->press(Controller::UP);
  }
  
  if (outputs.direction_down >= SEND_THRESHOLD) {
    if (TuxEvolution::debug) {
      std::cerr << "Button:D" << std::endl;
    }
    controller->press(Controller::DOWN);
  }
  
  if (outputs.direction_left >= SEND_THRESHOLD) {
    if (TuxEvolution::debug) {
      std::cerr << "Button:L" << std::endl;
    }

    controller->press(Controller::LEFT);
  }
  
  if (outputs.direction_right >= SEND_THRESHOLD) {
    if (TuxEvolution::debug) {
      std::cerr << "Button:R" << std::endl;
    }

    controller->press(Controller::RIGHT);
  }
  
  if (outputs.action >= SEND_THRESHOLD) {
    if (TuxEvolution::debug) {
      std::cerr << "Button:A" << std::endl;
    }

    controller->press(Controller::ACTION);
  }
  
  if (outputs.action >= SEND_THRESHOLD) {
    if (TuxEvolution::debug) {
      std::cerr << "Button:J" << std::endl;
    }
    controller->press(Controller::JUMP);
  }
}

void EvolutionInterface::on_tux_death()
{
  if (TuxEvolution::debug) {
    std::cerr << "-------------------------------" << std::endl;
  }
  
  sensors.clear();
  controller->reset();
  
  if (!neat.on_tux_death(tux->get_pos().x)) {
    scripting::quit();
  }
  
  idle = 0;
  fitness_idle = 0;
  max_x = 0;
}

void EvolutionInterface::on_level_won()
{
  std::cout << "Organism #" << neat.get_current_genome_id() << " finished the level!" << std::endl;
  neat.on_level_won();
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

void EvolutionInterface::debug_print()
{
  std::cerr << "Tux:" << tux->get_pos().x << "," << tux->get_pos().y << std::endl;
  for (int i = 0; i < sensors.size(); i++) {
    Sensor s = *sensors[i];
    std::cerr << s.getValue();
  }
  
  std::cerr << std::endl;
}


