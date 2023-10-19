% noise_hc_forrer2021  Displays result of noise_hc_forrer2021 setup.
%                 

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

% %  choose hdf5 file
[filename, folder] = uigetfile('*.hdf', ...
    'Select result data');
f = fullfile(folder, filename);

% read global attributes
d_x = h5readatt(f, '/setup/scenario', 'gridpoint_size');
d_t = h5readatt(f, '/setup/scenario', 'timestep_size');
t_e = h5readatt(f, '/setup/scenario', 'sim_endtime');
L_x = h5readatt(f, '/setup/device', 'dev_length');

% complete grid
x = 0:d_x:L_x;
t_full = 0:d_t:t_e;

% read simulation data
e = h5read(f, '/e1/real');

%% Analyse
% Define processing parameters
f_range = [3.3e12 3.7e12];
f_range_RIN = [1e6 1e10];
f_range_BN = [1e6 65e9];
fs = 1/d_t;
t_rt = 2*L_x / 3e8 * 3.15; 
num_modes = 6;
% Define time interval for Fourier analyse
t_offset = 0.2e-6; ;
t_step = t_e - t_offset;

start = t_offset;
stop = t_offset + t_step;
interval = and(t_full > start, t_full < stop);
t_int = t_full(interval);
e_int = e(interval);

% Calculate instantanteous frequency
signal_anal = hilbert((e_int));
instfrq= fs/(2*pi)*diff(unwrap(angle(signal_anal)));
interval_rt = and(t_int > t_int(end) - 3.2 * t_rt, t_int < t_int(end));
t_rt_end = t_int(interval_rt(2:end));
t_rt_end = t_rt_end - t_rt_end(1);
instfrq_rt = instfrq(interval_rt(2:end));
figure; plot(t_rt_end(1:end-1) , instfrq_rt(1:end-1));
ylim(f_range);

% Calculate Intensity
I = (e_int).^2;
P_norm_rt = I/max(I);
P_norm_rt = P_norm_rt(interval_rt(2:end));
% Calculate Beatnote
[Y_I, freqs] = calc_spectrum(I, d_t);
ind_BN_red = and(freqs > f_range_BN(1), freqs < f_range_BN(2));
Y_BN_red = Y_I(ind_BN_red);
freqs_BN_red = freqs(ind_BN_red);

figure;
plot(freqs, 10*log10(Y_I));
hold on;
plot(freqs_BN_red, 10*log10(Y_BN_red));
xlim(f_range_BN);
xlabel('Frequency/Hz');
ylabel('Beatnote/a.u.');

% Calculate spectrum
[Y, ~] = calc_spectrum(e_int, d_t);
% Find harmonic comb modes
ind_red = and(freqs > f_range(1), freqs < f_range(2));
freqs_red = freqs(ind_red);
Y_red = Y(ind_red);
ind_locmax = islocalmax(Y_red.^2 / max(Y_red.^2), ...
   'MaxNumExtrema', num_modes);
Y_locmax = Y_red(ind_locmax);
freqs_locmax = freqs_red(ind_locmax);
P_norm = Y.^2/max(Y).^2;
ind_P_red = P_norm > 1e-9;
P_norm_red = P_norm(ind_P_red);
freqs_norm_red = freqs(ind_P_red);
% Plot harmonic comb modes
figure;
semilogy(freqs,P_norm);
hold on; semilogy(freqs_locmax, Y_locmax.^2/max(Y_locmax).^2, 'xr');
xlim(f_range); ylim([2e-4,1]);

% Calculate RIN
mean_I = mean(I);
I_RIN = I - mean_I;
% RIN theory is based on the RP Photonics article: Relative Intensity Noise 
% (https://www.rp-photonics.com/relative_intensity_noise.html)
% Calc power spectral density
[pxx,~]= periodogram(I_RIN,hanning(length(e_int)), ...
     length(e_int), 1/d_t);
RIN = pxx/ mean_I^2;
ind_RIN = and(freqs >= 5e5, freqs <=1e10);
RIN_red = RIN(ind_RIN);
freqs_RIN = freqs(ind_RIN);
% Calculate RIN of individual modes
win = hanning(length(e_int));
for i = 1:num_modes
e_locmax(i,:) = filter_comb_mode(e_int, freqs_locmax(i), 2e10, fs);
I_locmax(i,:) = e_locmax(i,:).^2;
mean_I_loc(i) = mean(I_locmax(i,:));
[pxx_i(i,:),~]= periodogram(I_locmax(i,:) - mean(I_locmax(i,:)), ...
    hanning(length(e_int)), length(e_int), 1/d_t);
RIN_locmax(i,:) = pxx_i(i,:)/ mean_I_loc(i).^2;
end
RIN_locmax_red(:,1:length(RIN_red)) = RIN_locmax(:,ind_RIN);
% Plot RIN
figure;  axes('XScale', 'log', 'YScale', 'log'); hold on; 
loglog(freqs, RIN_locmax(1:end,:));
loglog(freqs, RIN);
xlim(f_range_RIN);
for i = 1:num_modes
    cell_legend{i} = append('mode ', num2str(i));
end
cell_legend{end+1} = 'sum';
legend(cell_legend);
%% Generate simulation data files
% Instantaneous frequency
T_inst_freq = array2table([t_rt_end' * 1e12, ...
    instfrq_rt']);
header_inst_freq = {'time', 'inst_freq'};
T_inst_freq_t.Properties.VariableNames(1:size(header_inst_freq,2)) = ...
    header_inst_freq;
writetable(T_inst_freq, 'hc-noise-forrer2021-inst-freq-time-THz.csv');
% Intensity over roundtrip
T_I_t = array2table([t_rt_end'*1e12, P_norm_rt']);
header_I_t = {'time', 'intensity'};
T_I_t.Properties.VariableNames(1:size(header_I_t,2)) = header_I_t;
writetable(T_I_t, 'hc-noise-forrer2021-intensity-time-THz.csv');
% Beatnote
T_BN = array2table([freqs_BN_red'*1e-9 10*log10(Y_BN_red)']);
header_BN = {'freq', 'intensity'};
T_BN.Properties.VariableNames(1:size(header_BN,2)) = header_BN;
string_BN = append('hc-noise-forrer2021-BN.csv');
writetable(T_BN, string_BN);
% HC comb spectrum
T_hc = array2table([freqs_norm_red'*1e-12 P_norm_red']);
header_hc = {'frequency', 'intensity'};
T_hc.Properties.VariableNames(1:size(header_hc,2)) = header_hc;
writetable(T_hc,append('hc-noise-forrer2021-spec.csv'));
% RIN
T_RIN = array2table([freqs_RIN' 10*log10(RIN_locmax_red(1:num_modes,:)'...
    ) 10*log10(RIN_red)]);
header_RIN = [{'frequency'}, cell_legend ];
T_RIN.Properties.VariableNames(1:size(header_RIN,2)) = header_RIN;
writetable(T_RIN,append('hc-noise-forrer2021-RINdB.csv'));