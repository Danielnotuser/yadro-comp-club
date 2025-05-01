#include "compclub.h"
#include <iostream>
#include <sstream>

CompClub::CompClub(int table_num, const std::string &open, const std::string& close, int hour_price) : table_num(table_num), hour_price(hour_price)
{
    if (open.size() != 5 || open.find(':') != 2 || close.size() != 5 || close.find(':') != 2) throw std::runtime_error("Invalid open or close time.");
    try {
        int open_hours = std::stoi(open.substr(0, 2)), open_mins = std::stoi(open.substr(3, 2));
        int close_hours = std::stoi(close.substr(0, 2)), close_mins = std::stoi(close.substr(3, 2));
        open_time = std::make_pair(open_hours, open_mins);
        close_time = std::make_pair(close_hours, close_mins);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Invalid format of open or close time (HH:MM): " << e.what() << std::endl;
    }
}

void CompClub::generate_error(const std::string &time, const std::string &err)
{
    std::cout << time << 13 << err << std::endl;
}

bool CompClub::correct_time(const std::string &time)
{
    if (time.size() != 5 || time.find(':') != 2) return false;
    try {
        int hours = std::stoi(time.substr(0, 2)), mins = std::stoi(time.substr(3, 2));
        if (hours < 0 || hours > 23 || mins < 0 || mins > 59) { std::cerr << "Invalid number of hours or minutes."; return false;}
        if (previous_event_time.first != -1 && previous_event_time.second != -1)
            if (hours < previous_event_time.first || (hours == previous_event_time.first && mins < previous_event_time.second)) return false;
        else if (hours < open_time.first || (hours == open_time.first && mins < previous_event_time.second))
            generate_error(time, "NotOpenYet");
        previous_event_time = std::make_pair(hours, mins);
        return true;
    }
    catch(...) {
        return false;
    }
}

void CompClub::handle_event(const std::string &event)
{
    std::cout << event;
    std::istringstream iss(event);
    try {
        std::string time, client_name;
        int id;
        iss >> time;
        if (!correct_time(time)) throw;
        iss >> id >> client_name;
        switch (id)
        {
            case 1: client_arrive(client_name); break;
            case 2: int table; iss >> table; client_occupy(client_name, table); break;
            case 3: client_wait(client_name); break;
            case 4: client_leave(client_name); break;
            default: std::cerr << "Invalid ID number." << std::endl; throw;
        }
    }
    catch(const std::exception &e)
    {
        std::cerr << "Incorrect input format: " << e.what() << std::endl;
        throw;
    }
}

void CompClub::client_arrive(const std::string &name)
{

}