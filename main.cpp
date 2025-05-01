#include <iostream>
#include <fstream>
#include "lib/compclub.h"

int main(int argc, char *argv[]) {
    if (argc != 2)
        throw std::runtime_error("Invalid number of arguments.");
    std::ifstream file(argv[1], std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Failed to open the file.");
    else
    {
        int table_num, price;
        std::string open, close;
        file >> table_num >> open >> close >> price;
        CompClub club(table_num, open, close, price);
        file.ignore(); // игнорирование \n символа
        while (!file.eof())
        {
            std::string event;
            std::getline(file, event);
            club.handle_event(event);
        }
    }
    return 0;
}
