/*
 * mbsolve: An open-source solver tool for the Maxwell-Bloch equations.
 *
 * Copyright (c) 2016, Computational Photonics Group, Technical University of
 * Munich.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef MBSOLVE_READER_HDF5_H
#define MBSOLVE_READER_HDF5_H

#include <mbsolve/lib/reader.hpp>

namespace mbsolve {

/**
 * \defgroup MBSOLVE_READER_HDF5 reader-hdf5
 * Reader for Hierarchic Data Format (HDF) 5 format.
 */

/**
 * Reader class for Hierarchic Data Format (HDF) 5 format.
 * \ingroup MBSOLVE_READER_HDF5
 */
class reader_hdf5 : public reader
{
public:
    /**
     * Constructs reader for Hierarchic Data Format (HDF) 5 format.
     */
    reader_hdf5();

    /**
     * Destructs reader for Hierarchic Data Format (HDF) 5 format.
     */
    ~reader_hdf5();

    /**
     * Reads field array from HDF5 file.
     */
    std::vector<real> read_field(
        const std::string& file,
        const std::string& name) const;

    /**
     * Reads density matrix array from HDF5 file.
     */
    std::vector<std::shared_ptr<qm_operator> > read_density(
        const std::string& file) const;
};

/**
 * Helper class that enables usage of the HDF5 reader.
 */
class reader_hdf5_loader
{
public:
    reader_hdf5_loader();
};
}

#endif
