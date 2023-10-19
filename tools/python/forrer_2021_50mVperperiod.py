#!/usr/bin/env python
# -*- coding: utf-8 -*-

""" forrer_2021_50mVperperiod.py: Runs forrer2021 harmonic comb THz QCL noise setup."""

# mbsolve: An open-source solver tool for the Maxwell-Bloch equations.
#
# Copyright (c) 2023, Computational Photonics Group, Technical University
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

# Hamiltonian
energies = [ 0.0097 * mb.E0, 0.0082 * mb.E0, -0.0047 * mb.E0, 
-0.0083 * mb.E0, -0.0097 * mb.E0 ]
off_diagonales = [ 0.0005 * mb.E0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ]
H = mb.qm_operator(energies, off_diagonales)

# dipole moment operator
off_dipoles = [ 0, -2.9500e-09 * mb.E0, 0, 0, 0, 0, 0, 0, 0, 0 ]
diag_dipoles = [ 0, 0, 0, 0, 0 ]
u = mb.qm_operator(diag_dipoles, off_dipoles)

# relaxation superoperator
# scattering rate matrix R
rates = [ [ 0, 1.8815e+09, 2.1290e+10, 4.0984e+09, 5.6000e+09 ],
          [ 3.5006e+09, 0, 3.2437e+08, 2.2854e+10, 2.0029e+12 ],
          [ 6.5578e+10, 6.2829e+08, 0, 8.0333e+11, 6.1577e+09 ],
          [ 6.8416e+09, 3.6845e+08, 6.6107e+11, 0, 4.7378e+12 ],
          [ 5.2192e+08, 6.7259e+10, 4.7554e+09, 4.7726e+12, 0 ] ]

# pure dephasing rates
pure_deph = [ 3.5857e+12, 9.3257e+11, 0, 0, 0, 0, 0, 0, 0, 0 ]
relax_sop = mb.qm_lindblad_relaxation(rates, pure_deph)

# initial density matrix 
rho_init = mb.qm_operator([ 0.3705, 0.4937, 0.0741, 0.0333, 0.0285])

h = 15e-6
w = 60e-6
A = h*w
N = 6.35e21
N_x = 2000
l_device = 4e-3
d_x = l_device / N_x
f_noise = 1
N_cell = N * A * d_x * f_noise

qm = mb.qm_description(N, N_cell, H, u, relax_sop)
loss = 760
mat_ar = mb.material("AR_Forrer", qm, 12.96, 1, loss, 1.0)
mb.material.add_to_library(mat_ar)

dev = mb.device("5lvl")

dev.add_region(mb.region("Active region", mat_ar, 0.0, l_device))

# Scenario
ic_d = mb.ic_density_const(rho_init)
ic_e = mb.ic_field_const(0.0)
sce = mb.scenario("hc_noise_forrer2021", N_x, 2200e-9,  ic_d, ic_e, ic_e)
sce.add_record(mb.record("e1",0,4e-3))

# run solver
sol = mb.solver.create_instance("cpu-fdtd-5lvl-reg-cayley-qnoise", dev, sce)
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
outfile_autosave = dev.get_name() + "_" + sce.get_name() + \
    "_autosave." + wri.get_extension()
sim_data = sol.get_sim_data()
wri.autosave(outfile_autosave, sim_data, dev, sce)
