// Open and read input file.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


const int parseInputFile(std::vector<std::string>& list);

struct PullResult
{
	int colors[3]{ 0, 0, 0 };
};

void FillSubsetData(std::string& pull, std::string subsets[]);
PullResult GetPullResult(std::string subsets[]);

const int ProcessEasyResults(std::vector<std::vector<PullResult>>& allResults);
const int ProcessHardResults(std::vector<std::vector<PullResult>>& allResults);

static constexpr int PUZZLE_PART = 2;

int main()
{
	// PARSE INPUT FILE //

	std::vector<std::string> list;

	if (parseInputFile(list) == 1)
	{
		return 1;
	}
		
	// SOLVE PUZZLE //

	std::vector<std::vector<PullResult>> allResults;
	std::string pullDelimiter = "; ";
	std::string subsets[3];

	for (int g = 0; g < list.size(); ++g)
	{
		std::vector<PullResult> gameResults;

		// Extract the draw data after the header.
		std::string game = list[g].substr(list[g].find(":") + 2);

		int pullEnd = 0;
		while (pullEnd >= 0)
		{
			// Extract the lead draw data.
			pullEnd = game.find(pullDelimiter);
			std::string pull = game.substr(0, pullEnd);	// if (pos == string::npos), substr() will read until end of string.

			// Extract each subset (value + color).
			FillSubsetData(pull, subsets);

			// Store values.
			gameResults.push_back(GetPullResult(subsets));

			// Remove draw data from game string (so we can just keep reading from beginning to delim).
			if (pullEnd != std::string::npos)
			{
				game = game.substr(pullEnd + pullDelimiter.length());
			}
		} 

		allResults.push_back(gameResults);
	}

	// Look through results. Only tally game ids if all pulls were valid.
	int totalValidSum{ 0 };	

	if (PUZZLE_PART == 1)
	{
		totalValidSum = ProcessEasyResults(allResults);
	}
	else
	{
		totalValidSum = ProcessHardResults(allResults);
	}


	std::cout << "The answer to part " << PUZZLE_PART << " is: " << totalValidSum << std::endl;
	
	return 0;
}


void FillSubsetData(std::string& pull, std::string subsets[])
{
	std::string componentDelimiter = ", ";

	for (int s = 0; s < 3; ++s)
	{
		int compEnd = pull.find(componentDelimiter);
		subsets[s] = pull.substr(0, compEnd);

		if (compEnd != std::string::npos)
		{
			pull = pull.substr(compEnd + componentDelimiter.length());
		}
	}
}


PullResult GetPullResult(std::string subsets[])
{
	std::string colorNames[3]{ "red", "green", "blue" };

	PullResult pullResult{ 0, 0, 0 };

	for (int s = 0; s < 3; ++s) // subsets
	{
		for (int c = 0; c < 3; ++c) // colors
		{
			int pos = subsets[s].find(colorNames[c]);

			if (pos != std::string::npos)
			{
				// Trim subsets[c] to just the value component.
				subsets[s] = subsets[s].substr(0, subsets[s].find(colorNames[c]) - 1);

				pullResult.colors[c] = std::atoi(subsets[s].c_str());

				// Check next subset.
				break;
			}
		}
	}

	return pullResult;
}


const int ProcessEasyResults(std::vector<std::vector<PullResult>>& allResults)
{
	int resultSum{0 };
	int validValues[3]{ 12, 13, 14 };

	for (int g = 0; g < allResults.size(); ++g)
	{
		bool isGameValid{ true };

		auto& gameResult = allResults[g];

		for (PullResult& pullResult : gameResult)
		{
			for (int c = 0; c < 3; ++c)
			{
				if (pullResult.colors[c] > validValues[c])
				{
					isGameValid = false;
					break;
				}
			}

			if (!isGameValid)
			{
				break;
			}
		}

		if (isGameValid)
		{
			resultSum += g + 1;
		}
	}

	return resultSum;
}


const int ProcessHardResults(std::vector<std::vector<PullResult>>& allResults)
{
	int resultSum{ 0 };

	for (int g = 0; g < allResults.size(); ++g)
	{
		auto& gameResult = allResults[g];

		int minColors[3]{ 0, 0, 0 };

		for (PullResult& pullResult : gameResult)
		{
			for (int c = 0; c < 3; ++c)
			{
				if (pullResult.colors[c] > minColors[c])
				{
					minColors[c] = pullResult.colors[c];
				}
			}
		}

		resultSum += minColors[0] * minColors[1] * minColors[2];
	}

	return resultSum;
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