// Open and read input file.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <array>
#include <algorithm>
#include <map>


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
	std::vector<int64_t> nextNumbers;

	// For each line...
	for (int i = 0; i < list.size(); ++i)	
	{
		std::string& line = list[i];
		std::vector<std::vector<int64_t>> tiers;

		// Get the input as separate values
		{
			std::vector<int64_t> historyTier;

			size_t pos = 0;
			while (pos < line.length())
			{
				size_t gap = line.find(' ', pos);

				if (gap == std::string::npos)
				{
					gap = line.length() + 1;
				}

				int64_t value = convertCharToInt(line.substr(pos, (gap - pos)));
				historyTier.push_back(value);

				pos = gap + 1;
			}
			tiers.push_back(historyTier);
		}

		// Diff history tier until all zero.
		{
			int tierIndex = 0;
			bool allZeros = false;
			while (!allZeros)
			{
				std::vector<int64_t>& currentTier = tiers[tierIndex];
				std::vector<int64_t> nextTier;

				// Create next row of differences.
				for (int i = 0; i < currentTier.size() - 1; ++i)
				{
					nextTier.push_back(currentTier[i + 1] - currentTier[i]);
				}
				tiers.push_back(nextTier);

				// Zero check.
				allZeros = true;
				for (int64_t value : nextTier)
				{
					if (value != 0)
					{
						allZeros = false;
						break;
					}
				}

				++tierIndex;
			}
		}

		// Find next history number.
		if (PUZZLE_PART == 1)
		{
			// Forwards.
			int64_t nextNumber = 0;
			for (int64_t t = tiers.size() - 1; t > -1; --t)
			{
				nextNumber += tiers[t][tiers[t].size() - 1];
			}
			nextNumbers.push_back(nextNumber);
		}
		else
		{
			// Backwards.
			int64_t nextNumber = 0;
			for (int64_t t = tiers.size() - 1; t > -1; --t)
			{
				nextNumber = tiers[t][0] - nextNumber;
			}
			nextNumbers.push_back(nextNumber);
		}
	}// Get next line.

	for (int64_t number : nextNumbers)
	{
		answer += number;
	}

	std::cout << "The answer to part " << PUZZLE_PART << " is: " << answer << std::endl;

	return 0;
}