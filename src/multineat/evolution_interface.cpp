#include "evolution_interface.hpp"
#include <supertux/gameconfig.hpp>

float EvolutionInterface::TIMEOUT = 1.0f;
float EvolutionInterface::FITNESS_TIMEOUT = 3.0f;
float EvolutionInterface::SEND_THRESHOLD = 0.7f;

EvolutionInterface::EvolutionInterface(GameSession* session, std::shared_ptr<SensorManager> sensor_manager) :
cur_session(session),
sensor_manager(sensor_manager),
sensors(sensor_manager->get_cur_sensors()),
sensorValues(new double[SensorManager::get_total_sensor_count()]),
neat(),			// will not be instantiated in this class anymore but from the outside
controller(new CodeController),
max_x()
{
}

EvolutionInterface::~EvolutionInterface()
{
  delete[] sensorValues;
  controller.reset();
}

void EvolutionInterface::init()
{
  cur_sector = cur_session->get_current_sector();
  tux = cur_sector->player;
  tux->set_controller(controller.get());
  sensor_manager->initSensors();
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
    std::vector<std::shared_ptr<Sensor>>::iterator it = sensors->begin();
    
    for (it = sensors->begin(); it != sensors->end(); ++it) {
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
  
  if (outputs.jump >= SEND_THRESHOLD) {
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
  
  sensor_manager->clearSensors();
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
  on_tux_death();
  cur_session->restart_level(false);
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
  sensors->push_back(s);
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
  for (int i = 0; i < sensors->size(); i++) {
    Sensor s = *((*sensors)[i]);
    std::cerr << s.getValue();
  }
  
  std::cerr << std::endl;
}


