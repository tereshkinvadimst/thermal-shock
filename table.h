#pragma once

#include <iostream>
#include <fstream>
#include <vector>

enum material_properties
{
	Cp = 1,
	lambda,
	a,
};


struct material_table
{
	float T, Cp, lambda, alpha;
	friend std::istream& operator >> (std::istream& in, material_table& data)
	{
		in >> data.T >> data.Cp >> data.lambda >> data.alpha;
		return in;
	}
	friend std::ostream& operator << (std::ostream& out, const material_table& part)
	{
		return (out << part.T << "\t" << part.Cp << "\t" << part.lambda << '\t' << part.alpha);
	}
};


std::vector<material_table> read_table(const char* file_name)
{
	std::ifstream fin(file_name);
	material_table buffer;
	std::vector<material_table> table;
	if (fin.is_open())
	{
		do
		{
			if (fin >> buffer)
			{
				table.push_back(buffer);
			}
			else
			{
				fin.clear();
				fin.ignore(1, '\n');
			}

		} while (!fin.eof());
		fin.close();
		return table;
	}
	else
	{
		fin.close();
		std::cerr << "Error\n";
		return {};
	}
}

const double liner_interpolation(const double& temperature, const std::vector<material_table>& data,
	const material_properties& type_output = a)
{
	if (temperature < data[0].T || temperature > data[data.size() - 1].T)
	{
		return {};
	}
	else
	{
		int i{};
		double k{}, b{};
		while (temperature > data[i].T)
			++i;

		switch (type_output)
		{
		case Cp:
			k = (data[i + 1].Cp - data[i].Cp) / (data[i + 1].T - data[i].T);
			b = data[i].Cp - k * data[i].T;
			break;
		case lambda:
			k = (data[i + 1].lambda - data[i].lambda) / (data[i + 1].T - data[i].T);
			b = data[i].lambda - k * data[i].T;
			break;
		case a:
			k = (data[i + 1].alpha - data[i].alpha) / (data[i + 1].T - data[i].T);
			b = data[i].alpha - k * data[i].T;
			break;
		default:
			k = (data[i + 1].alpha - data[i].alpha) / (data[i + 1].T - data[i].T);
			b = data[i].alpha - k * data[i].T;
			break;
		}
		return k * temperature + b;
	}
}