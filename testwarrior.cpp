#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <cstring>

std::string PMARS = "../../pmars-0.9.4/src/pmars";
std::string WARRIORS = "../warriors_all/";

int main() {

    int scoreWarrior1 = 0;
    int scoreWarrior2 = 0;

    for (const auto& entry : std::filesystem::directory_iterator(WARRIORS)) {
        if (entry.is_regular_file() && entry.path().extension() == ".red") {
            std::string command = PMARS + " -F 4000 -r 3 -b warrior_10000gen.red "+entry.path().string()+" > result.txt";
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
            int scoreWar2 = 0;
            for (int i = line.size() - 1; i >= 0; --i) {
                if (std::isdigit(line[i])) {
                    scoreWar2 = scoreWar2 * 10 + (line[i] - '0');
                    found_number = true;
                } else if (found_number) {
                    break;
                }
            }
            resultFile.close();
            scoreWarrior1 += scoreWar1;
            scoreWarrior2 += scoreWar2;
        }
    }

    std::cout << "Own Points: " << scoreWarrior1 << std::endl;
    std::cout << "Opponent Points: " << scoreWarrior2  << std::endl;
    return 0;
}