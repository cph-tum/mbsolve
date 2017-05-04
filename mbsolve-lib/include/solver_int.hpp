/*
 * mbsolve: Framework for solving the Maxwell-Bloch/-Lioville equations
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

#ifndef MBSOLVE_SOLVER_INT_H
#define MBSOLVE_SOLVER_INT_H

#include <string>
#include <vector>
#include <device.hpp>
#include <scenario.hpp>
#include <types.hpp>

namespace mbsolve {

class solver_int
{
protected:
    Scenario m_scenario;
    const device * const m_device;

    std::vector<Result *> m_results;
public:
    solver_int(device * const dev, const Scenario& scenario) :
	m_device(dev), m_scenario(scenario) { }

    virtual ~solver_int();

    const Scenario& get_scenario() const { return m_scenario; }

    //   const Device& getDevice() const { return m_device; }

    virtual std::string get_name() const = 0;

    virtual void run() const = 0;

    const std::vector<Result *>& get_results() const
    {
	return m_results;
    }
};

class solver_factory_int
{
public:
    solver_factory_int() { }
    virtual ~solver_factory_int() { }
    virtual solver_int* create_instance(device * const dev,
                                        const Scenario& scenario) const = 0;
    virtual const std::string& get_name() const = 0;
};

template<typename T>
class solver_factory : solver_factory_int
{
private:
    std::string m_name;
public:
    explicit solver_factory(const std::string& name) : m_name(name) {
        solver::register_factory(name, this);
    }

    solver_int* create_instance(device * const dev,
                                const Scenario& scenario) const {
	return new T(dev, scenario);
    }

    const std::string& get_name() const { return m_name; }
};

}

#endif