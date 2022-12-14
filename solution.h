#include "table.h"
#pragma once
#include <string>
#include <cmath>
#include <algorithm>

enum type_event
{
	thermal_shock = 1
};

struct events_param
{
	/*
	* Thermal shock param
	* T_s - temperature of shock
	* pos - position of shock
	* coef - coefficient of exp
	*/
	double T_s{}, pos{}, coef{};
};

struct main_param
{
	int N{}, event_strart{}, event_stop{}, type_border{};
	double lenght{}, h{}, X1{}, X2{}, local_time{}, T0{}, max_time{}, out_step{};
	std::string file_name{};
	std::vector<material_table> table{};
	type_event type__event__{};
	events_param events__param__{};
};

template<typename T>
inline auto max_value_array(const std::vector<T> array, const size_t& __size)
{
	T max{ array[0] };
	for (size_t i = 1; i < __size; ++i)
	{
		if (array[i] > max)
			max = array[i];
	}
	return max;
};

void new_time_layer(double& tau, std::vector<double>& temperature, std::vector<double>& Q,
	const main_param& m_p)
{
	size_t i{}, N_T{temperature.size()}, N_Q{Q.size()};
	std::vector<double> temperaturesp1(N_T), alpha(N_T), C(N_T);
	alpha.resize(N_T);
	for (i = 0; i < N_T; ++i)
	{
		alpha[i] = liner_interpolation(temperature[i], m_p.table);
	}

	double tau_cr{ m_p.h * m_p.h / 2. / max_value_array(alpha,N_T) };
	if (tau > tau_cr || tau == 0)
	{
		tau = tau_cr;
	}

	for (i = 0; i < N_Q; ++i)
	{
		Q[i] = -liner_interpolation(temperature[i], m_p.table, lambda) * (temperature[i + 1] - temperature[i]) / m_p.h;
	}

	for (i = 0; i < N_T; ++i)
	{
		C[i] = alpha[i] * tau / m_p.h / m_p.h;
	}

	for (i = 1; i < N_T - 1; ++i)
	{
		temperaturesp1[i] = C[i] * (temperature[i + 1] - 2 * temperature[i] + temperature[i - 1]) + temperature[i];
	}

	temperature.swap(temperaturesp1);
}

void border_cond(std::vector<double>& temperature, const main_param& m_p)
{
	switch (m_p.type_border)
	{
	case 1:
		temperature[0] = m_p.X1;
		temperature[temperature.size() - 1] = m_p.X2;
		break;
	case 2:
		temperature[0] = m_p.X1 + temperature[1];
		temperature[temperature.size() - 1] = - m_p.X2 + temperature[temperature.size() - 2];
		break;
	default:
		temperature[0] = m_p.X1;
		temperature[temperature.size() - 1] = m_p.X2;
		break;
	}
}

const size_t event(std::vector<double>& temperature, const main_param& m_p)
{
	size_t i{};
	switch (m_p.type__event__)
	{
	case thermal_shock:
		for (i = 0; i < temperature.size(); i++)
		{
			if (m_p.events__param__.pos < i * m_p.h)
			{
				temperature[i] = temperature[i] +
					(m_p.events__param__.T_s - temperature[i]) *
					(1. - exp(-m_p.events__param__.coef * (m_p.local_time - m_p.event_strart) / (m_p.event_stop - m_p.event_strart)));

					break;
			}
		}
		return i;
		break;
	default:
		return {};
		break;
	}
}