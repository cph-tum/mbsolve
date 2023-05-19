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

#include <mbsolve/lib/reader.hpp>

namespace mbsolve {

std::map<std::string, reader::bootstrap_t> reader::m_bootstraps;

std::shared_ptr<reader>
reader::create_instance(const std::string& name)
{
    auto it = m_bootstraps.find(name);
    if (it == m_bootstraps.end()) {
        throw std::invalid_argument("Unknown reader " + name);
    }
    return it->second();
}

void
reader::register_bootstrap(const std::string& name, bootstrap_t b)
{
    if (m_bootstraps[name]) {
        throw std::invalid_argument("Reader already registered.");
    }
    m_bootstraps[name] = b;
}

reader::reader(const std::string& name, const std::string& extension)
  : m_name(name), m_ext(extension)
{}

reader::~reader() {}

const std::string&
reader::get_name() const
{
    return m_name;
}

const std::string&
reader::get_extension() const
{
    return m_ext;
}

std::vector<std::string>
reader::get_avail_readers()
{
    std::vector<std::string> readers;

    for (const auto& s : m_bootstraps) {
        readers.push_back(s.first);
    }

    return readers;
}
}
