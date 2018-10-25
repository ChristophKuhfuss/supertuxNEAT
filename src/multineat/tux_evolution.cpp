#include "tux_evolution.hpp"
#include <boost/concept_check.hpp>
#include "Genes.h"
#include "multineat/sensors/sensormanager.hpp"
#include <vector>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <execinfo.h>

#include <sqlite3.h>

bool TuxEvolution::hyperneat = false;

int TuxEvolution::from_genome_id = -1;
int TuxEvolution::to_genome_id = -1;

int TuxEvolution::max_gens = 100;

int TuxEvolution::sensor_grid_size = 15;
int TuxEvolution::sensor_grid_padding = 18;
bool TuxEvolution::custom_sensor_grid = false;

bool TuxEvolution::using_seed = false;
int TuxEvolution::seed;

const char* TuxEvolution::filename = "";
const char* TuxEvolution::paramfilename = "";
  
int TuxEvolution::autosave_interval;

bool TuxEvolution::viewing_mode;
int TuxEvolution::view_genome_id;

string TuxEvolution::dbfile = "";
int TuxEvolution::current_gen = 0;
int TuxEvolution::max_db_retry = 100;
int TuxEvolution::db_sleeptime = 50;

TuxEvolution::TuxEvolution() : params(init_params()),
  start_genome(0, SensorManager::get_total_sensor_count() + 1, 5, 
	       6, false, UNSIGNED_SIGMOID, UNSIGNED_SIGMOID, 1, params),
  pop(strcmp(filename, "") ? Population(filename) : Population(start_genome, params, true, 2.0, (using_seed ? seed : (int) time(0)))),
  top_fitness(0),
  top_fitness_genome_id(-1),
  cur_outputs(),
  cur_inputs()
{
  pop.m_RNG.Seed(seed);
  
  if (SensorManager::instance && hyperneat) {
    generate_substrate(SensorManager::instance);
  }
  
  if (!viewing_mode) {
    gens = 0;

    refresh_genome_list();
    it = remaining_genomes.begin();
    get_genome_from_iterator();
  } else {
    set_viewing_genome();
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
  vector<double> inputs = vector<double>(cur_inputs.sensors);
  //Bias
  inputs.push_back(1);
  cur_network.Input(inputs);
  
  // Activate depth times to ensure full input propagation
  for (unsigned int i = 0; i < cur_genome->GetDepth(); i++) {
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
// The whole update() system of SuperTux makes the whole structure of the EA a little more complicated
bool TuxEvolution::tux_epoch()
{
  std::cout << "Generation #" << (gens + 1) << " finished." << " Max fitness: " << top_fitness << std::endl;
  if (autosave_interval) {
    autosave();
  }
  gens++;
  if (gens < max_gens) {    
    pop.Epoch();
    refresh_genome_list();
    it = remaining_genomes.begin();
    get_genome_from_iterator();
    top_fitness = 0;
    std::cout << "Starting generation #" << (gens + 1) << " with genome #" << cur_genome->GetID() << "..." << std::endl;
    return true;
  } else {
    std::cout << "Finished evolution." << std::endl;
    return false;
  }
}

// Calculates fitness of current genome and marks the evaluated flag
// Returns the result of advance_genome(), which is false if the simulation finished
bool TuxEvolution::on_tux_death(float progress, OutputQuotas q)
{
  double fitness = tux_evaluate(progress);
//   std::cout << "Organism #" << cur_genome->GetID() << " achieved a fitness of " << fitness << "." << std::endl;
  
  if (!viewing_mode) {
    //Only set fitness - adjfitness is set by species on pop.Epoch()
    cur_genome->SetFitness(fitness);
    update_db(cur_genome->GetID(), fitness, q);
    return advance_genome();
  } else {
    cur_network.Flush();
    return true;
  }
}

double TuxEvolution::tux_evaluate(float progress)
{
  float fitness = progress;
  if (fitness > top_fitness) {
    top_fitness = fitness;
  }
  return fitness;
}

// Advances the current genome to the next one
// Returns false if all generations finished OR we're in parallel mode
bool TuxEvolution::advance_genome()
{
  ++it;
  if (it != remaining_genomes.end()) {
    get_genome_from_iterator();
    return true;
  } else if (from_genome_id == -1 && to_genome_id == -1) {
    return tux_epoch();
  } else {
    // We're done!
    return false;
  }
}

// Clear the remaining genome list and iterate over all species' genomes to add them again
// To be called after pop.Epoch()
void TuxEvolution::refresh_genome_list()
{
  remaining_genomes.clear();
  
  std::vector<Genome*> genomes_to_add;
  
  for (std::vector<Species>::iterator it2 = pop.m_Species.begin(); it2 != pop.m_Species.end(); ++it2) {
    Species* cur = &(*it2);
    
    for (std::vector<Genome>::iterator it3 = cur->m_Individuals.begin(); it3 != cur->m_Individuals.end(); ++it3) {
      genomes_to_add.push_back(&(*it3));
    }
  }
  
  
  // If from_genome_id and to_genome_id are set, we're in parallel mode!
  // Only add the genomes we need
  if (from_genome_id != -1 && to_genome_id != -1) {
    int i;
    for (i = from_genome_id; i <= to_genome_id; i++) {
      remaining_genomes.push_back(genomes_to_add[i]);
    }
  } else {
    remaining_genomes = genomes_to_add;
  }
}

void TuxEvolution::generate_substrate(SensorManager* sm)
{
  this->sm = sm;
  
  std::vector<std::vector<double>> input_coords;
  std::vector<std::vector<double>> hidden_coords;	// TODO: Add support for hidden neurons (third dimension, arrange in a circle)
  std::vector<std::vector<double>> output_coords;
  
  std::vector<std::shared_ptr<RangeFinderSensor>>* rfsensors = sm->get_cur_rangefinder_sensors();
  std::vector<std::shared_ptr<DepthFinderSensor>>* dfsensors = sm->get_cur_depthfinder_sensors();
  std::vector<std::shared_ptr<PieSliceSensor>>*    pssensors = sm->get_cur_pieslice_sensors();
  
  // First, calculate maximum x and y coordinates so we can normalize substrate coordinates to (-1, 1)
  int maxX = 0;
  int maxY = 0;
  
  for (std::vector<std::shared_ptr<RangeFinderSensor>>::iterator it = rfsensors->begin(); it != rfsensors->end(); ++it)
  {
    Vector v = (*it)->get_offset();
    
    // Get middle point of the RangeFinderSensor
    int x = abs(v.x / 2.0);
    int y = abs(v.y);
    
    if (x > maxX)
      maxX = x;
    
    if (y > maxY) 
      maxY = y;
    
  }
  
  for (std::vector<std::shared_ptr<DepthFinderSensor>>::iterator it = dfsensors->begin(); it != dfsensors->end(); ++it)
  {
    Vector v = (*it)->get_offset();
    
    // Get middle point of the DepthFinderSensor
    int x = abs(v.x);
    int y = abs(v.y / 2.0);
    
    if (x > maxX) 
      maxX = x;
    
    if (y > maxY)
      maxY = y;
    
  }
  
  for (std::vector<std::shared_ptr<PieSliceSensor>>::iterator it = pssensors->begin(); it != pssensors->end(); ++it)
  {
    Vector v1 = (*it)->get_offset();
    Vector v2 = (*it)->get_offset2();
    
    // Get middle point of the PieSliceSensor
    // Divide by three because both vectors form a triangle with (0, 0) as the third point
    int x = abs((v1.x + v2.x) / 3.0);
    int y = abs((v1.y + v2.y) / 3.0);
    
    if (x > maxX)
      maxX = x;
    
    if (y > maxY)
      maxY = y;
    
  }
    
  for (std::vector<std::shared_ptr<RangeFinderSensor>>::iterator it = rfsensors->begin(); it != rfsensors->end(); ++it)
  {
    std::vector<double> coords;
    
    Vector v = (*it)->get_offset();
    
    // Normalize between (0, 1) and then between (-1, 1)
    coords.push_back(v.x / (double) maxX);
    coords.push_back(v.y / (double) maxY);
    
    coords.push_back(v.x);
    coords.push_back(v.y);
    
    input_coords.push_back(coords);
  }
  
  for (std::vector<std::shared_ptr<DepthFinderSensor>>::iterator it = dfsensors->begin(); it != dfsensors->end(); ++it)
  {
    std::vector<double> coords;
    
    Vector v = (*it)->get_offset();
    
    coords.push_back((v.x / (double) maxX));
    coords.push_back((v.y / (double) maxY));
    
    coords.push_back(v.x);
    coords.push_back(v.y);
    
    input_coords.push_back(coords);
  }
  
  for (std::vector<std::shared_ptr<PieSliceSensor>>::iterator it = pssensors->begin(); it != pssensors->end(); ++it)
  {
    std::vector<double> coords;
    
    Vector v1 = (*it)->get_offset();
    Vector v2 = (*it)->get_offset2();
    
    // Same procedure as above, third point is (0, 0)    
    coords.push_back(((v1.x + v2.x) / 3.0) / (double) maxX);
    coords.push_back(((v1.y + v2.y) / 3.0) / (double) maxY);
    
    input_coords.push_back(coords);
  }
  
  std::vector<double> bias;
  bias.push_back(0);
  bias.push_back(0);
  
  input_coords.push_back(bias);
  
  
  // TODO: Arrange hidden neurons in a circle around Tux, seems like a good idea
  
  std::vector<double> output;
  
  // Arrange output substrate coordinates around Tux
  // UP should be above Tux, DOWN below, etc. with Tux "being" at (0, 0)
  
  // UP
  output.push_back(0);
  output.push_back(1);
  
  output_coords.push_back(output);
  output.clear();
  
  // DOWN
  output.push_back(0);
  output.push_back(-1);
  
  output_coords.push_back(output);
  output.clear();
  
  // LEFT
  output.push_back(-1);
  output.push_back(0);
  
  output_coords.push_back(output);
  output.clear();
  
  // RIGHT
  output.push_back(1);
  output.push_back(0);
  
  output_coords.push_back(output);
  output.clear();
  
  // JUMP
  output.push_back(0);
  output.push_back(0.8);
  
  output_coords.push_back(output);
  output.clear();
  
  // ACTION
  output.push_back(0);
  output.push_back(0.1);
  
  output_coords.push_back(output);
  
  // Parameters taken from MultiNEAT's HyperNEAT example
  
  substrate = Substrate(input_coords, hidden_coords, output_coords);
  
  substrate.m_allow_hidden_hidden_links = false;
  substrate.m_allow_hidden_output_links = true;
  substrate.m_allow_input_hidden_links = true;
  substrate.m_allow_input_output_links = true;
  substrate.m_allow_looped_hidden_links = false;
  substrate.m_allow_looped_output_links = false;
  substrate.m_allow_output_hidden_links = false;
  substrate.m_allow_output_output_links = false;
  substrate.m_query_weights_only = true;
  substrate.m_max_weight_and_bias = 8;
  
  substrate.m_hidden_nodes_activation = ActivationFunction::SIGNED_SIGMOID;
  substrate.m_output_nodes_activation = ActivationFunction::UNSIGNED_SIGMOID;
  
//   params.PopulationSize = 150;
// 
//   params.DynamicCompatibility = true;
//   params.CompatTreshold = 2.0;
//   params.YoungAgeTreshold = 15;
//   params.SpeciesMaxStagnation = 100;
//   params.OldAgeTreshold = 35;
//   params.MinSpecies = 5;
//   params.MaxSpecies = 10;
//   params.RouletteWheelSelection = false;
// 
//   params.MutateRemLinkProb = 0.02;
//   params.RecurrentProb = 0;
//   params.OverallMutationRate = 0.15;
//   params.MutateAddLinkProb = 0.08;
//   params.MutateAddNeuronProb = 0.01;
//   params.MutateWeightsProb = 0.90;
//   params.MaxWeight = 8.0;
//   params.WeightMutationMaxPower = 0.2;
//   params.WeightReplacementMaxPower = 1.0;
// 
//   params.MutateActivationAProb = 0.0;
//   params.ActivationAMutationMaxPower = 0.5;
//   params.MinActivationA = 0.05;
//   params.MaxActivationA = 6.0;
// 
//   params.MutateNeuronActivationTypeProb = 0.03;
// 
//   params.ActivationFunction_SignedSigmoid_Prob = 0.0;
//   params.ActivationFunction_UnsignedSigmoid_Prob = 0.0;
//   params.ActivationFunction_Tanh_Prob = 1.0;
//   params.ActivationFunction_TanhCubic_Prob = 0.0;
//   params.ActivationFunction_SignedStep_Prob = 1.0;
//   params.ActivationFunction_UnsignedStep_Prob = 0.0;
//   params.ActivationFunction_SignedGauss_Prob = 1.0;
//   params.ActivationFunction_UnsignedGauss_Prob = 0.0;
//   params.ActivationFunction_Abs_Prob = 0.0;
//   params.ActivationFunction_SignedSine_Prob = 1.0;
//   params.ActivationFunction_UnsignedSine_Prob = 0.0;
//   params.ActivationFunction_Linear_Prob = 1.0;
  
  start_genome = Genome(0, substrate.GetMinCPPNInputs(), 0, 
	       substrate.GetMinCPPNOutputs(), false, TANH, TANH, 0, params);
  pop = strcmp(filename, "") ? Population(filename) : Population(start_genome, params, true, 1.0, (using_seed ? seed : (int) time(0)));
}


// For debugging purposes
void TuxEvolution::print_all_genomes()
{
  for (unsigned int i = 0; i < pop.m_Species.size(); i++) { 
    Species s = pop.m_Species[i];
    for (unsigned int j = 0; j < s.m_Individuals.size(); j++) {
      std::cout << "Genome #" << s.m_Individuals[j].GetID() << ", Fitness: " << s.m_Individuals[j].GetFitness() << std::endl;
    }
  }
}

Parameters TuxEvolution::init_params()
{
  Parameters res;
  
  if (strcmp(paramfilename, "") != 0) { 
    res.Load(paramfilename);
  }
  
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

// Takes the next genome from the iterator and builds the network accordingly
void TuxEvolution::get_genome_from_iterator()
{
  cur_genome = *it;
  
  // If we don't clear the network first, we'll run into some bad problems with HyperNEAT
  // Each produced network would have input + output more neurons than the last one
  cur_network.Clear();
  
  hyperneat ? cur_genome->BuildHyperNEATPhenotype(cur_network, substrate) : cur_genome->BuildPhenotype(cur_network);
  
  cur_genome->CalculateDepth();
  
  std::cout << "Net inputs: " << cur_network.m_num_inputs << ", net outputs: " << cur_network.m_num_outputs << ", net hidden: " << cur_network.m_neurons.size() - cur_network.m_num_inputs - cur_network.m_num_outputs << std::endl;
  std::cout << "Net depth: " << cur_genome->GetDepth() << std::endl;
  std::cout << "Connection count: " << cur_network.m_connections.size() << std::endl;
  for (std::vector<Connection>::iterator it = cur_network.m_connections.begin(); it != cur_network.m_connections.end(); ++it) {
    std::cout << (*it).m_weight << std::endl;
  }
}

// Just to make sure we don't lose winners
void TuxEvolution::on_level_won()
{
  save_pop();
}

// For each genome, update the row in the current db table with the right fitness value
void TuxEvolution::update_db(int genome_id, float fitness, OutputQuotas q)
{
  sqlite3* db;  
  
  sqlite3_open("neat.db", &db);
  sqlite3_busy_handler(db, busy_handler, (void*) nullptr);
  
  char* err;
  
  std::stringstream ss;
    
  ss << "UPDATE gen" << current_gen << " SET fitness = " << fitness << 
	", qLeft = " << q.qLeft <<
	", qRight = " << q.qRight <<
	", qUp = " << q.qUp <<
	", qDown = " << q.qDown <<
	", qJump = " << q.qJump <<
	", qAction = " << q.qAction << 
	" WHERE id = " << genome_id << ";";
        
  sqlite3_exec(db, ss.str().c_str(), 0, 0, &err);
  
  sqlite3_close(db);
}

int TuxEvolution::busy_handler(void* data, int retry)
{
  if (retry < max_db_retry) {
    sqlite3_sleep(db_sleeptime);
    return 1;
  } else {
    return 0;
  }
}

int TuxEvolution::get_current_genome_id()
{
  return cur_genome->GetID();
}

void TuxEvolution::autosave()
{
  if ((gens + 1) % autosave_interval == 0) {
    save_pop();
  }
}

void TuxEvolution::save_pop()
{
    std::ostringstream ss;
    ss << "./neat_gen" << (gens + 1);
    pop.Save(ss.str().c_str());
}

void TuxEvolution::set_viewing_genome()
{
  refresh_genome_list();
  
  it = remaining_genomes.begin();
  
  while (it != remaining_genomes.end() && (*it)->GetID() != view_genome_id) ++it;
  
  if (it == remaining_genomes.end()) {
    std::ostringstream ss;
    ss << "Couldn't find genome with ID " << view_genome_id;
    
    throw std::runtime_error(ss.str());
  } else {
    get_genome_from_iterator();
    std::cout << "Found genome. Starting playback..." << std::endl;
  }
}