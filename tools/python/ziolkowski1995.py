#!/usr/bin/env python
# -*- coding: utf-8 -*-

""" ziolkowski1995.py: Runs ziolkowski1995 two-level SIT setup."""

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
import mbsolve.readerhdf5


import math
import time

# vacuum
mat_vac = mb.material("Vacuum")
mb.material.add_to_library(mat_vac)

# Ziolkowski active region material
qm = mb.qm_desc_2lvl(1e24, 0, 2 * math.pi * 2e14, 6.24e-11, 1.0e10, 1.0e10, -1.0)
mat_ar = mb.material("AR_Ziolkowski", qm)
mb.material.add_to_library(mat_ar)

# Ziolkowski setup
dev = mb.device("Ziolkowski")
dev.add_region(mb.region("Vacuum left", mat_vac, 0, 7.5e-6))
dev.add_region(mb.region("Active region", mat_ar, 7.5e-6, 142.5e-6))
dev.add_region(mb.region("Vacuum right", mat_vac, 142.5e-6, 150e-6))

# initialize for new simulation
rho_init = mb.qm_operator([ 1, 0 ])
ic_d = mb.ic_density_const(rho_init)
ic_e = mb.ic_field_const(0.0)
# scenario
sce = mb.scenario("Basic", 32768, 200e-15, ic_d, ic_e)
sce.add_record(mb.record("inv12", 2.5e-15))
sce.add_record(mb.record("e", 2.5e-15))
# add source
sce.add_source(mb.sech_pulse("sech", 0.0, mb.source.hard_source, 4.2186e9,
                             2e14, 10, 2e14))

# restart with initialization from autosaved simulation data
# filename_autosave = "Ziolkowski_Basic_autosave.hdf" # Please choose the right autosave file to restart simulation
# read = mb.reader.create_instance("hdf5")
# rho_init = read.read_density(filename_autosave)
# ic_d = mb.ic_density_autosave(rho_init)
# ic_e = mb.ic_field_autosave(read.read_field(filename_autosave, "e"))
# ic_h = mb.ic_field_autosave(read.read_field(filename_autosave, "h"))
# ic_p = mb.ic_field_autosave(read.read_field(filename_autosave, "p"))
# # scenario
# sce = mb.scenario("Basic", 32768, 200e-15, ic_d, ic_e, ic_h, ic_p)
# sce.add_record(mb.record("inv12", 2.5e-15))
# sce.add_record(mb.record("e", 2.5e-15))

# run solver
sol = mb.solver.create_instance("cpu-fdtd-red-2lvl-cvr-rodr", dev, sce)
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
outfile_autosave = dev.get_name() + "_" + sce.get_name() + "_autosave." + wri.get_extension()
sim_data = sol.get_sim_data()
wri.autosave(outfile_autosave, sim_data, dev, sce)