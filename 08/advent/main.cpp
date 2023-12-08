// Open and read input file.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <array>
#include <algorithm>
#include <map>


// DIFFICULTY
static constexpr int PUZZLE_PART = 1;


// QOL
struct Node
{
	Node() : id{ "" } {}
	Node(std::string id) : id{ id } {}

	std::string id{ "" };
	std::string leftPath{ "" };
	std::string rightPath{ "" };
};

using NodeMap = std::map<std::string, Node>;

enum Direction
{
	Left = 'L',
	Right = 'R',
};


// EVERGREEN FUNCS
const int parseInputFile(std::vector<std::string>& list);
const size_t convertCharToInt(std::string intAsChars);

// PUZZLE FUNCS
void createDesertMap(std::vector<std::string>& list, std::string& desertMap);
void createNodes(std::vector<std::string>& list, NodeMap& nodemap);
const size_t traverseNodes(std::string& desertMap, NodeMap& nodemap);
const size_t traverseManyNodes(std::string& desertMap, NodeMap& nodemap);
Direction getNextMapMove(std::string& desertMap, size_t& stepsTaken);


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
	std::string desertMap;
	NodeMap nodemap;
	
	createDesertMap(list, desertMap);
	createNodes(list, nodemap);

	if (PUZZLE_PART == 1)
	{		
		answer = traverseNodes(desertMap, nodemap);
	}
	else
	{
		answer = traverseManyNodes(desertMap, nodemap);
	}

	std::cout << "The answer to part " << PUZZLE_PART << " is: " << answer << std::endl;

	return 0;
}


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
const size_t convertCharToInt(std::string intAsChars)
{
	size_t value{ 0 };

	for (size_t index{ 0 }, exponent{ intAsChars.length() - 1 }; index < intAsChars.length(); ++index, --exponent)
	{
		int num{ intAsChars[index] - '0' };

		value += static_cast<size_t>(num * pow(10, exponent));
	}

	return value;
}


void createDesertMap(std::vector<std::string>& list, std::string& desertMap)
{
	for (size_t i = 0; i < list.size(); ++i)
	{
		std::string& line = list[i];

		line = line.substr(0, line.find(' '));

		if (line.length() == 0)
		{
			break;
		}

		desertMap.append(line);

		line.clear();
	}
}


void createNodes(std::vector<std::string>& list, NodeMap& nodemap)
{
	// Find start of nodes.
	size_t start = 0;
	for (size_t n = 0; n < list.size(); ++n)
	{
		if (list[n].length() > 0)
		{
			start = n;
			break;
		}
	}

	// Start on nodes.
	for (size_t n = start; n < list.size(); ++n)
	{
		std::string& line = list[n];

		std::string id = line.substr(0, 3);

		// Make node entry.
		if (nodemap.count(id) == 0)
		{
			nodemap[id] = Node{ id };
		}

		Node& node = nodemap[id];
		
		// Connect paths.
		std::string leftPath = line.substr(line.find_first_of('(') + 1, 3);
		node.leftPath = leftPath;
		
		std::string rightPath = line.substr(line.find_first_of(')') - 3, 3);
		node.rightPath = rightPath;
	}
}


const size_t traverseNodes(std::string& desertMap, NodeMap& nodemap)
{
	size_t stepsTaken = 0;

	Node node = nodemap["AAA"];

	while (true)
	{
		if (getNextMapMove(desertMap, stepsTaken) == Direction::Left)
		{
			node = nodemap[node.leftPath];
		}
		else
		{
			node = nodemap[node.rightPath];
		}

		if (node.id == "ZZZ")
		{
			break;
		}
	}

	return stepsTaken;
}


const size_t traverseManyNodes(std::string& desertMap, NodeMap& nodemap)
{
	size_t stepsTaken = 0;

	for (auto& node : nodemap)
	{
		if (doesIdEndWith('A', node.second.id))
		{
			 
		}
	}
	size_t nodeCount = 
	Node node = nodemap["AAA"];

	while (true)
	{
		if (getNextMapMove(desertMap, stepsTaken) == Direction::Left)
		{
			node = nodemap[node.leftPath];
		}
		else
		{
			node = nodemap[node.rightPath];
		}

		if (node.id == "ZZZ")
		{
			break;
		}
	}

	return stepsTaken;
}


bool doesIdEndWith(char letter, std::string& id)
{
	return id[3] == letter;
}


Direction getNextMapMove(std::string& desertMap, size_t& stepsTaken)
{
	Direction direction;

	size_t loopStep = stepsTaken % desertMap.length();

	if (desertMap[loopStep] == 'L')
	{
		direction = Direction::Left;
	}
	else
	{
		direction = Direction::Right;
	}

	++stepsTaken;

	return direction;
}