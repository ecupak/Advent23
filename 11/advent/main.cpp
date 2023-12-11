// Open and read input file.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


//#pragma comment(linker, "/STACK:10000000")

// DIFFICULTY
static constexpr int PUZZLE_PART = 2;


// EVERGREEN FUNCS //

// Get and store input as lines of strings.
const int parseInputFile(std::vector<std::string>& list)
{
	std::ifstream inf{ "../input.txt" };

	if (!inf)
	{
		std::cerr << "Couldn't find file.\n";
		return 1;
	}

	std::string strInput;

	while (inf)
	{
		std::getline(inf, strInput);
		list.push_back(strInput);
	}

	// The while-loop always pulls a "blank" line at the end. Remove it here.
	list.pop_back();

	inf.close();

	return 0;
}


// Convert an array of char's into an int of the expected value ("563" = 563).
const int64_t convertCharToInt(std::string intAsChars)
{
	int64_t value{ 0 };

	bool isNegative = intAsChars[0] == '-';

	if (isNegative)
	{
		intAsChars = intAsChars.substr(1);
	}

	for (int64_t index{ 0 }, exponent{ static_cast<int64_t>(intAsChars.length() - 1) }; index < intAsChars.length(); ++index, --exponent)
	{
		int64_t num{ intAsChars[index] - '0' };

		value += static_cast<int64_t>(num * pow(10, exponent));
	}

	value *= isNegative ? -1 : 1;
	
	return value;
}


// PUZZLE FUNCS //

size_t leapAmount = 1'000'000;

struct int2
{
	int x{ 0 };
	int y{ 0 };
};

std::vector<bool> isBigStepX;
std::vector<bool> isBigStepY;

std::vector<size_t> distances;
std::vector<size_t> leaps;


int width, height;

int main()
{
	// PARSE INPUT FILE //

	std::vector<std::string> list;

	if (parseInputFile(list) == 1)
	{
		return 1;
	}

	// SOLVE PUZZLE //

	size_t answer = 0;

	// Expand on the x axis.
	{
		// Find step sizes.
		for (int i = 0; i < list.size(); ++i)
		{
			std::string& line = list[i];

			isBigStepY.push_back(line.find('#') == std::string::npos);
		}
	}

	// Expand on the y axis.
	{
		// Transpose.
		std::vector<std::string> transpose;
		for (int x = 0; x < list[0].length(); ++x)
		{
			std::string tline{ "" };

			for (int y = 0; y < list.size(); ++y)
			{
				tline.push_back(list[y][x]);
			}

			transpose.push_back(tline);
		}

		// Find step sizes. 
		for (int i = 0; i < transpose.size(); ++i)
		{
			std::string& line = transpose[i];

			isBigStepX.push_back(line.find('#') == std::string::npos);
		}
	}

	// Locate all galaxies.
	width = list[0].length();
	height = list.size();
	std::vector<int2> galaxies;
	{
		for (int y = 0; y < height; ++y)
		{
			std::string& line = list[y];
			int pos = line.find('#');

			// Inner x loop.
			while (pos != std::string::npos)
			{
				galaxies.push_back({ pos, y });

				pos = line.find('#', pos + 1);
			}
		}
	}

	// Find distances.
	for (int g = 0; g < galaxies.size() - 1; ++g)
	{
		for (int f = g + 1; f < galaxies.size(); ++f)
		{
			// Extract values.
			int x1 = galaxies[g].x;
			int y1 = galaxies[g].y;
			int x2 = galaxies[f].x;
			int y2 = galaxies[f].y;

			// Order smallest to largest.
			if (x2 < x1)
			{
				std::swap(x1, x2);
			}

			if (y2 < y1)
			{
				std::swap(y1, y2);
			}

			// Find normal distances.
			int2 distance = { x2 - x1, y2 - y1 };			
			distances.push_back(static_cast<size_t>(distance.x + distance.y));

			// Find expansion leaps.
			int2 leapDistance{ 0, 0 };

			for (int x = x1 + 1; x <= x2; ++x)
			{
				if (isBigStepX[x])
				{
					leapDistance.x += leapAmount - 1;
				}
			}

			for (int y = y1 + 1; y <= y2; ++y)
			{
				if (isBigStepY[y])
				{
					leapDistance.y += leapAmount - 1;
				}
			}
			
			leaps.push_back(leapDistance.x + leapDistance.y);
		}
	}

	// Sum distances.
	{
		for (int i = 0; i < distances.size(); ++i)
		{ 
			answer += distances[i] + leaps[i];
		}
	}
 
	std::cout << "The answer to part " << PUZZLE_PART << " is: " << answer << std::endl;
	
	return 0;
}