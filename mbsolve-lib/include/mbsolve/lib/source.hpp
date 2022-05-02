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

#ifndef MBSOLVE_LIB_SOURCE_H
#define MBSOLVE_LIB_SOURCE_H

#include <algorithm>
#include <complex>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <mbsolve/lib/types.hpp>

namespace mbsolve {

/**
 * Base class for all sources in \ref scenario.
 * \ingroup MBSOLVE_LIB
 */
class source
{
public:
    enum type
    {
        hard_source,
        soft_source
    };

public:
    std::string m_name;

    /* amplitude */
    real m_ampl;

    /* (carrier) frequency */
    real m_freq;

    /* phase */
    real m_phase;

    /* position */
    real m_position;

    /* internal resistance */
    real m_r_i;

    type m_type;

    real m_timestep_size;

public:
    /**
     * Constructs source.
     *
     * \param [in] name        Name of the source.
     * \param [in] position    Position of the source (in meter).
     * \param [in] source_type Source type.
     * \param [in] ampl        Amplitude of the source.
     * \param [in] freq        Frequency of the source.
     * \param [in] phase       Phase of the source.
     */
    source(
        const std::string& name,
        real position,
        type source_type,
        real ampl,
        real freq,
        real phase = 0)
      : m_name(name), m_position(position), m_type(source_type), m_ampl(ampl),
        m_freq(freq), m_phase(phase)
    {}

    /* TODO: get_value/calc_value : simplify */

    /**
     * Gets source value at certain \p time value.
     *
     * \param [in] n_t             Number of time step.
     * \param [in] current_value Reserved for later use.
     */
    real get_value(int n_t, real current_value = 0.0) const
    {
        /* calculate source value */
        real val = m_ampl * calc_value(n_t);

        /* if type == thevenin, consider internal resistance */

        /* else if soft source */
        // return current_value + val;

        /* else -> hard source */
        return val;
    }

    /* calculate new value */
    virtual real calc_value(int /* n_t */) const { return 0.0; }

    /**
     * Gets source position.
     */
    real get_position() const { return m_position; }

    /**
     * Gets source type.
     */
    type get_type() const { return m_type; }

    /* TODO: specify dm entry/field etc? */

    /* TODO: add position. how?
           virtual const Quantity& position
    */
    /* TODO: add source type: hard, soft, thevenin */
    /* TODO: for thevenin: add internal resistance */

    /**
     * Sets time step size.
     */
    virtual void set_timestep_size(real timestep_size)
    {
        m_timestep_size = timestep_size;
    }
};
/*
class sine_source : public source
{
private:

    real calc_value(real t) const
    {
        return sin(2 * PI * m_freq * t + m_phase);
    }

public:
    sine_source(const std::string& name, real ampl, real freq, real phase) :
        source(name, ampl, freq, phase)
    {
    }

    };*/

/**
 * Sech pulse source.
 * \ingroup MBSOLVE_LIB
 */
class sech_pulse : public source
{
private:
    real m_beta;
    real m_phase_sin;

public:
    /**
     * Constructs sech pulse source. The sech pulse is defined as
     * \f[ E(t) = A \sin ( 2 \pi f t - \varphi_1) \mathrm{sech}
     * (\beta t - \varphi_2). \f]
     *
     * \param [in] name        Name of the source.
     * \param [in] position    Position of the source (in meter).
     * \param [in] source_type Source type.
     * \param [in] ampl        Amplitude \f$ A \f$ of the source.
     * \param [in] freq        Frequency \f$ f \f$ of the source.
     * \param [in] phase       Phase \f$ \varphi_2 \f$ of the source.
     * \param [in] beta        Parameter \f$ \beta \f$.
     * \param [in] phase_sin   Paramter \f$ \varphi_1 \f$.
     */
    sech_pulse(
        const std::string& name,
        real position,
        type source_type,
        real ampl,
        real freq,
        real phase,
        real beta,
        real phase_sin = 0.0)
      : source(name, position, source_type, ampl, freq, phase), m_beta(beta),
        m_phase_sin(phase_sin)
    {}

    /**
     * Gets source value at certain \p time value.
     *
     * \param [in] n_t             Number of time step.
     */
    real calc_value(int n_t) const
    {
        real t = n_t * m_timestep_size;
        return 1 / std::cosh(m_beta * t - m_phase) *
            sin(2 * PI * m_freq * t - m_phase_sin);
    }
};

/*
class single_cycle_pulse : public source
{
private:
    real m_beta;

public:
    single_cycle_pulse(const std::string& name, real position,
                       type source_type,
                       real ampl, real freq,
                       real phase,
                       real beta) :
        source(name, position, source_type, ampl, freq, phase), m_beta(beta)
    {
    }

    real calc_value(real t) const
    {
        return 1/std::cosh(m_beta * (t - m_phase)) *
            sin(2 * PI * m_freq * (t - m_phase - 1/(m_freq * 4)));
    }

};
*/

/**
 * Gaussian pulse source.
 * \ingroup MBSOLVE_LIB
 */
class gaussian_pulse : public source
{
private:
    real m_tau;

public:
    /**
     * Constructs Gaussian pulse source. The pulse is defined as
     * \f[ E(t) = A \sin ( 2 \pi f t)
     * \exp\left( - \frac{(t - t_0)^2}{\tau^2} \right) \f]
     *
     * \param [in] name        Name of the source.
     * \param [in] position    Position of the source (in meter).
     * \param [in] source_type Source type.
     * \param [in] ampl        Amplitude \f$ A \f$ of the source.
     * \param [in] freq        Frequency \f$ f \f$ of the source.
     * \param [in] phase       Phase \f$ t_0 \f$ of the source.
     * \param [in] tau         Parameter \f$ \tau \f$.
     */
    gaussian_pulse(
        const std::string& name,
        real position,
        type source_type,
        real ampl,
        real freq,
        real phase,
        real tau)
      : source(name, position, source_type, ampl, freq, phase), m_tau(tau)
    {}

