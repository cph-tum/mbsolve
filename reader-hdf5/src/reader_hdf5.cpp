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

#include <H5Cpp.h>
#include <highfive/H5DataSet.hpp>
#include <highfive/H5DataSpace.hpp>
#include <highfive/H5Easy.hpp>
#include <highfive/H5File.hpp>
#include <mbsolve/reader-hdf5/reader_hdf5.hpp>

namespace mbsolve {

reader_hdf5::reader_hdf5() : reader("hdf5", "hdf") {}

reader_hdf5::~reader_hdf5() {}

std::vector<real>
reader_hdf5::read_field(const std::string& filename, const std::string& name)
    const
{
    std::vector<real> read_data;

    /* open file */
    HighFive::File file(filename, HighFive::File::ReadOnly);

    /* return dataset with specifier name*/
    std::string name_vec("m_" + name);
    HighFive::DataSet dataset = file.getDataSet(name_vec);

    /* Convert HDF5 dataset to vector */
    dataset.read(read_data);

    return read_data;
}

std::vector<std::shared_ptr<qm_operator> >
reader_hdf5::read_density(const std::string& filename) const
{
    /* create file */
    HighFive::File file(filename, HighFive::File::ReadOnly);

    std::string name("density");
    int num_gridpoints = H5Easy::load<int>(file, name + "/num_gridpoints");

    /* Load vector with density matrix elements */
    std::vector<std::shared_ptr<qm_operator> > m_dens;
    m_dens.reserve(num_gridpoints);

    for (int i = 0; i < num_gridpoints; i++) {

        /* read main diagonal of density matrix */
        std::vector<real> main_diag = H5Easy::load<std::vector<real> >(
            file, name + "/" + std::to_string(i) + "/main_diag");

        /* read off diagonal of density matrix */
        std::vector<std::complex<real> > off_diag =
            H5Easy::load<std::vector<std::complex<real> > >(
                file, name + "/" + std::to_string(i) + "/off_diag");

        m_dens.push_back(std::make_shared<qm_operator>(main_diag, off_diag));
    }

    return m_dens;
}

reader_hdf5_loader::reader_hdf5_loader()
{
    reader::register_reader<reader_hdf5>("hdf5");
}
}
