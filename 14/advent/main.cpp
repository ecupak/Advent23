// Open and read input file.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <algorithm> // count


// DIFFICULTY
static constexpr int PUZZLE_PART = 1;


// EVERGREEN FUNCS //

// Get and store input as lines of strings.
const int parseInputFile(std::string& filename, std::vector<std::string>& list)
{
	std::ifstream inf{ "../" + filename + ".txt" };

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

struct int2
{
	int2() = default;
	int2(int x, int y) : x{ x }, y{ y } {}

	int x = 0;
	int y = 0;
};


void AddBorder(std::vector<std::string>& newMap, std::vector<std::string>& oldMap);
void Rotate(std::vector<std::string>& newMap, std::vector<std::string>& oldMap);
void ConvertTo1D(std::vector<int>& startMap, std::vector<std::string>& borderedList);
void Tilt(int2& stepDir, int tiltMap[], int2 hashes[], size_t hashSize);


int width, height;

constexpr int DOT	= 0b001;
constexpr int ROCK	= 0b010;
constexpr int HASH	= 0b100;


// MAIN //

int main()
{
	// PARSE INPUT FILE //

	std::string filename = "input";
	std::vector<std::string> list;

	if (parseInputFile(filename, list) == 1)
	{
		return 1;
	}

	// SOLVE PUZZLE //

	auto answer = 0;

	// Add # border.
	std::vector<std::string> borderedList;
	AddBorder(borderedList, list);

	// Get width and height.
	width = borderedList[0].length();
	height = borderedList.size();

	// Convert to number map in 1D. Save a copy for comparison.
	std::vector<int> tempMap;
	ConvertTo1D(tempMap, borderedList);	
	std::vector<int> tempMap2{ tempMap };

	// Fill into dumb array.
	int* startMap = new int[tempMap.size()];
	int* tiltMap = new int[tempMap.size()];	
	
	startMap = tempMap.data();
	tiltMap = tempMap2.data();

	// Find all # locations.
	std::vector<int2> hashMarks;
	
	for (int y = 0; y < height; ++y)
	{
		int yOffset = y * width;

		for (int x = 0; x < width; ++x)
		{
			if (startMap[x + yOffset] && 2)
			{
				hashMarks.emplace_back(x, y);
			}
		}
	}

	int hashSize = hashMarks.size();
	int2* hashes = new int2[hashSize];
	hashes = hashMarks.data();

	// Repeat until original map returns.	
	bool match = false;
	int2 stepDirection{ 0, 0 };
	size_t fullSize = width * height;

	size_t cycles = 0;
	for (; cycles < 1'000'000'000; ++cycles)
	{
		// Spin 4 times.
		{
			// NORTH - Go down, pull things up.
			stepDirection.x = 0;
			stepDirection.y = 1;

			Tilt(stepDirection, tiltMap, hashes, hashSize);

			// WEST - Go right, pull things left.
			stepDirection.x = 1;
			stepDirection.y = 0;

			Tilt(stepDirection, tiltMap, hashes, hashSize);

			// SOUTH - Go up, pull things down.
			stepDirection.x = 0;
			stepDirection.y = -1;

			Tilt(stepDirection, tiltMap, hashes, hashSize);

			// EAST - Go left, pull things right.
			stepDirection.x = -1;
			stepDirection.y = 0;

			Tilt(stepDirection, tiltMap, hashes, hashSize);
		} // End 1 cycle (4 spins)

		match = true;
		for (int i = 0; i < fullSize; ++i)
		{
			if (tiltMap[i] != startMap[i])
			{
				match = false;
				break;
			}
		}

	} // Spin 4 more times.

	std::cout << cycles << std::endl;

	{
		std::ofstream ofs("bigresult.txt");
		for (int y = 1; y < height - 1; ++y)
		{
			int yOffset = y * width;

			for (int x = 1; x < width - 1; ++x)
			{
				ofs << tiltMap[x + yOffset];
			}
		}
		ofs.close();
	}

	// Count value of rocks.
	{
		int value = 0;

		for (int y = 1; y < height - 1; ++y)
		{
			value = height - 1 - y;

			int rocks = 0;

			for (int x = 1; x < width - 1; ++x)
			{
				if (tiltMap[x + (y * width)] & ROCK)
				{
					++rocks;
				}
			}

			answer += value * rocks;
		}
	}

	std::cout << "The answer to part " << PUZZLE_PART << " is: " << answer << std::endl;
	
	return 0;
}


void AddBorder(std::vector<std::string>& newMap, std::vector<std::string>& oldMap)
{
	// First and last line = all #
	std::string allObstacles(oldMap[0].length() + 2, '#');

	newMap.push_back(allObstacles);

	for (int i = 0; i < oldMap.size(); ++i)
	{
		newMap.push_back("#" + oldMap[i] + "#");
	}

	newMap.push_back(allObstacles);
}


void Rotate(std::vector<std::string>& newMap, std::vector<std::string>& oldMap)
{
	for (int x = 0; x < oldMap[0].length(); ++x)
	{
		std::string newLine = "";

		for (int y = oldMap.size() - 1; y > -1; --y)
		{
			newLine.push_back(oldMap[y][x]);
		}

		newMap.push_back(newLine);
	}
}


void ConvertTo1D(std::vector<int>& startMap, std::vector<std::string>& borderedList)
{
	startMap.reserve(borderedList.size() * borderedList[0].length());

	for (int y = 0; y < borderedList.size(); ++y)
	{
		std::string& line = borderedList[y];

		for (int x = 0; x < line.length(); ++x)
		{
			if (line[x] == '.')
			{
				startMap.push_back(DOT);
			}
			else if (line[x] == 'O')
			{
				startMap.push_back(ROCK);
			}
			else if (line[x] == '#')
			{
				startMap.push_back(HASH);
			}
		}
	}
}


void Tilt(int2& stepDir, int tiltMap[], int2 hashes[], size_t hashSize)
{
	int2 step{ 0, 0 };

	for (size_t i = 0; i < hashSize; ++i)
	{
		// First step from hash.
		step.x = hashes[i].x + stepDir.x;
		step.y = hashes[i].y + stepDir.y;

		// Skip if hash + stepDir is off grid.
		if (step.x < 0 || step.y < 0)
		{
			continue;
		}

		if (step.x >= width || step.y >= height)
		{
			continue;
		}

		// Move rocks.
		int value = -1;
		int2 firstEmpty = step;
		bool missingFirstEmpty = true;

		while (true)
		{
			// Precompute reused number.
			value = tiltMap[step.x + (step.y * width)];

			if (missingFirstEmpty && (value & DOT))
			{
				firstEmpty = step;
				missingFirstEmpty = false;
			}
			else if (value & ROCK)
			{
				if (!missingFirstEmpty) // Skip if haven't found an empty spot yet.
				{
					// Assign rock to the empty spot.
					tiltMap[firstEmpty.x + (firstEmpty.y * width)] = ROCK;

					// Assign empty to the rock (swap them).
					tiltMap[step.x + (step.y * width)] = DOT;

					// Advance empty spot by 1 step.
					firstEmpty.x += stepDir.x;
					firstEmpty.y += stepDir.y;
				}
			}
			else if (value & HASH)
			{
				break; // Move on to next hash to check.
			}

			// Advance step.
			step.x += stepDir.x;
			step.y += stepDir.y;
		}
	}
}