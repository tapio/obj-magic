OBJ-MAGIC
=========

## Introduction ##

This is a command line tool for manipulating Wavefront OBJ 3d meshes.
Non-exhaustive list of features include:

* Scaling
* Mirroring
* Translating
* Centering
* Etc...
* More to come

Most transformations can be done to each axis x/y/z separately or together.
Doing multiple different operations at the same time is also possible.


## Usage ##

Run:

	./obj-magic --help
	

## Dependencies ##

Coded using C++. No external dependencies. GLM math library is included within sources.


## Compiling ##

Run:

	./make.sh

If it doesn't work for you, just compile all .cpp files in the root directory into an executable using your preferred method / compiler.


## Design Goals ##

* Doesn't modify lines unless necessary
	* Version control system friendliness
	* Preserving normals seems to be hard with most 3d modelling softwares
* Effiency
	* No intermediate form, no unnecessary parsing
	* Many operations can be done in one simple pass
	* --> Results in quick operation and very low memory usage


## Known Issues ##

* Poorly tested, no test suite. This thing is under development, so improvements are coming
* Does not care about separate objects in one .obj file


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

