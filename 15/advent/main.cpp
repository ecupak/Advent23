// Open and read input file.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <set>
#include <map>

// DIFFICULTY
static constexpr int PUZZLE_PART = 2;


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

void Algo(size_t& value)
{
	value *= 17;
	value %= 256;
}


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

	std::map<std::string, size_t> lookup;

	size_t answer = 0;

	for (int i = 0; i < list.size(); ++i)
	{
		std::string line = list[i] + ",";

		int pos = 0;
		int comma = line.find(',');

		std::string code = line.substr(pos, (comma - pos));
		std::string key = "";

		while (comma != std::string::npos)
		{
			// Total value of code.
			size_t value = 0;
			size_t workingValue = 0;

			// Track position in code.
			int c = 0;

			// Find saved answer.
			for (c = code.length(); c > 0; --c)
			{
				key = code.substr(0, c);
				auto it = lookup.find(key);

				if (it != lookup.end())
				{
					value += it->second;
					break;
				}
			}

			// Solve rest of code.
			for (; c < code.length(); ++c)
			{
				value += code[c];

				Algo(value);

				key = code.substr(0, c + 1);
				lookup[key] = value;

			}// Done with code.

			answer += value;

			// Get next code.
			pos = comma + 1;
			comma = line.find(',', pos);
			code = line.substr(pos, (comma - pos));

		}// Done with entire loop.
	}

	std::cout << "The answer to part " << PUZZLE_PART << " is: " << answer << std::endl;
	
	return 0;
}