% andreasen2009  Displays result of andreasen2009 setup.

% mbsolve: An open-source solver tool for the Maxwell-Bloch equations.
%
% Copyright (c) 2023, Computational Photonics Group, Technical University
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
[filename, folder] = uigetfile('*.hdf', 'Select result data');
f = fullfile(folder, filename);
if (filename == 0)
    return;
end

% read global attributes
d_x = h5readatt(f, '/setup/scenario', 'gridpoint_size');
d_t = h5readatt(f, '/setup/scenario', 'timestep_size');
t_e = h5readatt(f, '/setup/scenario', 'sim_endtime');
L_x = h5readatt(f, '/setup/device', 'dev_length');

% data
e = h5read(f, '/e/real');
bloch = h5read(f, '/bloch/real');
inv12 = h5read(f, '/inv12/real');

% grid
dt_result = t_e/(size(e,2) - 1);
t = 0:dt_result:t_e;

%% detailed time trace, see Andreasen et al., Fig. 1 and 3
papersize = [ 15 12 ];
fig = figure('units', 'centimeters', 'DefaultAxesFontSize',12);
pos = get(gcf, 'pos');
subplot(2,1,1);
plot(t/1e-9, mean(e(:, :).^2),  ...
    'DisplayName', 'Electric field', 'Color', [0, 101, 189]/255);
ylabel('Normalized Amplitude')
legend('show', 'Location', 'northeast');
subplot(2,1,2);
plot(t/1e-9, mean(inv12(1001:101000, :))./mean(bloch(1001:101000,:)), ...
    'DisplayName', 'Population inversion', 'Color', [227, 114, 34]/255);
xlabel('Simulation time /ns');
ylabel('\rho_3/\rho_B /ns');
grid on;
legend('show', 'Location', 'northeast');
set(fig, 'PaperPositionMode', 'Auto', 'PaperUnits', 'Centimeters', ...
    'PaperSize', papersize);
print(fig, 'andreasen2009.pdf', '-dpdf', '-fillpage');
%%
I_mean = mean(e(:, :).^2);
bloch_mean = mean(inv12(1001:101000, :))./mean(bloch(1001:101000,:));
T = array2table([t'/1e-9 I_mean' bloch_mean']);
header = {'time', 'intensity', 'bloch'};
T.Properties.VariableNames(1:size(header,2)) = header;
writetable(T,append('andreasen2009.csv'));