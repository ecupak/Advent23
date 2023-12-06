// Open and read input file.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <algorithm>
#include <limits>
#include <utility>


static constexpr int PUZZLE_PART = 2;

const int parseInputFile(std::vector<std::string>& list);


struct SeedData
{
	SeedData(size_t id)
		:id_{ id }
	{	}

	size_t id_{ 0 };
	size_t data_[7]{ 0 };
};

using Seeds = std::vector<SeedData>;

struct FieldData
{
	FieldData(size_t dest, size_t src, size_t rng)
	{	
		data_[0] = dest;
		data_[1] = src;
		data_[2] = rng;
	}

	size_t data_[3]{ 0 };
};

using Almanac = std::vector<FieldData>;
using Almanacs = std::vector<Almanac>;

void LoadSeeds(Seeds& seeds, std::vector<std::string>& list);
void LoadManySeeds(Seeds& seeds, Almanac& soilAlmanac);
void LoadAlmanacData(Almanacs& almanacs, std::vector<std::string>& list);
void SortAlmanacData(Almanacs& almanacs);
void ProcessSeed(SeedData& seed, Almanacs& almanacs);

const size_t ConvertCharToInt(std::string intAsChars);

const int ProcessEasyResults();
const int ProcessHardResults();


static constexpr int DESTINATION = 0;
static constexpr int SOURCE = 1;
static constexpr int RANGE = 2;

static constexpr int SOIL = 0;
static constexpr int FERTILIZER = 1;
static constexpr int WATER = 2;
static constexpr int LIGHT = 3;
static constexpr int TEMPERATURE = 4;
static constexpr int HUMIDITY = 5;
static constexpr int LOCATION = 6;


int main()
{
	// PARSE INPUT FILE //

	std::vector<std::string> list;

	if (parseInputFile(list) == 1)
	{
		return 1;
	}
		
	// SOLVE PUZZLE //

	size_t lowestDestination = std::numeric_limits<size_t>::max();
	
	// Load each set of almanac (field map) data.
	Almanacs almanacs;
	LoadAlmanacData(almanacs, list);
	SortAlmanacData(almanacs);

	// Load seed data.
	Seeds seeds;
	LoadSeeds(seeds, list);
	
	if (PUZZLE_PART == 1)
	{
		for (int s = 0; s < seeds.size(); ++s)
		{
			ProcessSeed(seeds[s], almanacs);

			if (seeds[s].data_[LOCATION] < lowestDestination)
			{
				lowestDestination = seeds[s].data_[LOCATION];
			}
		}	
	}
	else
	{
		//LoadManySeeds(seeds, almanacs[SOIL]);

		for (size_t s = 0; s < seeds.size(); s += 2)
		{
			size_t start = seeds[s].id_;
			size_t end = start + seeds[s + 1].id_ - 1;

			for (size_t i = start; i < end; ++i)
			{
				SeedData seed{ i };
				ProcessSeed(seed, almanacs);

				if (seed.data_[LOCATION] < lowestDestination)
				{
					lowestDestination = seed.data_[LOCATION];
				}
			}
		}
	}
	
	// Find lowest location value.

	if (PUZZLE_PART == 1)
	{
		std::cout << "The answer to part " << PUZZLE_PART << " is: " << lowestDestination << std::endl;
	}
	else
	{
		std::cout << "The answer to part " << PUZZLE_PART << " is: " << lowestDestination << std::endl;
	}

	return 0;
}


void LoadSeeds(Seeds& seeds, std::vector<std::string>& list)
{
	std::string& line = list[0];

	int start = line.find(':') + 2;
	int gap = 0;

	while (gap != std::string::npos)
	{
		gap = line.find(' ', start);

		std::string strNumber = line.substr(start, (gap - start));
		const size_t intNumber = ConvertCharToInt(strNumber);

		seeds.emplace_back(intNumber);

		start = gap + 1;
	}
}