    real calc_value(int n_t) const
    {
        real t = n_t * m_timestep_size;
        return exp(-(t - m_phase) * (t - m_phase) / (m_tau * m_tau)) *
            sin(2 * PI * m_freq * t);
    }
};

class randomNumberBetween
{
public:
    randomNumberBetween(real low, real high)
      : random_engine_{ std::random_device{}() }, distribution_{ low, high }
    {}
    std::complex<real> operator()()
    {
        return std::complex<real>(
            distribution_(random_engine_), distribution_(random_engine_));
    }

private:
    std::mt19937 random_engine_;
    std::normal_distribution<real> distribution_;
};

class density_blackbody_norm
{
private:
    real m_temp;

public:
    density_blackbody_norm(real temp) : m_temp(temp) {}
    real calc_value(real omega) const
    {
        return (6.0 * pow(HBAR, 2)) / (PI * pow(KB * m_temp, 2)) *
            std::abs(omega) /
            (exp((HBAR * std::abs(omega)) / (KB * m_temp)) - 1);
    }
};

/**
 * Thermal noise source.
 * \ingroup MBSOLVE_LIB
 */
class thermal_noise : public source
{
private:
public:
    real m_tau_sim, m_freq_max, m_freq_min, m_delta_freq;
    std::vector<real> m_omega;
    std::vector<std::complex<real> > m_rand_numbers;
    density_blackbody_norm m_dens_e;
    /**
     * Constructs thermal noise source. The pulse is
     * based on the white-noise spectrum.
     *
     * \param [in] name        Name of the source.
     * \param [in] position    Position of the source (in meter).
     * \param [in] source_type Source type.
     * \param [in] delta       Amplitude \f$ A \f$ of the source.
     * \param [in] temp        Temperature of blackbody.
     */
    thermal_noise(
        const std::string& name,
        real position,
        type source_type,
        real temp,
        real tau_sim,
        real delta_freq = 0.0,
        real freq_max = 0.0,
        real freq_min = 0.0)
      : source(
            name,
            position,
            source_type,
            sqrt(2.0 / (6.0 * EPS0 * HBAR * 1.0 / sqrt(MU0 * EPS0))) * KB *
                temp / std::sqrt(tau_sim),
            0,
            0),
        m_dens_e(temp), m_tau_sim(tau_sim)
    {
        if (delta_freq < 1 / tau_sim) {
            m_delta_freq = 1 / tau_sim;
        } else {
            m_delta_freq = delta_freq;
        }
        m_freq_max = freq_max;
        if (freq_min < 1 / tau_sim) {
            m_freq_min = 1 / tau_sim;
        } else {
            m_freq_min = freq_min;
        }
    }

    /**
     * Sets time step size.
     */
    void set_timestep_size(real timestep_size)
    {
        m_timestep_size = timestep_size;
        int num_timesteps = ceil(m_tau_sim / timestep_size) + 1;
        if (m_freq_max == 0.0) {
            m_freq_max = 1 / (2 * m_timestep_size);
        } else if (m_freq_max > 1 / (2 * m_timestep_size)) {
            throw std::invalid_argument("Max frequency  has to be smaller "
                                        "than 1 / (2 * m_timestep_size).");
        }
        m_rand_numbers.clear();
        m_omega = get_omega();
        std::generate_n(
            std::back_inserter(m_rand_numbers),
            num_timesteps / 2,
            randomNumberBetween(0, 1));
        std::vector<std::complex<real> > rand_numbers_flip = m_rand_numbers;
        std::transform(
            rand_numbers_flip.begin(),
            rand_numbers_flip.end(),
            rand_numbers_flip.begin(),
            [](const std::complex<real> c) -> std::complex<real> {
                return std::conj(c);
            });

        std::reverse(m_rand_numbers.begin(), m_rand_numbers.end());
        m_rand_numbers.insert(
            m_rand_numbers.begin(),
            rand_numbers_flip.begin(),
            rand_numbers_flip.end());
    }

    real calc_value(int n_t) const
    {
        if (n_t % 10000 == 0) {
            std::cout << "Numt: " << n_t << std::endl;
        }
        std::complex<real> E = 0.0;
        for (int i = 0; i < m_omega.size(); i++) {
            real t = n_t * m_timestep_size;
            E += m_rand_numbers[i] *
                std::sqrt(m_dens_e.calc_value(std::abs(m_omega[i]))) *
                exp(std::complex<real>(0, 1) * m_omega[i] * t);
        }
        return E.real();
    }

    std::vector<real> get_omega()
    {
        int ind_omega = std::floor(
            (2 * PI * m_freq_max - 2 * PI * m_freq_min) /
            (2 * PI * m_delta_freq));
        std::vector<real> omega_i;
        for (int i = ind_omega; i >= 0; i--) {
            real omega =
                -(1) * i * 2 * PI * m_delta_freq - 2 * PI * m_freq_min;
            omega_i.push_back(omega);
        }
        for (int i = 0; i <= ind_omega; i++) {
            real omega = i * 2 * PI * m_delta_freq + 2 * PI * m_freq_min;
            omega_i.push_back(omega);
        }
        return omega_i;
    }
};
}
#endif
