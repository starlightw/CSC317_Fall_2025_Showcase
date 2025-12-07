#include "write_ppm.h"
#include <fstream>
#include <cassert>
#include <iostream>

bool write_ppm(
	const std::string& filename,
	const std::vector<unsigned char>& data,
	const int width,
	const int height,
	const int num_channels)
{
	assert(
		(num_channels == 3 || num_channels == 1) &&
		".ppm only supports RGB or grayscale images");
	////////////////////////////////////////////////////////////////////////////
	// Replace with your code here:

	if (width <= 0 || height <= 0) return false;
	//if (num_channels != 1 && num_channels != 3) return false;

	std::ofstream out(filename);
	if (!out) return false;

	const char* magic_num = (num_channels == 1) ? "P2" : "P3"; // Magic number: P2 for grayscale, P3 for RGB
	out << magic_num << "\n";
	out << width << ' ' << height << "\n";
	out << 255 << "\n";

	for (int i = 0; i < width * height * num_channels; i++)
	{
		out << static_cast<int>(data[i]) << ' ';
		if (i % width == 0 && i != 0)
		{
			out << "\n";
		}

	}

	return true;


	////////////////////////////////////////////////////////////////////////////


}