void LoadManySeeds(Seeds& seeds, Almanac& soilAlmanac)
{
	struct SeedZone
	{
		SeedZone(size_t s, size_t e)
			: start{ s }
			, end{ e }
		{	
			range = end - start + 1;
		}

		size_t start{ 0 };
		size_t range{ 0 };
		size_t end{ 0 };
	};

	std::vector<SeedZone> zones;

	// Build zones.
	for (int s = 0; s < seeds.size(); s += 2)
	{
		// Originally stored as ids for puzzle part 1.
		size_t seedStart = seeds[s].id_;
		size_t seedRange = seeds[s + 1].id_;
		size_t seedEnd = seedStart + seedRange - 1;

		zones.emplace_back(seedStart, seedEnd);
	}

	// Go through each field in the soil almanac.
	// Split seed zones along boundary lines and delete the ones not needed.
	std::vector<SeedZone> newZones;
	for (int f = 0; f < soilAlmanac.size(); ++f)
	{
		FieldData field = soilAlmanac[f];

		size_t start = field.data_[SOURCE];
		size_t range = field.data_[RANGE];
		size_t end = start + range - 1;

		for (int z = 0; z < zones.size(); ++z)
		{
			SeedZone& zone = zones[z];
			bool madeNewZone = false;

			// If no overlap, skip.
			if (zone.start <= end && zone.end >= start)
			{
				// Keep anything above and until the start value.
				if (zone.start <= start)
				{
					newZones.emplace_back(zone.start, start);
					madeNewZone = true;
				}

				// Keep anything below and up to the end value.
				if (zone.end >= end)
				{
					newZones.emplace_back(end, zone.end);
					madeNewZone = true;
				}
			}

			// Keep original zone if no new ones made.
			if (!madeNewZone)
			{
				newZones.emplace_back(zone.start, zone.end);
			}
		}

		zones = newZones;
		newZones.clear();	
	}

	// Apply final zones as new seed values.
	using SeedPair = std::pair<size_t, size_t>;
	std::vector<SeedPair> seedPairs;

	for (int z = 0; z < zones.size(); z += 2)
	{
		SeedPair sp = std::make_pair(zones[z].start, zones[z].range);
	}

	seeds.clear();

}


void LoadAlmanacData(Almanacs& almanacs, std::vector<std::string>& list)
{
	int lineCount = 1;

	Almanac almanac;

	while (lineCount < list.size())
	{
		std::string& line = list[lineCount];

		// Look for colon - represents new header.
		if (line.find(':') != std::string::npos)
		{
			// Add almanac with data to almanacs.
			if (almanac.size() > 0)
			{
				almanacs.push_back(almanac);
			}

			// Reset almanac.
			almanac.clear();
		}
		// Make sure this isn't an empty line and then load almanac data.
		else if (line.length() > 0)
		{
			int start = 0;
			std::string number = "";
			FieldData fieldData{0, 0, 0};

			for (int i = 0; i < 3; ++i)
			{
				int gap = line.find(' ', start);

				number = line.substr(start, (gap - start));
				const size_t intNumber = ConvertCharToInt(number);
				
				fieldData.data_[i] = intNumber;

				start = gap + 1;
			}

			almanac.push_back(fieldData);
		}

		++lineCount;
	}

	// Final almanac added.
	if (almanac.size() > 0)
	{
		almanacs.push_back(almanac);
	}
}


bool AlmanacSorter(FieldData& a, FieldData& b)
{
	return a.data_[SOURCE] < b.data_[SOURCE];
}


void SortAlmanacData(Almanacs& almanacs)
{
	// For every almanac in almanacs, sort their field data by smallest source to largest.
	for (int a = 0; a < almanacs.size(); ++a)
	{
		Almanac& almanac = almanacs[a];

		std::sort(almanac.begin(), almanac.end(), AlmanacSorter);
	}
}


void ProcessSeed(SeedData& seed, Almanacs& almanacs)
{
	for (int m = 0; m < almanacs.size(); ++m)
	{
		// Pre-assign default value if previous value is not in mapping.
		// (Use Id as soil, soil as fert, fert as water, etc).
		size_t compare;
		if (m == SOIL)
		{
			seed.data_[SOIL] = seed.id_;
			compare = seed.id_;
		}
		else
		{
			seed.data_[m] = seed.data_[m - 1];
			compare = seed.data_[m - 1];
		}

		// Look for mapping.
		for (FieldData& field : almanacs[m])
		{
			size_t start = field.data_[SOURCE];
			size_t end = start + field.data_[RANGE] - 1;			

			if (compare >= start && compare <= end)
			{
				size_t offset = compare - start;
				seed.data_[m] = field.data_[DESTINATION] + offset;
				break;
			}
		}
	}
}


// Convert an array of char's into an int of the expected value ("563" = 563).
const size_t ConvertCharToInt(std::string intAsChars)
{
	size_t value{ 0 };

	for (size_t index{ 0 }, exponent{ intAsChars.length() - 1}; index < intAsChars.length(); ++index, --exponent)
	{
		int num{ intAsChars[index] - '0' };

		value += static_cast<size_t>(num * pow(10, exponent));
	}

	return value;
}

const int ProcessEasyResults()
{	
	return 0;
}


const int ProcessHardResults()
{
	return 0;
}


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