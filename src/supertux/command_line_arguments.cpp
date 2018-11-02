//  SuperTux
//  Copyright (C) 2014 Ingo Ruhnke <grumbel@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "supertux/command_line_arguments.hpp"

#include <boost/format.hpp>
#include <iostream>
#include <physfs.h>
#include <stdexcept>
#include <string>

#include "supertux/gameconfig.hpp"
#include "supertux/main.hpp"
#include "util/gettext.hpp"
#include "version.h"
#include "math/vector.hpp"
#include "multineat/tux_evolution.hpp"
#include "multineat/evolution_interface.hpp"

CommandLineArguments::CommandLineArguments() :
  m_action(NO_ACTION),
  m_log_level(LOG_WARNING),
  datadir(),
  userdir(),
  fullscreen_size(),
  fullscreen_refresh_rate(),
  window_size(),
  aspect_size(),
  use_fullscreen(),
  video(),
  show_fps(),
  show_player_pos(),
  sound_enabled(),
  music_enabled(),
  start_level(),
  enable_script_debugger(),
  start_demo(),
  record_demo(),
  tux_spawn_pos(),
  developer_mode(),
  christmas_mode(),
  repository_url(),
  edit_level()
{
}

CommandLineArguments::~CommandLineArguments()
{
}

void
CommandLineArguments::print_datadir() const
{
  // Print the datadir searchpath to stdout, one path per
  // line. Then exit. Intended for use by the supertux-editor.
  char **sp;
  sp = PHYSFS_getSearchPath();
  if (sp)
    for (size_t sp_index = 0; sp[sp_index]; sp_index++)
      std::cout << sp[sp_index] << std::endl;
  PHYSFS_freeList(sp);
}

void
CommandLineArguments::print_help(const char* arg0) const
{
  std::cerr
            << boost::format(_(     "Usage: %s [OPTIONS] [LEVELFILE]")) % arg0 << "\n" << "\n"
            << _(     "General Options:" ) << "\n"
            << _(     "  -h, --help                   Show this help message and quit") << "\n"
            << _(     "  -v, --version                Show SuperTux version and quit") << "\n"
            << _(     "  --verbose                    Print verbose messages") << "\n"
            << _(     "  --debug                      Print extra verbose messages") << "\n"
            << _( "  --print-datadir              Print SuperTux's primary data directory.") << "\n" << "\n"
            << _(     "Video Options:") << "\n"
            << _(     "  -f, --fullscreen             Run in fullscreen mode") << "\n"
            << _(     "  -w, --window                 Run in window mode") << "\n"
            << _(     "  -g, --geometry WIDTHxHEIGHT  Run SuperTux in given resolution") << "\n"
            << _(     "  -a, --aspect WIDTH:HEIGHT    Run SuperTux with given aspect ratio") << "\n"
            << _(     "  -d, --default                Reset video settings to default values") << "\n"
            << _(     "  --renderer RENDERER          Use sdl, opengl, or auto to render") << "\n" << "\n"
            << _(     "Audio Options:") << "\n"
            << _(     "  --disable-sound              Disable sound effects") << "\n"
            << _(     "  --disable-music              Disable music") << "\n" << "\n"
            << _(     "Game Options:") << "\n"
            << _(     "  --edit-level                 Open given level in editor") << "\n"
            << _(     "  --show-fps                   Display framerate in levels") << "\n"
            << _(     "  --no-show-fps                Do not display framerate in levels") << "\n"
            << _(     "  --show-pos                   Display player's current position") << "\n"
            << _(     "  --no-show-pos                Do not display player's position") << "\n"
            << _(     "  --developer                  Switch on developer feature") << "\n"
            << _(     "  -s, --debug-scripts          Enable script debugger.") << "\n"
            << _(     "  --spawn-pos X,Y              Where in the level to spawn Tux. Only used if level is specified.") << "\n" << "\n"
            << _(     "Demo Recording Options:") << "\n"
            << _(     "  --record-demo FILE LEVEL     Record a demo to FILE") << "\n"
            << _(     "  --play-demo FILE LEVEL       Play a recorded demo") << "\n" << "\n"
            << _(     "Directory Options:") << "\n"
            << _(     "  --datadir DIR                Set the directory for the games datafiles") << "\n"
            << _(     "  --userdir DIR                Set the directory for user data (savegames, etc.)") << "\n" << "\n"
            << _(     "Add-On Options:") << "\n"
            << _(     "  --repository-url URL         Set the URL to the Add-On repository") << "\n" << "\n"
	    << _(     "NEAT options:") << "\n"
	    << _(     "  --neat                       Activate NEAT") << "\n"
	    << _(     "  --headless                   Start evolution in headless mode") << "\n"
	    << _(     "  --popfile FILE               Load population from file") << "\n"
	    << _(     "  --paramfile FILE             Load MultiNEAT parameters from file") << "\n"
	    << _(     "  --experimentparamfile FILE   Load experiment parameters from file") << "\n"
	    << _(     "  --sensorgridsize SIZE        Sets the sensor grid size to SIZE") << "\n"
	    << _(     "  --sensorgridpadding PADDING  Sets the sensor grid padding to PADDING") << "\n"
	    << _(     "  --viewgenome ID              View genome with ID. No evolution, just playback. Needs a proper population file") << "\n" << "\n"
	    << _(     "Environment variables:") << "\n"
            << _(     "  SUPERTUX2_USER_DIR           Directory for user data (savegames, etc.)" ) << "\n"
            << _(     "  SUPERTUX2_DATA_DIR           Directory for the games datafiles" ) << "\n"<< "\n"



            << std::flush;
}

