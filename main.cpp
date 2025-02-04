#include <iostream>
#include <random>
#include <ga/GA2DArrayGenome.h>
#include <ga/garandom.h>
#include <ga/ga.h>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <filesystem>
#include "config.h"

// Random
std::random_device rd;
std::mt19937 gen(rd());

// Paths
const std::string PMARS = PMARS_CMAKE;// = "../../pmars-0.9.4/src/pmars";
const std::string WARRIORS = "../warriors_all/";    // Path for Opponents
//constexpr bool EXT_OPPONENT = false;    // True if already available external warriors are used

const std::vector<std::string> OPCODES = {"dat","mov","add","sub","mul","div","mod","jmp","jmz","jmn","djn","spl","slt","seq","sne","nop"};
//const std::vector<std::string> OPCODES = {"dat","mov","add","sub","jmp","spl"};   // Reduced OPCODES
const std::vector<std::string> MODIFIERS = {".a ",".b ",".ab",".ba",".f ",".x ",".i "};
const std::vector<std::string> ADRESS_MODES = {"","", "#","@", "$","<",">","*","{","}"};
//const std::vector<std::string> ADRESS_MODES = {"","", "#","@"};   // Reduced Adress Modes
constexpr int NUM_BATTLES = 1;
constexpr int POPULATION_SIZE = 10;
constexpr int MAX_GENERATIONS = 1000;
constexpr int MAX_INSTRUCTIONS = 10; // Codelines of Warrior
constexpr float REPLACEMENT_RATE = 0.6;
constexpr float MUTATION_RATE = 0.5;
constexpr float CROSSOVER_RATE = 0.5;


std::string createWarriorString(GAGenome& g, std::string& name) {
    // Creates the String that than is written into a .red file
    auto&  genome = (GA2DArrayGenome<int>&)g;
    std::string warrior;
    warrior += ";redcode-94\n";
    warrior += ";name "+name+"\n";
    warrior += ";assert CORESIZE==8000\n";
    for (int i=0; i<MAX_INSTRUCTIONS;++i) {
        warrior +=OPCODES[genome.gene(0,i)];
        warrior += MODIFIERS[genome.gene(1,i)];
        warrior.push_back(' ');
        warrior  += ADRESS_MODES[genome.gene(2,i)];
        warrior  += std::to_string(genome.gene(3,i));
        warrior  += ", ";
        warrior  += ADRESS_MODES[genome.gene(4,i)];
        warrior += std::to_string(genome.gene(5,i));
        warrior.push_back('\n');
    }
    warrior += "end";
    return warrior;
}

int runBattle(const std::string& warrior1, const std::string& pathWarrior2="") {

    // Save temp Warrior1
    std::ofstream file1("warrior1.red");
    file1 << warrior1;
    file1.close();

    // Only One Warrior --> Check if Warrior Code has a Syntax Error
    if (pathWarrior2.empty()) {
        std::string command = PMARS + " -A warrior1.red > result.txt";
        int result = system(command.c_str());
        return result;
    }

    // Run Battle
    std::string command = PMARS + " -F 4000 -r " + std::to_string(NUM_BATTLES) + " -b warrior1.red "+pathWarrior2+" > result.txt";
    int result = system(command.c_str());

    if (result!=0) {
        // Syntaxs Errors at warriors_all or wrong path,...
        std::cerr << "Error at Running a PMARS Battle!!" << std::endl;
        exit(1);
    }

    // Read Result
    std::ifstream resultFile("result.txt");
    if (!resultFile.is_open()) {
        std::cerr << "Error opening file result.txt!" << std::endl;
        exit(1);
    }
    std::string line;
    std::getline(resultFile, line);
    size_t end = line.find_last_not_of(" \t\r\n");
    if (end != std::string::npos) {
        line = line.substr(0, end + 1);
    }
    int scoreWar1 = 0;
    bool found_number = false;
    for (int i = line.size() - 1; i >= 0; --i) {
        if (std::isdigit(line[i])) {
            scoreWar1 = scoreWar1 * 10 + (line[i] - '0');
            found_number = true;
        } else if (found_number) {
            break;
        }
    }
    std::getline(resultFile, line);
    end = line.find_last_not_of(" \t\r\n");
    if (end != std::string::npos) {
        line = line.substr(0, end + 1);
    }
    /*int scoreWar2 = 0;
    for (int i = line.size() - 1; i >= 0; --i) {
        if (std::isdigit(line[i])) {
            scoreWar2 = scoreWar2 * 10 + (line[i] - '0');
            found_number = true;
        } else if (found_number) {
            break;
        }
    }*/
    resultFile.close();
    return scoreWar1;// - scoreWar2;
}

