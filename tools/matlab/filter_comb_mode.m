function field_out = filter_comb_mode(field, f_mode, f_win, fs)
% filter_comb_mode  Filter comb mode using iirpeak. 

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

% Design filter
[b,a] = iirpeak(f_mode/(fs/2),f_win/(fs/2));
% Filter comb mode
    field_out = filter(b,a, field);
for i = 1:5
            field_out = filter(b,a, field_out);
end
end