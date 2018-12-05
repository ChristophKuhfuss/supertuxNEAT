#include "evolution_interface.hpp"
#include <supertux/gameconfig.hpp>

float EvolutionInterface::TIMEOUT = 1.0f;
float EvolutionInterface::FITNESS_TIMEOUT = 3.0f;
float EvolutionInterface::SEND_THRESHOLD = 0.7f;

int EvolutionInterface::MAX_JUMP_TICKS = 35;

EvolutionInterface::EvolutionInterface(GameSession* session, std::shared_ptr<SensorManager> sensor_manager) :
cur_session(session),
sensor_manager(sensor_manager),
sensors(sensor_manager->get_cur_sensors()),
sensorValues(new double[SensorManager::get_total_sensor_count()]),
neat(),			// will not be instantiated in this class anymore but from the outside
controller(new CodeController),
max_x(),
ticks_total(0),
ticks_down(0),
ticks_up(0),
ticks_left(0),
ticks_right(0),
ticks_jump(0),
ticks_action(0),
ticks_airtime(0),
ticks_groundtime(0),
remaining_jump_ticks(0),
jump_skip_next_frame(false),
num_jumps(0),
idle(0),
fitness_idle(0)
{
}

EvolutionInterface::~EvolutionInterface()
{
  delete[] sensorValues;
  controller.reset();
}

void EvolutionInterface::init()
{
  controller->reset();
  cur_sector = cur_session->get_current_sector();
  tux = cur_sector->player;
  tux->set_controller(controller.get());
  sensor_manager->initSensors(true);
}


void EvolutionInterface::update(float elapsed_time) 
{  
  if (TuxEvolution::debug)
    debug_print();
  
  ticks_total++;
  
  if (!tux->on_ground()) ticks_airtime++;
  else ticks_groundtime++;
  
  update_idle(elapsed_time);
  
  if (idle < TIMEOUT && fitness_idle < FITNESS_TIMEOUT) {
    send_inputs(generate_inputs());
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
  controller->reset();
  
  // Only calculate new remaining jump ticks if we don't have to skip and the threshold is right
  if (outputs.jump >= SEND_THRESHOLD) {
    if (!jump_skip_next_frame) {
      if (!remaining_jump_ticks && tux->on_ground()) {
	remaining_jump_ticks = (int) std::min((double) MAX_JUMP_TICKS, std::max(0.0, MAX_JUMP_TICKS * (outputs.jump - SEND_THRESHOLD) / (0.95 - SEND_THRESHOLD)));
	
	if (remaining_jump_ticks)
	  num_jumps++;
      }
    } else {
      jump_skip_next_frame = false;
    }
  }
    
  if (remaining_jump_ticks > 0) {
    controller->press(Controller::JUMP);
    ticks_jump++;
    remaining_jump_ticks--;
    if (remaining_jump_ticks == 0) jump_skip_next_frame = true;
  }
  
  if (outputs.direction_up >= SEND_THRESHOLD) {
    controller->press(Controller::UP);
    ticks_up++;
  }
  
  if (outputs.direction_down >= SEND_THRESHOLD) {
    controller->press(Controller::DOWN);
    ticks_down++;
   }
  
  if (outputs.direction_left >= SEND_THRESHOLD) {
    controller->press(Controller::LEFT);
    ticks_left++;
  }
  
  if (outputs.direction_right >= SEND_THRESHOLD) {
    controller->press(Controller::RIGHT);
    ticks_right++;
  }
  
  if (outputs.action >= SEND_THRESHOLD) {
    controller->press(Controller::ACTION);
    ticks_action++;
  }
}

void EvolutionInterface::on_tux_death()
{  
  sensor_manager->clearSensors();
  controller->reset();
  
  remaining_jump_ticks = 0;
  jump_skip_next_frame = false;
  
  OutputQuotas q = {0, 0, 0, 0, 0, 0};
  
  if (ticks_total > 0) {
    q.qDown = ticks_down / (double) ticks_total;
    q.qUp = ticks_up / (double) ticks_total;
    q.qLeft = ticks_left / (double) ticks_total;
    q.qRight = ticks_right / (double) ticks_total;
    q.qJump = ticks_jump / (double) ticks_total;
    q.qAction = ticks_action / (double) ticks_total;
  }
  
  if (!neat.on_tux_death(tux->get_pos().x, ticks_airtime / (float) ticks_total, ticks_groundtime / (float) ticks_total, num_jumps, q)) {
    scripting::quit();
  }
  
  idle = 0;
  fitness_idle = 0;
  max_x = 0;
  
  ticks_down = 0;
  ticks_up = 0;
  ticks_left = 0;
  ticks_right = 0;
  ticks_jump = 0;
  ticks_action = 0;
  ticks_airtime = 0;
  ticks_groundtime = 0;
  ticks_total = 0; 
  
  num_jumps = 0;
}

void EvolutionInterface::on_level_won()
{
//   std::cout << "Organism #" << neat.get_current_genome_id() << " finished the level!" << std::endl;
  neat.on_level_won();
  cur_session->restart_level(true);
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
  cur_session->restart_level(true);
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
//   std::stringstream ss;
//   ss << debug_filename << debug_filename_number;
//   std::ofstream os;
//   
//   os.open(ss.str(), std::ofstream::out | std::ofstream::app);
//   
//   os << "Tux:" << tux->get_pos().x << "," << tux->get_pos().y << std::endl;
//   os << "Inputs: ";
//   for (int i = 0; i < sensors->size(); i++) {
//     Sensor s = *((*sensors)[i]);
//     os << s.getValue() << " ";
//   }
//   
//   os << std::endl;
}


