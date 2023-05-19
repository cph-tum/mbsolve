/*
 * mbsolve: An open-source solver tool for the Maxwell-Bloch equations.
 *
 * Copyright (c) 2023, Computational Photonics Group, Technical University of
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

#ifndef MBSOLVE_LIB_READER_H
#define MBSOLVE_LIB_READER_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <mbsolve/lib/device.hpp>
#include <mbsolve/lib/qm_description.hpp>
#include <mbsolve/lib/result.hpp>
#include <mbsolve/lib/scenario.hpp>
#include <mbsolve/lib/types.hpp>

namespace mbsolve {

/**
 * This class provides the static interface to create an instance of a reader
 * implementation and the base class fo each reader implementation.
 * \ingroup MBSOLVE_LIB
 */
class reader
{
public:
    typedef std::function<std::shared_ptr<reader>()> bootstrap_t;

private:
    static std::map<std::string, bootstrap_t> m_bootstraps;

protected:
    /**
     * Constructs reader (only available for derived classes).
     *
     * \param [in] name      Name of the reader method.
     * \param [in] extension File extension used by reader.
     */
    reader(const std::string& name, const std::string& extension);

    /* reader name, set during registration */
    std::string m_name;

    /* file extension, set by derived class */
    std::string m_ext;

public:
    /**
     * Destructs reader.
     */
    virtual ~reader();

    /**
     * Gets available readers.
     */
    static std::vector<std::string> get_avail_readers();

    /**
     * Registers reader bootstrap.
     */
    static void register_bootstrap(const std::string& name, bootstrap_t b);

    /**
     * Provides a shortcut function to register a reader of given type T.
     */
    template<typename T>
    static void register_reader(const std::string& name)
    {
        register_bootstrap(name, []() { return std::make_shared<T>(); });
    }

    /**
     * Constructs reader with a given \p name.
     *
     * \param [in] name Name of the reader method.
     */
    static std::shared_ptr<reader> create_instance(const std::string& name);

    /**
     * Gets reader name.
     */
    const std::string& get_name() const;

    /**
     * Gets file extension.
     */
    const std::string& get_extension() const;

    /**
     * Reads field array from a \p file.
     *
     * \param [in] file     Filename.
     * \param [in] name     Name of specific field array to be red.
     */
    virtual std::vector<real> read_field(
        const std::string& file,
        const std::string& name) const = 0;

    /**
     * Reads density matrix array from a \p file.
     *
     * \param [in] file     Filename.
     */
    virtual std::vector<std::shared_ptr<qm_operator> > read_density(
        const std::string& file) const = 0;
};
}

#endif
