#ifndef COMP_CLUB_COMP_CLUB_H
#define COMP_CLUB_COMP_CLUB_H

#include <utility>
#include <vector>
#include <queue>
#include <unordered_map>
#include <map>
#include <memory>

class CompClub {
private:
    typedef struct Client {
        std::string name;
        int table;
        std::pair<int, int> start_time;
        Client() = default;
        explicit Client(const std::string &name) : name(name), table(0), start_time(-1, -1) {};
        Client(const std::string &name, int table, const std::pair<int, int> &time) : name(name), table(table), start_time(time) {};
    } Client;

    enum EventError {NotOpenYet = 1, YouShallNotPass, PlaceIsBusy, ClientUnknown, ICanWaitNoLonger};

    std::pair<int, int> open_time, close_time;
    int table_num;
    int hour_price;
    std::unordered_map<int, std::shared_ptr<Client>> tables;
    std::vector<int> revenue;
    std::vector<std::pair<int, int>> occupy_time;
    std::queue<std::shared_ptr<Client>> wait_q;
    std::map<std::string, std::shared_ptr<Client>> clients;

    std::pair<int, int> previous_event_time = {-1, -1};
    static std::pair<int, int> str_to_time(const std::string &);
    static std::string time_to_str(const std::pair<int, int>&);
    bool correct_time(const std::string&);
    void table_leave(std::pair<int, int>, const std::shared_ptr<Client>&);

    int client_arrive(const std::string&);
    int client_occupy(const std::string&, const std::string&, int);
    int client_wait(const std::string &, const std::string&);
    int client_leave(const std::string&, const std::string&);

    static void generate_error(const std::string&, int);
    void generate_occupy(std::pair<int, int>, int);
    void generate_leave(std::pair<int, int>, const std::shared_ptr<Client>&);
public:
    CompClub(int table_num, const std::string &open, const std::string& close, int hour_price);

    int handle_event(const std::string&);

    ~CompClub();
};

#endif //COMP_CLUB_COMP_CLUB_H
