#ifndef EXP_PARAM_PARSER
#define EXP_PARAM_PARSER

#include <fstream>

#include "multineat/tux_evolution.hpp"
#include "multineat/evolution_interface.hpp"

#include "multineat/sensors/sensor.hpp"
#include "multineat/sensors/rangefindersensor.hpp"
#include "multineat/sensors/depthfindersensor.hpp"
#include "multineat/sensors/pieslicesensor.hpp"

class ExperimentParameterParser {
private:
  std::ifstream file;
  
public:
  ExperimentParameterParser(std::string filename);
  ~ExperimentParameterParser();
  
  void load();
};

#endif /* EOF */