#include <iostream>
#include <fstream>
#include <limits>
#include "lib/comp_club.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }
    std::ifstream file(argv[1], std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << argv[1] << "'" << std::endl;
        return 1;
    }
    int table_num, price;
    std::string open, close;
    try {
        file >> table_num >> open >> close >> price;
        CompClub club(table_num, open, close, price);
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // игнорирование \n символа
        for (std::string event; std::getline(file, event);) {
            if (club.handle_event(event)) return 1;
        }
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
