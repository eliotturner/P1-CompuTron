#ifndef COMPUTRON_H
#define COMPUTRON_H

#include <iostream>
#include <array>
#include <string>
#include <vector>

constexpr size_t memorySize{ 100 };
constexpr int minWord{ -9999 };
constexpr int maxWord{ 9999 };

enum class Command {
	read = 10, write,
	load = 20, store,
	add = 30, subtract, divide, multiply,
	branch = 40, branchNeg, branchZero, halt
};

//Loads file into memory word by word
void load_from_file(std::array<int, memorySize>& memory, const std::string& filename);

//executes program loaded into memory
void execute(std::array<int, memorySize>& memory, int* const acPtr,
	size_t* const icPtr, int* const irPtr,
	size_t* const opCodePtr, size_t* const opPtr,
	const std::vector<int>& inputs
	);

//dump all memory data and register contents into console
void dump(std::array<int, memorySize>& memory, int* const acPtr,
	size_t instructionCounter, size_t instructionRegister,
	size_t operationCode, size_t operand);

//check valid word
bool validWord(int word);

//helper function for dump, prints output
void output(std::string label, int width, int value, bool sign);

#endif