void
CommandLineArguments::print_version() const
{
  std::cout << PACKAGE_NAME << " " << PACKAGE_VERSION << std::endl;
}

void
CommandLineArguments::parse_args(int argc, char** argv)
{
  bool visualmode = false;
  bool view_genome = false;
  
  for(int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];

    if (arg == "--version" || arg == "-v")
    {
      m_action = PRINT_VERSION;

    }
    else if (arg == "--help" || arg == "-h")
    {
      m_action = PRINT_HELP;
    }
    else if (arg == "--print-datadir")
    {
      m_action = PRINT_DATADIR;
    }
    else if (arg == "--debug")
    {
      m_log_level = LOG_DEBUG;
    }
    else if (arg == "--verbose")
    {
      if (m_log_level < LOG_INFO)
      {
        m_log_level = LOG_INFO;
      }
    }
    else if (arg == "--datadir")
    {
      if (i + 1 >= argc)
      {
        throw std::runtime_error("Need to specify a directory for --datadir");
      }
      else
      {
        datadir = argv[++i];
      }
    }
    else if (arg == "--userdir")
    {
      if (i + 1 >= argc)
      {
        throw std::runtime_error("Need to specify a directory for --userdir");
      }
      else
      {
        userdir = argv[++i];
      }
    }
    else if (arg == "--fullscreen" || arg == "-f")
    {
      use_fullscreen = true;
    }
    else if (arg == "--default" || arg == "-d")
    {
      use_fullscreen = false;

      window_size = Size(1280, 800);
      fullscreen_size = Size(1280, 800);
      fullscreen_refresh_rate = 0;
      aspect_size = Size(0, 0);  // auto detect
    }
    else if (arg == "--window" || arg == "-w")
    {
      use_fullscreen = false;
    }
    else if (arg == "--geometry" || arg == "-g")
    {
      i += 1;
      if (i >= argc)
      {
        throw std::runtime_error("Need to specify a size (WIDTHxHEIGHT) for geometry argument");
      }
      else
      {
        int width, height;
        if (sscanf(argv[i], "%9dx%9d", &width, &height) != 2)
        {
          throw std::runtime_error("Invalid geometry spec, should be WIDTHxHEIGHT");
        }
        else
        {
          window_size     = Size(width, height);
          fullscreen_size = Size(width, height);
          fullscreen_refresh_rate = 0;
        }
      }
    }
    else if (arg == "--aspect" || arg == "-a")
    {
      i += 1;
      if (i >= argc)
      {
        throw std::runtime_error("Need to specify a ratio (WIDTH:HEIGHT) for aspect ratio");
      }
      else
      {
        int aspect_width  = 0;
        int aspect_height = 0;
        if (strcmp(argv[i], "auto") == 0)
        {
          aspect_width  = 0;
          aspect_height = 0;
        }
        else if (sscanf(argv[i], "%9d:%9d", &aspect_width, &aspect_height) != 2)
        {
          throw std::runtime_error("Invalid aspect spec, should be WIDTH:HEIGHT or auto");
        }
        else
        {
          float aspect_ratio = static_cast<float>(aspect_width) / static_cast<float>(aspect_height);

          // use aspect ratio to calculate logical resolution
          if (aspect_ratio > 1) {
            aspect_size = Size(static_cast<int>(600 * aspect_ratio + 0.5),
                                         600);
          } else {
            aspect_size = Size(600,
                                         static_cast<int>(600 * 1/aspect_ratio + 0.5));
          }
        }
      }
    }
    else if (arg == "--renderer")
    {
      i += 1;
      if (i >= argc)
      {
        throw std::runtime_error("Need to specify a renderer for renderer argument");
      }
      else
      {
        video = VideoSystem::get_video_system(argv[i]);
      }
    }
    else if (arg == "--show-fps")
    {
      show_fps = true;
    }
    else if (arg == "--no-show-fps")
    {
      show_fps = false;
    }
    else if (arg == "--show-pos")
    {
      show_player_pos = true;
    }
    else if (arg == "--no-show-pos")
    {
      show_player_pos = false;
    }
    else if (arg == "--developer")
    {
      developer_mode = true;
    }
    else if (arg == "--christmas")
    {
      christmas_mode = true;
    }
    else if (arg == "--no-christmas")
    {
      christmas_mode = false;
    }
    else if (arg == "--disable-sound" || arg == "--disable-sfx")
    {
      sound_enabled = false;
    }
    else if (arg == "--disable-music")
    {
      music_enabled = false;
    }
    else if (arg == "--play-demo")
    {
      if (i + 1 >= argc)
      {
        throw std::runtime_error("Need to specify a demo filename");
      }
      else
      {
        start_demo = argv[++i];
      }
    }
    else if (arg == "--record-demo")
    {
      if (i + 1 >= argc)
      {
        throw std::runtime_error("Need to specify a demo filename");
      }
      else
      {
        record_demo = argv[++i];
      }
    }
    else if (arg == "--spawn-pos") 
    {
      Vector spawn_pos;
      
      if (!start_level)
        throw std::runtime_error("--spawn-pos can only be used when a levelfile is specified.");
      
      if (++i >= argc)
        throw std::runtime_error("Need to specify a spawn-pos X,Y");
      else
      {
        int x, y;
        if (sscanf(argv[i], "%9d,%9d", &x, &y) != 2)
          throw std::runtime_error("Invalid spawn-pos, should be X,Y");
        spawn_pos.x = x;
        spawn_pos.y = y;
      }
      
      tux_spawn_pos = spawn_pos;
    }
    else if (arg == "--debug-scripts" || arg == "-s")
    {
      enable_script_debugger = true;
    }
    else if (arg == "--repository-url")
    {
      if (i + 1 >= argc)
      {
        throw std::runtime_error("Need to specify a repository URL");
      }
      else
      {
        repository_url = argv[++i];
      }
    }
    else if (arg == "--edit-level")
    {
      if (i + 1 >= argc)
      {
        throw std::runtime_error("Need to specify a level for --edit-level");
      }
      else
      {
        edit_level = argv[++i];
      }
    }
    else if (arg == "--neat")
    {
      m_action = START_EVOLUTION;
      
      if (i + 1 < argc && argv[i + 1][0] != '-') {
	start_level = argv[++i];
      }
      else
      {
	start_level = "../data/levels/world1/01 - Welcome to Antarctica.stl";
      } 
    }
    else if (arg == "--visualmode")
    {
      if (m_action != START_EVOLUTION) 
      {
	throw std::runtime_error("Need to specify NEAT usage before setting headless mode");
      }
      
      m_action = START_EVOLUTION;
      visualmode = true;
    }
    else if (arg == "--sensorgridsize")
    {
      if (m_action != START_EVOLUTION) 
      {
	throw std::runtime_error("Need to specify NEAT usage before setting the sensor grid size");
      }
      
      /*if (strcmp(TuxEvolution::filename, "") != 0) 
      {
	throw std::runtime_error("Cannot specify the sensor grid size when loading population from file");
      }*/
      
      int sensor_grid_size = 0;
      if (i + 1 < argc && sscanf(argv[i + 1], "%d", &sensor_grid_size) == 1) 
      {
	TuxEvolution::custom_sensor_grid = true;
	TuxEvolution::sensor_grid_size = sensor_grid_size;
	++i;
      } 
      else 
      {
	throw std::runtime_error("Please specify a seed integer after using --sensorgridsize");
      }
    }
    else if (arg == "--sensorgridpadding")
    {
      if (m_action != START_EVOLUTION) 
      {
	throw std::runtime_error("Need to specify NEAT usage before setting the sensor grid padding");
      }
      
      /*if (strcmp(TuxEvolution::filename, "") != 0) 
      {
	throw std::runtime_error("Cannot specify the sensor grid padding when loading population from file");
      }*/
      
      int sensor_grid_padding = 0;
      if (i + 1 < argc && sscanf(argv[i + 1], "%d", &sensor_grid_padding) == 1) 
      {
	TuxEvolution::custom_sensor_grid = true;
	TuxEvolution::sensor_grid_padding = sensor_grid_padding;
	++i;
      } 
      else 
      {
	throw std::runtime_error("Please specify a seed integer after using --sensorgridpadding");
      }
    }
    else if (arg == "--popfile")
    {
      if (m_action != START_EVOLUTION) 
      {
	throw std::runtime_error("Need to specify NEAT usage before specifying a population filename");
      }      
      
      if (i + 1 >= argc || argv[i + 1][0] == '-') 
      {
	throw std::runtime_error("Need to specify a population file after using --filename");
      }
      else
      {
	TuxEvolution::filename = argv[++i];
      }
    }
    else if (arg == "--paramfile")
    {
      if (m_action != START_EVOLUTION) 
      {
	throw std::runtime_error("Need to specify NEAT usage before specifying a parameter filename");
      }
      
      if (i + 1 >= argc || argv[i + 1][0] == '-') 
      {
	throw std::runtime_error("Need to specify a parameter file after using --paramfile");
      }
      else
      {
	TuxEvolution::paramfilename = argv[++i];
      }
    }
    else if (arg == "--experimentparamfile")
    {
      if (m_action != START_EVOLUTION) 
      {
	throw std::runtime_error("Need to specify NEAT usage before specifying an experiment parameter filename");
      }
      
      if (i + 1 >= argc || argv[i + 1][0] == '-') 
      {
	throw std::runtime_error("Need to specify an experiment parameter file after using --experimentparamfile");
      }
      else
      {
	Config::neat_experimentparamfile = argv[++i];
      }
    }
    else if (arg == "--viewgenome")
    {
      if (m_action != START_EVOLUTION) {
	throw std::runtime_error("Need to specify NEAT usage before viewing single genomes");
      }
      
      if (strcmp(TuxEvolution::filename, "") == 0) 
      {
	throw std::runtime_error("Cannot specify a genome to view without loading population from file");
      }
      
      view_genome = true;
      
      int genomeid = -1;
      if (i + 1 < argc && sscanf(argv[i + 1], "%d", &genomeid) == 1) 
      {
	if (genomeid >= 0) 
	{
	  TuxEvolution::viewing_mode = true;
	  TuxEvolution::view_genome_id = genomeid;
	  ++i;
	}
	else
	{
	  throw std::runtime_error("Genome ID must be >= 0");
	}
      }
    }
    else if (arg == "--fromtogenome")
    {
      if (m_action != START_EVOLUTION) {
	throw std::runtime_error("Need to specify NEAT usage before setting genomes to evaluate");
      }
      
      if (strcmp(TuxEvolution::filename, "") == 0) 
      {
	throw std::runtime_error("Cannot specify genomes to evaluate without loading population from file");
      }
      
      int fromgenomeid = -1;
      int togenomeid = -1;
      if ((i + 1 < argc && sscanf(argv[i + 1], "%d", &fromgenomeid) == 1) && (i + 2 < argc && sscanf(argv[i + 2], "%d", &togenomeid) == 1))
      {
	if (fromgenomeid >= 0 && togenomeid >= 0) 
	{
	  TuxEvolution::from_genome_id = fromgenomeid;
	  TuxEvolution::to_genome_id = togenomeid;
	}
	else
	{
	  throw std::runtime_error("Genome ID must be >= 0");
	}
      }
      i += 2;
    }
    else if (arg == "--dbfile")
    {
      if (m_action != START_EVOLUTION) {
	throw std::runtime_error("Need to specify NEAT usage before setting dbfile");
      }
      
      if (i + 1 >= argc || argv[i + 1][0] == '-') 
      {
	throw std::runtime_error("Need to specify a database file after using --dbfile");
      }
      else
      {
	TuxEvolution::dbfile = argv[++i];
      }
    }
    else if (arg == "--curgen")
    {
      if (m_action != START_EVOLUTION) {
	throw std::runtime_error("Need to specify NEAT usage before setting current generation");
      }
      
      int curgen = -1;
      
      if ((i + 1 < argc && sscanf(argv[i + 1], "%d", &curgen) == 1))
      {
	if (curgen >= 0) 
	{
	  TuxEvolution::current_gen = curgen;
	}
	else
	{
	  throw std::runtime_error("Current generation must be >= 0");
	}
      }
    }
    else if (arg == "--hyperneat")
    {
      if (m_action != START_EVOLUTION) {
	throw std::runtime_error("Need to specify NEAT usage before setting hyperneat usage");
      }
      
      TuxEvolution::hyperneat = true;
    }
    else if (arg == "--drawsensors")
    {
      Sensor::DRAW_SENSORS = true;
    }
    else if (arg[0] != '-')
    {
      start_level = arg;
    }
    else
    {
      throw std::runtime_error((boost::format("Unknown option '%1%''. Use --help to see a list of options") % arg).str());
    }
  }
  
  if (m_action != START_EVOLUTION)
      Config::neat_headless_mode = false;
  else if (!visualmode && !view_genome)
      Config::neat_headless_mode = true;
}

void
CommandLineArguments::merge_into(Config& config)
{
#define merge_option(x) if (x) { config.x = *x; }

  merge_option(fullscreen_size);
  merge_option(fullscreen_refresh_rate);
  merge_option(window_size);
  merge_option(aspect_size);
  merge_option(use_fullscreen);
  if (!Config::neat_headless_mode)
    merge_option(video);
  merge_option(show_fps);
  merge_option(show_player_pos);
  merge_option(sound_enabled);
  merge_option(music_enabled);
  merge_option(start_level);
  merge_option(enable_script_debugger);
  merge_option(start_demo);
  merge_option(record_demo);
  merge_option(tux_spawn_pos);
  merge_option(developer_mode);
  merge_option(christmas_mode);
  merge_option(repository_url);
  merge_option(edit_level);

#undef merge_option
}

/* EOF */
