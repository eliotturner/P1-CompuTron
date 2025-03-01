#include "computron.h"

#include <fstream>
#include <iomanip>

void load_from_file(std::array<int, memorySize>& memory, const std::string& filename)
{
	//set vars
	constexpr int sentinel{ -99999 };
	size_t i{ 0 };
	std::string line;
	int instruction;

	//open file and check open
	std::ifstream inputFile(filename);
	if (!inputFile)
	{
		throw std::runtime_error("invalid_input");
	}

	//get each line
	while (std::getline(inputFile, line))
	{
		//extract instruction and check for sentinel
		instruction = std::stoi(line);
		if (instruction == sentinel)
			break;

		//add valid words to memory, exit if invalid word
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
	//return relevant enum element for each opcode
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
		//extract full instruction, opcode, and operand
		*irPtr = memory[*icPtr];
		*opCodePtr = *irPtr / 100;
		*opPtr = *irPtr % 100;

		//switch based on opcode
		switch (int word{}; opCodeToCommand(*opCodePtr))
		{
			case Command::read:
				word = inputs[inputIndex]; //read input
				memory[*opPtr] = word; //write to mem
				++(*icPtr); 
				inputIndex++;
				break;
			case Command::write:
				++(*icPtr); 
				//std::cout << "Contents of " << std::setfill('0') << std::setw(4);
				//std::cout << *opPtr << " : " << memory[*opPtr] << "\n";
				break;
			case Command::load:
				*acPtr = memory[*opPtr]; //write mem to acc
				++(*icPtr);
				break;
			case Command::store:
				memory[*opPtr] = *acPtr; //write acc to mem
				++(*icPtr);
				break;
			case Command::add:
				word = *acPtr + memory[*opPtr]; //do operation
				if (validWord(word)) //check valid & write acc
				{
					*acPtr = word;
					++(*icPtr);
				}
				else
					throw std::runtime_error("invalid_input");
				break;
			case Command::subtract:
				word = *acPtr - memory[*opPtr]; //do operation
				if (validWord(word)) //check valid & write acc
				{
					*acPtr = word;
					++(*icPtr);
				}
				else
					throw std::runtime_error("invalid_input");
				break;
			case Command::multiply:
				word = *acPtr * memory[*opPtr]; //do operation
				if (validWord(word)) //check valid & write acc
				{
					*acPtr = word;
					++(*icPtr);
				}
				else
					throw std::runtime_error("invalid_input");
				break;
			case Command::divide:
				if (memory[*opPtr] == 0) //div-by-zero check
					throw std::runtime_error("invalid_input");
				word = *acPtr / memory[*opPtr]; //do operation
				if (validWord(word)) //check valid & write acc
				{
					*acPtr = word;
					++(*icPtr);
				}
				else
					throw std::runtime_error("invalid_input");
				break;
			case Command::branch:
				*icPtr = *opPtr; //update instruction counter
				break;
			case Command::branchNeg:
				*acPtr < 0 ? *icPtr = *opPtr : ++(*icPtr); //check negative, then branch
				break;
			case Command::branchZero:
				*acPtr == 0 ? *icPtr = *opPtr : ++(*icPtr); //check zero, then branch
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
	//print top-label
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
			//check sign and print accordingly
			int val = memory[row * 10 + col];
			if (val < 0)
				std::cout << "-" << std::setw(4) << std::setfill('0') << abs(memory[row * 10 + col]);
			else
				std::cout << "+" << std::setw(4) << std::setfill('0') << memory[row * 10 + col];
		}

		std::cout << "\n";
	}

	//print register contents using output function
	std::cout << '\n' << "Registers" << '\n';
	output("Accumulator", 4, *acPtr, true);
	output("instructionCounter", 2, instructionCounter, false);
	output("instructionRegister", 4, instructionRegister, true);
	output("operationCode", 2, operationCode, false);
	output("operand", 2, operand, false);
	std::cout << '\n';
}

bool validWord(int word)
{
	//check for outside word bounds
	if (word > maxWord || word < minWord)
		return false;

	return true;
}

void output(std::string label, int width, int value, bool sign)
{
	//print the label with a constant width for formatting
	std::cout << std::setw(22) << std::setfill(' ') << std::left << label << "\t";
	
	//add sign if necessary
	if (sign)
	{
		if (value < 0)
			std::cout << "-";
		else
			std::cout << "+";
	}

	//add zero-filled value (signless)
	std::cout << std::setfill('0') << std::setw(width) << abs(value) << '\n';

	//reset cout formatting
	std::cout << std::right;
}