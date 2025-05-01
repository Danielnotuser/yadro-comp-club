#ifndef COMP_CLUB_COMPCLUB_H
#define COMP_CLUB_COMPCLUB_H

#include <utility>
#include <vector>
#include <queue>
#include <unordered_map>
#include <memory>

class CompClub {
private:
    typedef struct Client {
        std::string name;
        int table;
        std::pair<int, int> start_time;
        explicit Client(const std::string &name) : name(name), table(-1), start_time(-1, -1) {};
        Client(const std::string &name, int table, const std::pair<int, int> &time) : name(name), table(table), start_time(time) {};
    } Client;

    std::pair<int, int> open_time, close_time;
    int table_num;
    int hour_price;
    std::vector<std::shared_ptr<Client>> tables;
    std::queue<std::shared_ptr<Client>> wait_q;
    std::unordered_map<std::string, std::shared_ptr<Client>> clients;

    std::pair<int, int> previous_event_time = {-1, -1};
    bool correct_time(const std::string&);

    void client_arrive(const std::string&);
    void client_occupy(const std::string&, int);
    void client_wait(const std::string&);
    void client_leave(const std::string&);

    void generate_error(const std::string&, const std::string&);
public:
    CompClub(int table_num, const std::string &open, const std::string& close, int hour_price);

    void handle_event(const std::string&);
};

#endif //COMP_CLUB_COMPCLUB_H
