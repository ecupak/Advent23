// Open and read input file.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <array>
#include <algorithm>
#include <map>


// 7 MINUTE RUN TIME FOR PART 2


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

	size_t cycle;
	size_t distance;
};

using NodeMap = std::map<std::string, Node>;

enum Direction
{
	Left = 'L',
	Right = 'R',
};

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


// PUZZLE FUNCS //

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


struct lowest_distance_key
{
	bool operator()(Node& nodeA, Node& nodeB)
	{
		return nodeA.distance < nodeB.distance;
	}
};


const size_t traverseManyNodes(std::string& desertMap, NodeMap& nodemap)
{
	// Find starting nodes.
	std::vector<Node> nodes;

	for (std::pair<const std::string, Node>& nodepair : nodemap)
	{
		if (nodepair.first[2] == 'A')
		{
			nodes.push_back(nodepair.second);
			nodes[nodes.size() - 1].cycle = 0;
			nodes[nodes.size() - 1].distance = 0;
			nodes[nodes.size() - 1].id = nodepair.first;
		}
	}

	// Find cycle of each node (steps until a Z is reached - should be cyclic).
	for (size_t i = 0; i < nodes.size(); ++i)
	{
		size_t stepsTaken = 0;

		Node& node = nodes[i];

		Node traversalNode{ node };

		while (true)
		{
			if (getNextMapMove(desertMap, stepsTaken) == Direction::Left)
			{
				traversalNode = nodemap[traversalNode.leftPath];
			}
			else
			{
				traversalNode = nodemap[traversalNode.rightPath];
			}

			if (traversalNode.id[2] == 'Z')
			{				
				node.cycle = stepsTaken;
				break;
			}
		}
	}

	// Advance the shortest "distance" by the node's cycle until all distances are equal. That is when all are on a Z node.
	std::sort(nodes.begin(), nodes.end(), lowest_distance_key());
	while (true)
	{
		// Apply cycle to shortest distance.
		nodes[0].distance += nodes[0].cycle;

		// See if all distances match (all on Z).
		bool allMatch = true;
		for (size_t i = 0; i < nodes.size() - 1; ++i)
		{
			if (nodes[i].distance != nodes[i + 1].distance)
			{
				allMatch = false;
				break;
			}
		}

		// Break if all on Z.
		if (allMatch)
		{
			break;
		}

		// If first node distance is longer than next node, sort all nodes again.
		if (nodes[0].distance > nodes[1].distance)
		{
			std::sort(nodes.begin(), nodes.end(), lowest_distance_key());
		}
	}

	// Return distance stepped.
	return nodes[0].distance;
}


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