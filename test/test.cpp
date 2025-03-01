#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "computron.h"

TEST_CASE("Valid word check", "[validWord]") {
    //check some instructions
    REQUIRE(validWord(1007));
    REQUIRE(validWord(2008));
    REQUIRE(validWord(4300));
    REQUIRE(validWord(1108));
    REQUIRE(validWord(3007));

    //check valid number values
    REQUIRE(validWord(0));
    REQUIRE(validWord(9999));
    REQUIRE(validWord(-9999));

    //out of bounds numbers
    REQUIRE_FALSE(validWord(10000));
    REQUIRE_FALSE(validWord(-10000));
}

TEST_CASE("Load memory from file", "[load_from_file]") {
    std::array<int, memorySize> memory{ 0 };
    REQUIRE_NOTHROW(load_from_file(memory, "p1.txt")); //load given text file

    //check every index for proper loading
    REQUIRE(memory[0] == 1007);
    REQUIRE(memory[1] == 1008);
    REQUIRE(memory[2] == 2007);
    REQUIRE(memory[3] == 3008);
    REQUIRE(memory[4] == 2109);
    REQUIRE(memory[5] == 1109);
    REQUIRE(memory[6] == 4300);
    for (int i = 7; i < 100; i++)
    {
        REQUIRE(memory[i] == 0);
    }

   
    //create file with invalid inputs to read
    std::ofstream outfile("test.txt");
    outfile << "+1007" << std::endl
            << "+2008" << std::endl
            << "+100000" << std::endl; //invalid word
    outfile.close();
    REQUIRE_THROWS_AS(load_from_file(memory, "test.txt"), std::runtime_error);

    //create file with invalid inputs to read
    std::ofstream outfile1("test1.txt");
    outfile1 << "+1007" << std::endl
        << "+2008" << std::endl
        << "-100000" << std::endl; //invalid word
    outfile1.close();
    REQUIRE_THROWS_AS(load_from_file(memory, "test1.txt"), std::runtime_error);
}

TEST_CASE("Execute full program", "[execute]") {
    std::array<int, memorySize> memory{ 0 }; //create registers
    int accumulator{ 0 };
    size_t instructionCounter{ 0 };
    int instructionRegister{ 0 };
    size_t operationCode{ 0 };
    size_t operand{ 0 };
    const std::vector<int> inputs{ 4, 5 };

    //load and execute
    load_from_file(memory, "p1.txt");
    execute(memory, &accumulator, &instructionCounter, &instructionRegister, &operationCode, &operand, inputs);

    //check loaded values
    REQUIRE(memory[7] == 4);
    REQUIRE(memory[8] == 5);
    
    //check correct loaded result
    REQUIRE(memory[9] == 9);

    //check stopped on halt
    REQUIRE(instructionCounter == 6);
    REQUIRE(instructionRegister == 4300);

    //check calculation registers
    REQUIRE(accumulator == 9);

    //check instruction decoding
    REQUIRE(operationCode == 43);
    REQUIRE(operand == 0);
}

