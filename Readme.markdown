OBJ-MAGIC [![Build Status](https://travis-ci.org/tapio/obj-magic.svg)](https://travis-ci.org/tapio/obj-magic)
=========

## Introduction ##

This is a command line tool for manipulating Wavefront OBJ 3d meshes.
The idea is that when processing third-party models for use,
I want to quickly scale them to the correct scale for me, center them etc.
It's much faster to issue a couple of commands than to fire up a 3d modelling program,
try to remember how it worked, where the functions I need are and finally hope
it won't break the normals or other things when exporting.
Of course you might feel differently.

Non-exhaustive list of features include:

* Scaling
* Mirroring
* Translating
* Centering
* Etc...

Most transformations can be done to each axis x/y/z separately or together.
Doing multiple different operations at the same time is also possible,
but some combinations may result in funky behaviour, so be careful.
This should be improved in the future.


## Usage ##

Run:

	./obj-magic --help
	

## Dependencies ##

Coded using C++. No external dependencies. GLM math library is included within sources.


## Compiling ##

Run:

	./make.sh

If it doesn't work for you, just compile all .cpp files in the `src/` directory into an executable using your preferred method / compiler.


## Design Goals ##

* Doesn't modify lines unless necessary
	* Version control system friendliness
	* Preserving normals seems to be hard with most 3d modelling softwares
* Efficiency
	* No intermediate form, no unnecessary parsing
	* Many operations can be done in one simple pass
	* --> Results in quick operation and very low memory usage


## License ##

     Copyright (C) 2012  Tapio Vierros

      This program is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <http://www.gnu.org/licenses/>.

