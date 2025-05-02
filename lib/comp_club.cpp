#include "comp_club.h"
#include <iostream>
#include <sstream>

std::pair<int, int> CompClub::str_to_time(const std::string &time)
{
    int hours = std::stoi(time.substr(0, 2)), mins = std::stoi(time.substr(3, 2));
    return std::make_pair(hours, mins);
}

std::string CompClub::time_to_str(const std::pair<int, int> &time)
{
    int hours = time.first, mins = time.second;
    std::string res;
    if (hours / 10 == 0)
        res += "0" + std::to_string(hours);
    else
        res += std::to_string(hours);
    res += ":";
    if (mins / 10 == 0)
        res += "0" + std::to_string(mins);
    else
        res += std::to_string(mins);
    return res;
}

CompClub::CompClub(int table_num, const std::string &open, const std::string& close, int hour_price) : table_num(table_num), hour_price(hour_price)
{
    if (open.size() != 5 || open.find(':') != 2 || close.size() != 5 || close.find(':') != 2)
        throw std::runtime_error("Invalid open or close time.");
    open_time = str_to_time(open);
    close_time = str_to_time(close);
    revenue = std::vector<int>(table_num, 0);
    occupy_time = std::vector<std::pair<int, int>>(table_num, std::make_pair(0, 0));
    previous_event_time = {-1, -1};
    std::cout << open << std::endl;
}

void CompClub::generate_error(const std::string &time, int error)
{
    std::cout << time << " 13 ";
    switch(error)
    {
        case NotOpenYet: std::cout << "NotOpenYet" << std::endl; break;
        case YouShallNotPass: std::cout << "YouShallNotPass" << std::endl; break;
        case PlaceIsBusy: std::cout << "PlaceIsBusy" << std::endl; break;
        case ClientUnknown: std::cout << "ClientUnknown" << std::endl; break;
        case ICanWaitNoLonger: std::cout << "ICanWaitNoLonger!" << std::endl; break;
    }
}

bool CompClub::correct_time(std::pair<int, int> time)
{
    if (time.first < 0 || time.first > 23 || time.second < 0 || time.second > 59)
        throw std::runtime_error("Invalid number of hours or minutes.");
    if (previous_event_time.first != -1 && previous_event_time.second != -1) {
        if (time.first < previous_event_time.first ||
        (time.first == previous_event_time.first && time.second < previous_event_time.second))
            throw std::runtime_error("The event time does not follow the previous one.");
    }
    else if (time.first < open_time.first || (time.first == open_time.first && time.second < open_time.second))
        return false;
    previous_event_time = time;
    return true;
}

int CompClub::handle_event(const std::string &event)
{
    std::cout << event << std::endl;
    std::istringstream iss(event);
    try {
        std::string str_time, client_name;
        int id;
        iss >> str_time;
        if (str_time.size() != 5 || str_time.find(':') != 2)
            throw std::runtime_error("Invalid time format.");
        std::pair<int, int> time = str_to_time(str_time);
        if (!correct_time(time)) {
            generate_error(str_time, NotOpenYet);
            return 0;
        }
        iss >> id >> client_name;
        int err;
        switch (id)
        {
            case 1: err = client_arrive(client_name); break;
            case 2: int table; iss >> table; err = client_occupy(time, client_name, table); break;
            case 3: err = client_wait(time, client_name); break;
            case 4: err = client_leave(time, client_name); break;
            default: throw std::invalid_argument("Invalid ID number.");
        }
        if (err) generate_error(str_time, err);
        return 0;
    }
    catch(const std::exception &e) {
        std::cerr << "Incorrect event input format: " << e.what() << std::endl;
        return 1;
    }
}

void CompClub::generate_occupy(std::pair<int, int> time, int table)
{
    std::shared_ptr<Client> cl = wait_q.front(); wait_q.pop();
    cl->table = table;
    cl->start_time = time;
    tables[table] = cl;
    std::cout << time_to_str(time) << " 12 " << cl->name << " " << table << std::endl;
}

void CompClub::generate_leave(std::pair<int, int> time, const std::shared_ptr<Client>& cl)
{
    if (cl->table)
        table_leave(time, cl);
    std::cout << time_to_str(time) << " 11 " << cl->name << std::endl;
    clients.erase(cl->name);
}

void CompClub::table_leave(std::pair<int, int> end_time, const std::shared_ptr<Client>& cl)
{
    int hours = end_time.first - cl->start_time.first, mins;
    if (end_time.second > cl->start_time.second) {
        revenue[cl->table - 1] += (hours + 1) * hour_price;
        mins = end_time.second - cl->start_time.second;
    }
    else {
        revenue[cl->table - 1] += hours-- * hour_price;
        mins = (60 + end_time.second) - cl->start_time.second;
    }
    occupy_time[cl->table - 1].first += hours + (occupy_time[cl->table-1].second + mins) / 60;
    occupy_time[cl->table - 1].second = (occupy_time[cl->table-1].second + mins) % 60;
    tables.erase(cl->table);
    if (!wait_q.empty())
        generate_occupy(end_time, cl->table);
}

int CompClub::client_arrive(const std::string &name)
{
    if (clients.contains(name))
        return YouShallNotPass;
    clients[name] = std::make_shared<Client>(name);
    return 0;
}

int CompClub::client_occupy(std::pair<int, int> time, const std::string &name, int table)
{
    if (!clients.contains(name))
        return ClientUnknown;
    if (table > table_num)
        throw std::invalid_argument("Table number is out of range.");
    if (tables.contains(table))
        return PlaceIsBusy;
    std::shared_ptr<Client> cl = clients[name];
    if (cl->table)
        table_leave(time, cl);
    tables[table] = cl;
    cl->table = table;
    cl->start_time = time;
    return 0;
}

int CompClub::client_wait(std::pair<int, int> time, const std::string &name)
{
    if (!clients.contains(name))
        return ClientUnknown;
    if (tables.size() < table_num)
        return ICanWaitNoLonger;
    if (wait_q.size() + 1 > table_num) {
        generate_leave(time, clients[name]);
        return 0;
    }
    wait_q.push(clients[name]);
    return 0;
}

int CompClub::client_leave(std::pair<int, int> time, const std::string &name)
{
    if (!clients.contains(name))
        return ClientUnknown;
    if (clients[name]->table)
        table_leave(time, clients[name]);
    clients.erase(name);
    return 0;
}

void CompClub::close()
{
    for (auto & client : clients)
        generate_leave(close_time, client.second);
    std::cout << time_to_str(close_time) << std::endl;
    for (int t = 0; t < table_num; t++) {
        std::cout << t + 1 << " " << revenue[t] << " " << time_to_str(occupy_time[t]) << std::endl;
    }
}
