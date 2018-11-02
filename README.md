# SuperTux + NEAT
Creating a neural agent for the SuperTux Jump'n'Run game using the NEAT algorithm as a project for my master thesis. To accomplish this, the MultiNEAT implementation by Peter Chervenski and Shane Ryan is used (http://multineat.com/, Code at https://github.com/peter-ch/MultiNEAT/tree/master/src).
To start the evolutionary process, compile SuperTux as usual (check the SuperTux repo for info) and execute SuperTux with the '--neat' argument.

I added several additional console arguments for customization. Just use "./supertux2 --help" after building to display all available options.

Note that this project needs sqlite3.

NOTE: Without specifying "--autosavegen", no output files will be produced and all progress will be lost if you terminate the program!
The console arguments that are NOT displayed when starting the program with the "--help" option are not made for humans and should primarily be used by the "stneatexperiment" program (see other repo) or for debugging purposes.

Features:
- NEAT experiments in SuperTux
- You can watch the training visually or train headless for faster training times
- Autosave every nth generation
- Watch pretrained genomes from files
- Select training levels or build your own one (this is originally a SuperTux feature)
- Specify sensor parameters
- Create own experiment parameter files for sensor and experiment configuration
- HyperNEAT support

Currently known bugs:
- When viewing a network that finishes the level, the next replay will be very slow and the net won't act like it did the first time

# SuperTux

[![Build Status](https://travis-ci.org/SuperTux/supertux.svg?branch=master)](https://travis-ci.org/SuperTux/supertux)
[![AppVeyor Build Satus](https://ci.appveyor.com/api/projects/status/github/SuperTux/supertux?svg=true&branch=master)](https://ci.appveyor.com/project/supertux/supertux-9ml4d/branch/master)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/4025/badge.svg)](https://scan.coverity.com/projects/4025)
[![Github All Releases](https://img.shields.io/github/downloads/supertux/supertux/total.svg?maxAge=2592000)](https://github.com/SuperTux/supertux)
[![#supertux on freenode](https://img.shields.io/badge/freenode-%23supertux-brightgreen.svg)](https://webchat.freenode.net/?channels=supertux)

## Description

SuperTux is a jump'n'run game with strong inspiration from the
Super Mario Bros. games for the various Nintendo platforms.

Run and jump through multiple worlds, fighting off enemies by jumping
on them, bumping them from below or tossing objects at them, grabbing
power-ups and other stuff on the way.


## Story: Penny gets captured!

Tux and Penny were out having a nice picnic on the ice fields of
Antarctica. Suddenly, a creature jumped from behind an ice bush, there
was a flash, and Tux fell asleep!

When Tux wakes up, he finds that Penny is missing. Where she lay
before now lies a letter. "Tux, my arch enemy!" says the letter. "I
have captured your beautiful Penny and have taken her to my fortress.
The path to my fortress is littered with my minions. Give up on the
thought of trying to reclaim her, you haven't got a chance! -Nolok"

Tux looks and sees Nolok's fortress in the distance. Determined to
save his beloved Penny, he begins his journey.


## Documentation

Important documentation for SuperTux is contained in multiple files.
Please see them:

* `INSTALL.md` - Requirements, compiling and installing.
* `README.md` - This file
* `NEWS.md` - Changes since the previous versions of SuperTux.
* `LICENSE.txt` - The GNU General Public License, under whose terms SuperTux is
licensed. (Most of the data subdirectory is also licensed under
CC-by-SA)
* `docs/levelguidelines.txt` - Very useful information for those that want to
design levels for SuperTux.
* `data/credits.stxt` - Credits for people that contributed to the creation of
SuperTux. (You can view these in the game menu as well.)


## Running the game

SuperTux makes use of proc to see where it is. In other words, it does
not have any need to be installed and can be run from anywhere. This
makes it possible to click in the executable in your filemanager (i.e.
Konqueror or Nautilus) as opposed to many other Linux games.

Options can be reached from the menu, so you don't need to specify
arguments, but if you want, type `supertux2 --help` to check the ones
that are available. Also, notice that SuperTux saves the options, so
it's often enough to specify them once. For example, fullscreen mode
causes problems on some setups, so just run `supertux2 --window` and
you should be set.

The game uses OpenGL to render the graphics. You will either need a
CPU with about 1 GHz or an accelerated video card with recent
graphics drivers.


## Playing the game

Both keyboards and joysticks/gamepads are supported. You can change
the controls via the Options menu. Basically, the only keys you will
need to use in-game are to do the following actions: jump, duck,
right, left, action and 'P' to pause/unpause the game. There isn't much
to tell about the first few, but the "action" key allows you to pick
up objects and use any powerup you got. For instance, with the fire
flower, you can shoot fireballs, or with the ice flower fire ice pellets.

Other useful keys include the Esc key, which is used to go to the menu
or to go up a level in the menu. The menu can be navigated using the
arrow keys or the mouse.

In the worldmap, the arrow keys are used to navigate and Enter to
enter the current level.


## Development status

With the release of SuperTux 0.4.0 (December 2015), we wanted to provide a
fairly stable release. The release of SuperTux 0.5.0 should have fixed some
stability issues from 0.4.0 and introduced a new level editor. Development of
the game will continue (of course), now working towards more releases.
Most notably, development focus will be shifted on providing a better gameplay
experience.

The Forest World (you can check it out: Start Game > Contrib Levels > Forest
World) is relatively unstable, which is why it resides only in the contrib
levels menu. We (SuperTux Team) intend to continue with the development of the
Forest World, but decided that in order to allow more access to the most recent
version (in repositories etc.) we would have to release Milestone 2 without the
Forest World included in the Story Mode. Constructive feedback with regards to
the Forest World is welcome.

Don't forget that you can get involved with the development at
<https://github.com/supertux/supertux>,
or get notified about the recent changes on Twitter
[@supertux_team](https://twitter.com/supertux_team)


## The End

Thanks for playing SuperTux.

The SuperTux Team