// Objective
float objective(GAGenome& g) {
    //std::cout << "Hallo" << std::endl;
    auto& genome = (GA2DArrayGenome<int>&)g;
    std::string name = "war1";
    std::string warrior = createWarriorString(genome, name);
    int countEnemy = 0;
    int countFitness = 0;
    for (const auto& entry : std::filesystem::directory_iterator(WARRIORS)) {
        if (entry.is_regular_file() && entry.path().extension() == ".red") {
            const int score = runBattle(warrior, entry.path().string());
            if (score == -9999) {
                // Error in Running PMARS
                return 0;
            }
            countFitness += score;
            countEnemy++;
        }
    }
    if (countEnemy == 0) {
        // Empty Folder --> appears before initialization of genome
        return 0;
    }
    return static_cast<float>(countFitness + 3 * NUM_BATTLES * countEnemy); // Add Constant so assure Return-Values is Positive
}

// Initializer
void initializer(GAGenome& g) {
    auto& genome = (GA2DArrayGenome<int>&)g;

    int result = -1;
    while (result != 0) {
        for (int i=0; i<MAX_INSTRUCTIONS;++i) {
            // Operationcode
            genome.gene(0, i, std::uniform_int_distribution<int>(0, OPCODES.size()-1)(gen));
            // Modifier
            genome.gene(1, i, std::uniform_int_distribution<int>(0, MODIFIERS.size()-1)(gen));
            // Adress-Mode Field A
            genome.gene(2, i, std::uniform_int_distribution<int>(0, ADRESS_MODES.size()-1)(gen));
            // Adress Field A -> 0-9
            genome.gene(3, i, std::uniform_int_distribution<int>(0, 9)(gen));
            // Adress-Mode Field B
            genome.gene(4, i, std::uniform_int_distribution<int>(0, ADRESS_MODES.size()-1)(gen));
            // Adress Field B -> 0-9
            genome.gene(5, i, std::uniform_int_distribution<int>(0, 9)(gen));
        }
        // Test Genome against Warrior that doesn't make anything useful --> PLANT WARRIOR
        std::string name = "war1";
        std::string warrior = createWarriorString(genome, name);

        result = runBattle(warrior);    // 0 == No Syntax Error in Warrior
    }
}

// Mutator
int mutator(GAGenome& g, float p) {
    // Mutator uses new values
    auto& genome = (GA2DArrayGenome<int>&)g;

    int result = -1;
    int nMutations = 0;
    while (result != 0) {
        for (int i = 0; i < genome.height(); i++) {
            if (GAFlipCoin(p)) {
                // Operationcode
                genome.gene(0, i, std::uniform_int_distribution<int>(0, OPCODES.size()-1)(gen));
                nMutations++;
            }
            if (GAFlipCoin(p)) {
                // Modifier
                genome.gene(1, i, std::uniform_int_distribution<int>(0, MODIFIERS.size()-1)(gen));
                nMutations++;
            }
            if (GAFlipCoin(p)) {
                // Adress-Mode Field A
                genome.gene(2, i, std::uniform_int_distribution<int>(0, ADRESS_MODES.size()-1)(gen));
                nMutations++;
            }
            if (GAFlipCoin(p)) {
                // Adress Field A -> 0-9
                genome.gene(3, i, std::uniform_int_distribution<int>(0, 9)(gen));
                nMutations++;
            }
            if (GAFlipCoin(p)) {
                // Adress-Mode Field B
                genome.gene(4, i, std::uniform_int_distribution<int>(0, ADRESS_MODES.size()-1)(gen));
                nMutations++;
            }
            if (GAFlipCoin(p)) {
                // Adress Field B -> 0-9
                genome.gene(5, i, std::uniform_int_distribution<int>(0, 9)(gen));
                nMutations++;
            }
        }
        std::string name = "war1";
        std::string warrior = createWarriorString(genome, name);
        result = runBattle(warrior);
    }
    return nMutations;
}

