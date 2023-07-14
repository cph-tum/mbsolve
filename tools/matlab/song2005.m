% song2005  Displays result of song2005 setup.
%           Reference data available in tools/reference-data.

% mbsolve: An open-source solver tool for the Maxwell-Bloch equations.
%
% Copyright (c) 2016, Computational Photonics Group, Technical University
% of Munich.
%
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 3 of the License, or
% (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program; if not, write to the Free Software Foundation,
% Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

clear all;
close all;

%  choose hdf5 file
[filename, folder] = uigetfile('../../*.hdf', 'Select result data');
f = fullfile(folder, filename);
if (filename == 0)
   return;
end

[filename, folder] = uigetfile('../reference-data/*.csv', ...
    'Select compare trace');
use_reference = 0;
if (filename ~= 0)
    comp_data = csvread(fullfile(folder, filename));
    use_reference = 1;
end

% read setup and device attributes
d_x_DFG = h5readatt(f_DFG, '/setup/scenario', 'gridpoint_size');
d_t_DFG = h5readatt(f_DFG, '/setup/scenario', 'timestep_size');
t_e_DFG = h5readatt(f_DFG, '/setup/scenario', 'sim_endtime');
L_x_DFG = h5readatt(f_DFG, '/setup/device', 'dev_length');

% time grid
t = 0:d_t:t_e;

% data
e = h5read(f, '/e/real');
rho11 = h5read(f, '/d11/real');
rho22 = h5read(f, '/d22/real');
rho33 = h5read(f, '/d33/real');

figure;
plot(t, e);
xlabel('Time/s');
ylabel('E-Field/Vm^{-1}');
xlim([0, t_e]);

figure;
plot(t, rho11);
hold on;
plot(t, rho22);
plot(t, rho33);
if (use_reference == 1)
    plot(comp_data(:, 1) * 1e-15, comp_data(:, 2));
end
xlabel('Time/s');
ylabel('Populations');
xlim([0, t_e]);
%ylim([0, 1]);
