#!/usr/bin/env python
# -*- coding: utf-8 -*-

""" andreasen2008.py: Runs andreasen2008 thermal noise in open cavity setup."""

# mbsolve: An open-source solver tool for the Maxwell-Bloch equations.
#
# Copyright (c) 2016, Computational Photonics Group, Technical University
# of Munich.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

import mbsolve.lib as mb
import mbsolve.solvercpu
import mbsolve.writerhdf5

import math
import time
import sys

# vacuum
mat_vac = mb.material("Vacuum")
mb.material.add_to_library(mat_vac)

# Andreasen 2008 setup
dev = mb.device("Andreasen")
num_timesteps = 1000000
# Courant number
S = 1
# Delta x
gridpoint_size = 1e-9
c0 = 1 / math.sqrt(mb.MU0 *mb.EPS0)
timestep_size = S * gridpoint_size / c0
length = 20e-9
num_gridpoints = math.ceil(length / gridpoint_size) + 1

dev.add_region(mb.region("Vacuum left", mat_vac, 0, length))

# scenario
ic_d = mb.ic_density_random_2lvl(3e4)
ic_e = mb.ic_field_const(0.0)
tau_sim = num_timesteps * timestep_size
sce = mb.scenario("Basic", num_gridpoints, tau_sim, ic_d, ic_e)
sce.add_record(mb.record("e_left", 0.0, 0.0))
sce.add_record(mb.record("e_middle", 0.0, length/2))
sce.add_record(mb.record("e_right", 0.0, length))
sce.set_courant_number(S)

# add source
sce.add_source(mb.thermal_noise("noise_left", 0.0, mb.source.hard_source, 30000,
                             tau_sim, 1e12/(2*math.pi), 2.5e16/(2*math.pi),2e15/(2*math.pi)))
sce.add_source(mb.thermal_noise("noise_right", length, mb.source.hard_source, 30000,
                             tau_sim, 1e12/(2*math.pi), 2.5e16/(2*math.pi),2e15/(2*math.pi)))
# run solver
sol = mb.solver.create_instance("cpu-fdtd-noop", dev, sce)
print('Solver ' + sol.get_name() + ' started')
tic = time.time()
sol.run()
toc = time.time()
print('Solver ' + sol.get_name() + ' finished in ' + str(toc - tic) + ' sec')

# write results
wri = mb.writer.create_instance("hdf5")
outfile = dev.get_name() + "_" + sce.get_name() + "." + wri.get_extension()
results = sol.get_results()
wri.write(outfile, sol.get_results(), dev, sce)