int crossover(const GAGenome& p1, const GAGenome& p2, GAGenome* c1, GAGenome* c2) {
    auto& parent1 = (GA2DArrayGenome<int>&)p1;
    auto& parent2 = (GA2DArrayGenome<int>&)p2;

    if (c1 && c2) {
        auto& child1 = (GA2DArrayGenome<int>&)*c1;
        auto& child2 = (GA2DArrayGenome<int>&)*c2;

        int result1 = -1;
        int result2 = -1;
        while (result1 != 0 || result2 != 0) {
            int cut = std::uniform_int_distribution<int>(0, MAX_INSTRUCTIONS)(gen);

            for (int i=0; i<cut; i++) {
                for (int w=0; w<parent1.width(); ++w) {
                    child1.gene(w, i, parent1.gene(w, i));
                    child2.gene(w, i, parent2.gene(w, i));
                }
            }
            for (int i=cut; i<parent1.height(); i++) {
                for (int w=0; w<parent1.width(); ++w) {
                    child1.gene(w, i, parent2.gene(w, i));
                    child2.gene(w, i, parent1.gene(w, i));
                }
            }
            std::string name1 = "child1";
            std::string warrior1 = createWarriorString(child1, name1);
            result1 = runBattle(warrior1);
            std::string name2 = "child2";
            std::string warrior2 = createWarriorString(child2, name2);
            result2 = runBattle(warrior2);
        }
        return 2;
    } else if (c1) {
        auto& child = (GA2DArrayGenome<int>&)*c1;
        int cut = std::uniform_int_distribution<int>(0, MAX_INSTRUCTIONS)(gen);

        int result = -1;
        while (result != 0) {
            for (int i=0; i<cut; i++) {
                for (int w=0; w<parent1.width(); ++w) {
                    child.gene(w, i, parent1.gene(w, i));
                }
            }
            for (int i=cut; i<parent1.height(); i++) {
                for (int w=0; w<parent1.width(); ++w) {
                    child.gene(w, i, parent2.gene(w, i));
                }
            }
            std::string name = "war1";
            std::string warrior = createWarriorString(child, name);
            result = runBattle(warrior);
        }
        return 1;
    } else {
        return 0;
    }
}

int main() {
    // Check if Path to PMARS exist
    std::filesystem::path pathPMARS(PMARS);
    if (!std::filesystem::exists(pathPMARS)) {
        std::cerr << "Path to PMARS doesn't exist: " << PMARS << std::endl;
        exit(1);
    }

    /*// XCreate warrior folder if not exist
    std::filesystem::path pathWARRIORS(WARRIORS);
    if (!std::filesystem::exists(pathWARRIORS)) {
        if (!EXT_OPPONENT) {
            // Opponents are created by alg
            if (!std::filesystem::create_directories(pathWARRIORS)) {
			    std::cerr << "The folder for Populations-Warriors could not be created." << std::endl;
			    exit(1);
		    }
	    } else {
	        std::cerr << "The folder for Populations-Warriors does not exist." << std::endl;
	        exit(1);
	    }
    }*/
    std::filesystem::path pathWARRIORS(WARRIORS);
    if (!std::filesystem::exists(pathWARRIORS)) {
        std::cerr << "The folder for Populations-Warriors does not exist." << std::endl;
        exit(1);
    }

    GA2DArrayGenome<int> genome(6, MAX_INSTRUCTIONS, objective);
	// 6 is number of changeable values in one command line --> {mov}.{ab} {#}{0}, {#}{1}
    genome.initializer(initializer);
    genome.mutator(mutator);
    genome.crossover(crossover);

    GASteadyStateGA ga(genome);
    ga.pReplacement(REPLACEMENT_RATE);
    ga.populationSize(POPULATION_SIZE);
    ga.nGenerations(MAX_GENERATIONS);
    ga.pMutation(MUTATION_RATE);
    ga.pCrossover(CROSSOVER_RATE);

    ga.initialize();

    std::cout << "Evolving..." <<std::endl;
     while(!ga.done()){
         /*if (!EXT_OPPONENT) {
             // Save Warrior for Fight
             for(int ii=0; ii<ga.population().size(); ii++) {
                 auto& genom  = ga.population().individual(ii);
                 std::string name = "War"+std::to_string(ii+1);
                 std::string warrior = createWarriorString(genom, name);
                 std::ofstream file1(WARRIORS+name+".red");
                 file1 << warrior;
                 file1.close();
             }
             ga.step();
             // Evaluate also old individuals
             for (int ii=0; ii<ga.population().size(); ii++) {
                 auto& genom  = ga.population().individual(ii);
                 genom.evaluate(gaTrue);
             }
             ga.population().scale();
         } else {*/
         ga.step();
         //}

        if(ga.generation() % 20 == 0){
            std::cout << "Generation: " << ga.generation()
                     << ", Best fitness: " << ga.statistics().bestIndividual().score() << std::endl;
        }
    }
    std::cout << "\n\n";

    GAGenome &bestGenome = (GA2DArrayGenome<int> &) ga.statistics().bestIndividual();
    std::string name = "BestWarrior";
    std::cout << "Best solution found: "<<bestGenome.score()<< std::endl;
    std::cout << "Warrior" << std::endl;
    std::cout << createWarriorString(bestGenome, name)<< std::endl;
    return 0;
};