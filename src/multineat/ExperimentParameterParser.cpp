#include "ExperimentParameterParser.hpp"

#include <iostream>

ExperimentParameterParser::ExperimentParameterParser(string filename) : file(filename)
{
}

ExperimentParameterParser::~ExperimentParameterParser()
{

}

void ExperimentParameterParser::load()
{
  string line, s;
  double d;
  
  
  
  while (std::getline(file, line)) {
    stringstream ss;
    
    ss << line;
    
    if (!(ss >> s >> d)) continue;
    
    if (s == "maxgens") 			TuxEvolution::max_gens = (int) d;
    else if (s == "randseed") {
      TuxEvolution::using_seed = true;
      TuxEvolution::seed = (int) d;
    }
    else if (s == "autosaveinterval")		TuxEvolution::autosave_interval = (int) d;
    
    else if (s == "numhiddenstartneurons")	TuxEvolution::num_hidden_start_neurons = (int) d;
    else if (s == "numhiddenstartneuronscppn")	TuxEvolution::num_hidden_start_neurons_cppn = (int) d;
        
    else if (s == "numrangesensors") 		SensorManager::AMOUNT_RANGE_SENSORS = (int) d;
    else if (s == "numdepthsensors") 		SensorManager::AMOUNT_DEPTH_SENSORS = (int) d;
    else if (s == "numpiesensors") 		SensorManager::AMOUNT_PIESLICE_SENSORS = (int) d;
    
    else if (s == "lengthrangesensors") 	RangeFinderSensor::length = (int) d;
    else if (s == "lengthdepthsensors") 	DepthFinderSensor::length = (int) d;
    else if (s == "radiuspieslicesensors")	PieSliceSensor::radius = (int) d;
    
    else if (s == "spacingdepthsensors")	SensorManager::SPACING_DEPTH_SENSORS = (int) d;
    
    else if (s == "timeout")			EvolutionInterface::TIMEOUT = d;
    else if (s == "fitnesstimeout")		EvolutionInterface::FITNESS_TIMEOUT = d;
    else if (s == "sendthreshold")		EvolutionInterface::SEND_THRESHOLD = d;
    
    else if (s == "regularizejumps")	
      if ((int) d)				TuxEvolution::regularize_jumps = true;
      else 					TuxEvolution::regularize_jumps = false;
  }
}