TEST_CASE("Execute Operations", "[execute]") {

    SECTION("Memory Operations")
    {
        int accumulator{ 0 }; //create Registers
        size_t instructionCounter{ 0 };
        int instructionRegister{ 0 };
        size_t operationCode{ 0 };
        size_t operand{ 0 };
        const std::vector<int> inputs{ 5, 7 };

        //create custom instruction set
        std::array<int, memorySize> memory{ 0 };
        
        //Read
        memory[0] = 1007; //read 5 to mem[7]
        memory[1] = 1008; //read 7 to mem[8]
        //Write
        memory[2] = 1108; //write 7 to output (not active for submission)
        //Load
        memory[3] = 2007; //load 5 to accumulator
        //Store
        memory[4] = 2109; //write 5 to mem[9]
        memory[5] = 4300; //halt the program

        //execute
        execute(memory, &accumulator, &instructionCounter, &instructionRegister, &operationCode, &operand, inputs);

        //check read
        REQUIRE(memory[7] == 5);
        REQUIRE(memory[8] == 7);

        //check load
        REQUIRE(accumulator == 5);

        //check store
        REQUIRE(memory[9] == 5);
    }

    SECTION("Arithmetic Operations")
    {
        int accumulator{ 0 }; //create Registers
        size_t instructionCounter{ 0 };
        int instructionRegister{ 0 };
        size_t operationCode{ 0 };
        size_t operand{ 0 };
        const std::vector<int> inputs{ 10, 5 };

        //Test 
        std::array<int, memorySize> memory{ 0 };
        memory[0] = 1015; //read operands 10, 5
        memory[1] = 1016; 
        memory[2] = 2015; //load 10

        memory[3] = 3016; // 10+5=mem[17]
        memory[4] = 2117;

        memory[5] = 2016; //load 5
        memory[6] = 3115; // 5-10=mem[18]
        memory[7] = 2118;

        memory[8] = 2015; //load 10
        memory[9] = 3216; // 10/5=mem[19]
        memory[10] = 2119;

        memory[11] = 2015; //load 10
        memory[12] = 3316; // 10*5=mem[20]
        memory[13] = 2120;

        memory[14] = 4300; //halt

        //execute
        execute(memory, &accumulator, &instructionCounter, &instructionRegister, &operationCode, &operand, inputs);

        //check loaded inputs and registers
        REQUIRE(accumulator == 50);
        REQUIRE(memory[15] == 10);
        REQUIRE(memory[16] == 5);

        //check add output
        REQUIRE(memory[17] == 15);

        //check sub output
        REQUIRE(memory[18] == -5);

        //check div output
        REQUIRE(memory[19] == 2);

        //check mult output
        REQUIRE(memory[20] == 50);
    }

    SECTION("Overflow Arithmetic Operations")
    {
        int accumulator{ 0 }; //create Registers
        size_t instructionCounter{ 0 };
        int instructionRegister{ 0 };
        size_t operationCode{ 0 };
        size_t operand{ 0 };
        const std::vector<int> inputs{ 1000, 100 };

        //Test 
        std::array<int, memorySize> memory{ 0 };
        memory[0] = 1006; //read operands 1000, 100
        memory[1] = 1007;
        memory[2] = 2006; //load 1000

        memory[3] = 3307; // 1000*100= 100,000 > wordMax
        memory[4] = 2108;

        memory[5] = 4300; //halt

        //execute
        REQUIRE_THROWS_AS(execute(memory, &accumulator, &instructionCounter, &instructionRegister, &operationCode, &operand, inputs), std::runtime_error);

        //check loaded inputs and registers
        REQUIRE(accumulator == 1000);
        REQUIRE(instructionCounter == 3);
        REQUIRE(instructionRegister == 3307);
        REQUIRE(memory[8] == 0);
    }

    SECTION("Instruction Control Operations")
    {
        int accumulator{ 0 }; //create Registers
        size_t instructionCounter{ 0 };
        int instructionRegister{ 0 };
        size_t operationCode{ 0 };
        size_t operand{ 0 };
        const std::vector<int> inputs{ 5, -2, 0 };

        //Use memory[17] to check for branches
        //I use differnet branch structures to skip 
        //instructions that would write to mem[17]
        //if mem[17] is 0 after operation, all branches
        //completed successfully.
        std::array<int, memorySize> memory{ 0 };
        memory[0] = 1014; //load 5
        memory[1] = 1015; //load -2
        memory[2] = 1016; //load 0
        
        memory[3] = 2014; //5 to acc
        memory[4] = 4006; //BRANCH
        memory[5] = 2117; //store (should be skipped)

        memory[6] = 2015; //-2 to acc
        memory[7] = 4109; // BRANCH_NEG
        memory[8] = 2117; //store (should be skipped)

        memory[9] = 2016; //0 to acc
        memory[10] = 4213; //BRANCH_ZERO
        memory[11] = 2014; //reload 5 and store (skipped)
        memory[12] = 2117;

        memory[13] = 4300; //halt

        //execute
        execute(memory, &accumulator, &instructionCounter, &instructionRegister, &operationCode, &operand, inputs);

        //check registers
        REQUIRE(accumulator == 0);
        REQUIRE(instructionRegister == 4300);
        REQUIRE(instructionCounter == 13);

        //check memory[17] for modification
        REQUIRE(memory[17] == 0);
    }
}