#!/usr/bin/env python
# -*- coding: utf-8 -*-

""" andreasen2009.py: Runs andreasen2009 two-level SF setup."""

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

decoherence_rate = float(sys.argv[1])
c_num = float(sys.argv[2])
print('Decoherence rate: ' + sys.argv[1] + ' s^-1')
print('Courant number: ' + str(c_num))

# vacuum
mat_vac = mb.material("Vacuum")
mb.material.add_to_library(mat_vac)

# Andreasen active region material
qm = mb.qm_desc_2lvl(8.53e18, 3e4, 2 * math.pi * 4.77e14, 6.875e-11, 1.32e7, decoherence_rate, -1.0)
mat_ar = mb.material("AR_Andreasen", qm)
mb.material.add_to_library(mat_ar)

# Ziolkowski setup
dev = mb.device("Andreasen")
dev.add_region(mb.region("Vacuum left", mat_vac, 0, 70e-6))
dev.add_region(mb.region("Active region", mat_ar, 70e-6, 7.07e-3))
dev.add_region(mb.region("Vacuum right", mat_vac, 7.07e-3, 7.14e-3))
bc = mb.bc_UPML(200)
dev.set_bc_field(bc, bc)

# scenario
ic_d = mb.ic_density_random_2lvl(3e4)
ic_e = mb.ic_field_const(0.0)
sce = mb.scenario("Basic", 102000, 5e-10, ic_d, ic_e)
sce.add_record(mb.record("inv12", 1e-13))
sce.add_record(mb.record("bloch", 1e-13))
sce.add_record(mb.record("e", 1e-13))
sce.set_courant_number(c_num)

# run solver
sol = mb.solver.create_instance("cpu-fdtd-2lvl-cvr-rodr", dev, sce)
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
