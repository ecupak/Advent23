// Open and read input file.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <utility>

using event = std::pair<size_t, size_t>;


static constexpr int PUZZLE_PART = 2;

const int parseInputFile(std::vector<std::string>& list);

void GetRaceEvents(std::vector<std::string>& list, std::vector<event>& races);
void GetRaceEvent(std::vector<std::string>& list, std::vector<event>& races);

std::string GetNextNumber(int& pos, std::string& line);
void GetWinningCombos(std::vector<event>&  races, std::vector<size_t>& winningCombos);
const size_t GetMarginOfError(std::vector<size_t>& winningCombos);

const size_t ConvertCharToInt(std::string intAsChars);

const int ProcessEasyResults();
const int ProcessHardResults();


int main()
{
	// PARSE INPUT FILE //

	std::vector<std::string> list;

	if (parseInputFile(list) == 1)
	{
		return 1;
	}
		
	size_t answer = 0;

	// SOLVE PUZZLE //

	std::vector<event> races;

	if (PUZZLE_PART == 1)
	{
		GetRaceEvents(list, races);		
	}
	else
	{
		GetRaceEvent(list, races);
	}

	std::vector<size_t> winningCombos;

	GetWinningCombos(races, winningCombos);
	
	answer = GetMarginOfError(winningCombos);

	if (PUZZLE_PART == 1)
	{
		std::cout << "The answer to part " << PUZZLE_PART << " is: " << answer << std::endl;
	}
	else
	{
		std::cout << "The answer to part " << PUZZLE_PART << " is: " << answer << std::endl;
	}

	return 0;
}


void GetRaceEvents(std::vector<std::string>& list, std::vector<event>& races)
{
	// Find the first line and create the events.
	{
		std::string& line = list[0];

		int pos = line.find(':');

		while (pos < line.length())
		{
			std::string strNumber = GetNextNumber(pos, line);
			
			if (strNumber.length() > 0)
			{
				races.push_back({ ConvertCharToInt(strNumber), 0 });			
			}
		}
	}
		
	// Find the secondline and add to the events.
	{
		int eventId = 0;

		std::string& line = list[1];

		int pos = line.find(':');

		while (pos < line.length())
		{
			std::string strNumber = GetNextNumber(pos, line);

			if (strNumber.length() > 0)
			{
				races[eventId++].second = ConvertCharToInt(strNumber);
			}
		}
	}
}


void GetRaceEvent(std::vector<std::string>& list, std::vector<event>& races)
{
	// Find the first line and create the events.
	{
		std::string& line = list[0];

		int pos = line.find(':');
		std::string strNumber = "";

		while (pos < line.length())
		{
			strNumber.append(GetNextNumber(pos, line));
		}

		if (strNumber.length() > 0)
		{
			races.push_back({ ConvertCharToInt(strNumber), 0 });
		}
	}

	// Find the secondline and add to the events.
	{
		int eventId = 0;

		std::string& line = list[1];

		int pos = line.find(':');
		std::string strNumber = "";

		while (pos < line.length())
		{
			strNumber.append(GetNextNumber(pos, line));
		}

		if (strNumber.length() > 0)
		{
			races[eventId++].second = ConvertCharToInt(strNumber);
		}
	}
}


std::string GetNextNumber(int& pos, std::string& line)
{
	std::string strNumber;
	
	// Find next number.
	for (; pos < line.length(); ++pos)
	{
		if (std::isdigit(line[pos]))
		{
			break;
		}
	}

	// Load number.
	for (; pos < line.length(); ++pos)
	{
		if (std::isdigit(line[pos]))
		{
			strNumber.push_back(line[pos]);
		}
		else
		{
			break;
		}
	}

	// Return converted number.
	return strNumber;
}


void GetWinningCombos(std::vector<event>& races, std::vector<size_t>& winningCombos)
{
	for (int e = 0; e < races.size(); ++e)
	{	
		size_t shortWinTime = 0;
		size_t longWinTime = 0;

		size_t totalTime = races[e].first;
		size_t bestDistance = races[e].second;

		// Short win.
		for (size_t timePressed = 0; timePressed < totalTime; ++timePressed)
		{
			size_t timeLeft = totalTime - timePressed;

			size_t myDistance = timePressed * timeLeft; // timePressed = speed

			if (myDistance > bestDistance)
			{
				shortWinTime = timePressed;
				break;
			}
		}

		if (shortWinTime > 0)
		{
			// Long win.
			for (size_t timePressed = totalTime; timePressed >= shortWinTime; --timePressed)
			{
				size_t timeLeft = totalTime - timePressed;

				size_t myDistance = timePressed * timeLeft; // timePressed = speed

				if (myDistance > bestDistance)
				{
					longWinTime = timePressed;
					break;
				}
			}
		}

		if (shortWinTime > 0)
		{
			winningCombos.push_back(longWinTime - shortWinTime + 1);
		}
		else
		{
			winningCombos.push_back(0);
		}
	}
}


const size_t GetMarginOfError(std::vector<size_t>& winningCombos)
{
	size_t moe = 1;

	for (size_t& wins : winningCombos)
	{
		moe *= wins;
	}

	return moe;
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