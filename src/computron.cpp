#include "computron.h"

#include <fstream>
#include <iomanip>

void load_from_file(std::array<int, memorySize>& memory, const std::string& filename)
{
	constexpr int sentinel{ -99999 };
	size_t i{ 0 };
	std::string line;
	int instruction;

	std::ifstream inputFile(filename);
	if (!inputFile)
	{
		throw std::runtime_error("invalid_input");
	}

	while (std::getline(inputFile, line))
	{
		instruction = std::stoi(line);
		if (instruction == sentinel)
			break;

		if (validWord(instruction))
		{
			memory[i] = instruction;
			i++;
		}
		else
			throw std::runtime_error("invalid_input");
	}

	inputFile.close();
}

Command opCodeToCommand(size_t opCode)
{
	switch (opCode)
	{
		case 10: return Command::read; break;
		case 11: return Command::write; break;
		case 20: return Command::load; break;
		case 21: return Command::store; break;
		case 30: return Command::add; break;
		case 31: return Command::subtract; break;
		case 32: return Command::divide; break;
		case 33: return Command::multiply; break;
		case 40: return Command::branch; break;
		case 41: return Command::branchNeg; break;
		case 42: return Command::branchZero; break;
		case 43: return Command::halt; break;
		default: return Command::halt;
	}
}

void execute(std::array<int, memorySize>& memory, int* const acPtr,
	size_t* const icPtr, int* const irPtr,
	size_t* const opCodePtr, size_t* const opPtr,
	const std::vector<int>& inputs)
{
	size_t inputIndex{ 0 }; //Tracks input

	do
	{
		*irPtr = memory[*icPtr];
		*opCodePtr = *irPtr / 100;
		*opPtr = *irPtr % 100;

		switch (int word{}; opCodeToCommand(*opCodePtr))
		{
			case Command::read:
				word = inputs[inputIndex];
				memory[*opPtr] = word;
				++(*icPtr);
				inputIndex++;
				break;
			case Command::write:
				++(*icPtr);
				//std::cout << "Contents of " << std::setfill('0') << std::setw(4);
				//std::cout << *opPtr << " : " << memory[*opPtr] << "\n";
				break;
			case Command::load:
				*acPtr = memory[*opPtr];
				++(*icPtr);
				break;
			case Command::store:
				memory[*opPtr] = *acPtr;
				++(*icPtr);
				break;
			case Command::add:
				word = *acPtr + memory[*opPtr];
				if (validWord(word))
				{
					*acPtr = word;
					++(*icPtr);
				}
				else
					throw std::runtime_error("invalid_input");
				break;
			case Command::subtract:
				word = *acPtr - memory[*opPtr];
				if (validWord(word))
				{
					*acPtr = word;
					++(*icPtr);
				}
				else
					throw std::runtime_error("invalid_input");
				break;
			case Command::multiply:
				word = *acPtr * memory[*opPtr];
				if (validWord(word))
				{
					*acPtr = word;
					++(*icPtr);
				}
				else
					throw std::runtime_error("invalid_input");
				break;
			case Command::divide:
				if (memory[*opPtr] == 0)
					throw std::runtime_error("invalid_input");
				word = *acPtr / memory[*opPtr];
				if (validWord(word))
				{
					*acPtr = word;
					++(*icPtr);
				}
				else
					throw std::runtime_error("invalid_input");
				break;
			case Command::branch:
				*icPtr = *opPtr;
				break;
			case Command::branchNeg:
				*acPtr < 0 ? *icPtr = *opPtr : ++(*icPtr);
				break;
			case Command::branchZero:
				*acPtr == 0 ? *icPtr = *opPtr : ++(*icPtr);
				break;
			case Command::halt:
				//dump(memory, acPtr, *icPtr, *irPtr, *opCodePtr, *opPtr);
				break;
			default:
				//any instruction necessary?
				break;
		}

	} while (opCodeToCommand(*opCodePtr) != Command::halt);
}

void dump(std::array<int, memorySize>& memory, int* const acPtr,
	size_t instructionCounter, size_t instructionRegister,
	size_t operationCode, size_t operand)
{
	std::cout << "Memory:" << "\n";
	//print column labels
	std::cout << "    "; //pad
	for (int col = 0; col < 10; col++)
		std::cout << col << "    ";
	std::cout << "\n";

	for (int row = 0; row < 10; ++row) {
		//print row label
		std::cout << std::setw(2) << row * 10 << " ";

		// print memory in row
		for (int col = 0; col < 10; ++col)
		{
			int val = memory[row * 10 + col];
			if (val < 0)
				std::cout << "-" << std::setw(4) << std::setfill('0') << abs(memory[row * 10 + col]);
			else
				std::cout << "+" << std::setw(4) << std::setfill('0') << memory[row * 10 + col];
		}

		std::cout << "\n";
	}
}

bool validWord(int word)
{
	//check bounds
	if (word > maxWord || word < minWord)
		return false;

	return true;
}

void output(std::string label, int width, int value, bool sign)
{
	std::cout << label << "\t\t";
	if (sign)
		std::cout << '+' << std::setw(width) << value;
	else
		std::cout << '-' << std::setw(width) << value;
}