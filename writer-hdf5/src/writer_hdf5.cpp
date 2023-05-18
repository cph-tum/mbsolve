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
#include <mbsolve/writer-hdf5/writer_hdf5.hpp>

namespace mbsolve {

writer_hdf5::writer_hdf5() : writer("hdf5", "hdf") {}

writer_hdf5::~writer_hdf5() {}

void
writer_hdf5::write(
    const std::string& filename,
    const std::vector<std::shared_ptr<result> >& results,
    std::shared_ptr<const device> dev,
    std::shared_ptr<const scenario> scen) const
{
    auto h5_dbl = H5::PredType::NATIVE_DOUBLE;
    auto h5_bool = H5::PredType::NATIVE_HBOOL;

    /* create file */
    HighFive::File file(
        filename, HighFive::File::ReadWrite | HighFive::File::Truncate);

    /* create group for simulation setup */
    file.createGroup("setup");

    /* create dataset for simulation scenario */
    auto scenario = file.createDataSet("/setup/scenario", scen->get_name());

    /* attribute time step size */
    H5Easy::dumpAttribute(
        file, "/setup/scenario", "timestep_size", scen->get_timestep_size());

    /* attribute grid point size */
    H5Easy::dumpAttribute(
        file,
        "/setup/scenario",
        "gridpoint_size",
        scen->get_gridpoint_size());

    // /* attribute simulation end time */
    H5Easy::dumpAttribute(
        file, "/setup/scenario", "sim_endtime", scen->get_endtime());

    /* create dataset for simulation device */
    auto device = file.createDataSet("/setup/device", dev->get_name());

    /* attribute grid point size */
    H5Easy::dumpAttribute(
        file, "/setup/device", "dev_length", dev->get_length());

    /* Add names of device regions */

    for (auto region : dev->get_regions()) {
    }
    /* results */
    for (auto result : results) {
        /* create group for result */
        HighFive::Group g = file.createGroup(result->get_name());

        /* attribute time interval -- per result */
        /* TODO
        H5::Attribute a_int_t = g.createAttribute("int_time", h5_dbl,
                                                  space_scalar);
        double int_t = 50e-15;
        a_int_t.write(h5_dbl, &int_t);
        */

        /* attribute result position -- per result */
        /* TODO */

        /* attribute is complex ? -- per result */
        bool is_complex = result->is_complex();
        HighFive::Attribute a_is_complex =
            g.createAttribute("is_complex", is_complex);
        a_is_complex.write(is_complex);

        /* write real part */
        std::vector<size_t> dims{ result->get_rows(), result->get_cols() };
        auto data_real = result->get_data_real().data();

        HighFive::DataSet d_real =
            g.createDataSet<double>("real", HighFive::DataSpace(dims));
        d_real.write_raw(data_real);

        /* write imag part */
        if (is_complex) {
            auto data_imag = result->get_data_imag().data();
            HighFive::DataSet d_imag =
                g.createDataSet<double>("imag", HighFive::DataSpace(dims));
            d_imag.write_raw(data_imag);
        }
    }
}

void
writer_hdf5::autosave(
    const std::string& filename,
    const std::shared_ptr<sim_data>& sim_data,
    std::shared_ptr<const device> dev,
    std::shared_ptr<const scenario> scen) const
{
    /* create file */
    HighFive::File file(
        filename, HighFive::File::ReadWrite | HighFive::File::Truncate);

    /* write e-field components*/
    HighFive::DataSet dataset_e = file.createDataSet<real>(
        "m_e", HighFive::DataSpace::From(sim_data->m_e_save));
    dataset_e.write(sim_data->m_e_save);
    /* write h-field components*/
    HighFive::DataSet dataset_h = file.createDataSet<real>(
        "m_h", HighFive::DataSpace::From(sim_data->m_h_save));
    dataset_h.write(sim_data->m_h_save);
    /* write p-field components*/
    HighFive::DataSet dataset_p = file.createDataSet<real>(
        "m_p", HighFive::DataSpace::From(sim_data->m_p_save));
    dataset_p.write(sim_data->m_p_save);
    /* write df-field components*/
    // HighFive::DataSet dataset_df =
    //     file.createDataSet<real>("m_df",
    //     HighFive::DataSpace::From(sim_data->m_df_save));
    // dataset_df.write(sim_data->m_df_save);
    /* write density matrix*/
    H5Easy::dump(file, "/density/num_gridpoints", sim_data->m_d_save.size());
    for (int i = 0; i < sim_data->m_d_save.size(); i++) {
        auto main_diag = sim_data->m_d_save[i].get_main_diagonal();
        auto off_diag = sim_data->m_d_save[i].get_off_diagonal();
        H5Easy::dump(
            file, "/density/" + std::to_string(i) + "/main_diag", main_diag);
        H5Easy::dump(
            file, "/density/" + std::to_string(i) + "/off_diag", off_diag);
    }
}

writer_hdf5_loader::writer_hdf5_loader()
{
    writer::register_writer<writer_hdf5>("hdf5");
}
}
