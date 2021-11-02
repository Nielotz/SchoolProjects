#include <iostream>
#include <fstream>
#include <utility>
#include <string>

using namespace std;

class Ranking {
public:
    Ranking() {
        this->first = nullptr;
    }

    class Player {
    public:
        string name;
        string second_name;
        string country;
        int rank;
        int birthdate;
        Player *next;

        Player(string name, string secondName, string country, int rank, int birthdate)
                : name(std::move(name)), second_name(std::move(secondName)),
                  country(std::move(country)),
                  rank(rank),
                  birthdate(birthdate),
                  next(nullptr) {};

        Player(Player &player) : name(player.name), second_name(player.second_name),
                                 country(player.country),
                                 rank(player.rank),
                                 birthdate(player.birthdate),
                                 next(nullptr) {}

        void print() const {
            printf("name: %14s", name.c_str());
            printf("country: %11s, ", country.c_str());
            printf("rank: %d, ", rank);
            printf("birthdate: %d\n", birthdate);
        }
    };

    Player *first = nullptr;

    void push_back(Player *player) {
        if (first) {
            Player *new_player = first;
            while (new_player->next)
                new_player = new_player->next;
            new_player->next = new Player(*player);
        } else
            first = new Player(*player);
    }

    void remove_first() {
        Player *next = first->next;
        delete first;
        first = next;
    }

    void remove(Player *previous_to_removed) {
        Player *next = previous_to_removed->next->next;
        delete previous_to_removed->next;
        previous_to_removed->next = next;
    }

    void replace_first(Player *player) {
        auto new_first = new Player(*player);
        new_first->next = first->next;
        delete first;
        first = new_first;
    }

    /*
     * Assume list contain at least {position} + 1 elements.
     */
    void replace(Player *player, Player *previous_to_replaced) {
        auto copy_of_player = new Ranking::Player(*player);
        copy_of_player->next = previous_to_replaced->next->next;

        delete previous_to_replaced->next;
        previous_to_replaced->next = copy_of_player;
    }

    /*
 * Assume list contain at least {position} + 1 elements.
 */
    void replace(Player *player, size_t position) {
        auto copy_of_player = new Ranking::Player(*player);

        if (position) {
            Player *previous_to_player = first;
            for (int i = 0; i < position - 1; i++)
                previous_to_player = previous_to_player->next;

            copy_of_player->next = previous_to_player->next->next;
            delete previous_to_player->next;
            previous_to_player->next = copy_of_player;
        } else  // (position == 0)
        {
            copy_of_player->next = first->next;
            delete first;
            first = copy_of_player;
        }
    }


    void print() const {
        cout << "Ranking:" << endl;
        if (first) {
            Player *player;
            for (player = first; player->next; player = player->next) {
                cout << "\t";
                player->print();
            }
            cout << "\t";
            player->print();
        }
        cout << "End of ranking:" << endl;
    }

    ~Ranking() {
        Player *player = first;
        while (player) {
            Player *next_player = player->next;
            delete player;
            player = next_player;
        }
    }
};

void read_ranking_from_file(Ranking &ranking) {
    ifstream input_data("data.txt");

    for (string input_line; getline(input_data, input_line);) {
        // cout << "Reading line: " << input_line << endl;

        string strings[6];
        int i = 0;
        size_t last_space_pos = 0;
        size_t space_pos = input_line.find(' ');

        while (space_pos != std::string::npos) {
            strings[i] = input_line.substr(last_space_pos, space_pos - last_space_pos);
            // cout << "\tstrings[" << i << "]: " << strings[i] << endl;

            last_space_pos = space_pos + 1;  // Recompense space.
            space_pos = input_line.find(' ', last_space_pos);

            i++;
        }
        strings[i] = input_line.substr(last_space_pos, space_pos - last_space_pos);
        // cout << "\tstrings[" << i << "]: " << strings[i] << endl;


        if (i == 6)  // Contain second name.
            ranking.push_back(
                    new Ranking::Player(strings[0], strings[1],
                                        strings[2],
                                        stoi(strings[3]),
                                        stoi(strings[4])));
        else
            ranking.push_back(
                    new Ranking::Player(strings[0], "",
                                        strings[2],
                                        stoi(strings[3]),
                                        stoi(strings[4])));
    }
}

void select_players_with_score_above(Ranking &ranking, Ranking &ranking_out, int threshold) {
    if (ranking.first) {
        Ranking::Player *player = ranking.first;
        while (player->next) {
            if (player->rank > threshold)
                ranking_out.push_back(player);
            player = player->next;
        }
    }
}

void select_best_players_per_country(Ranking &ranking, Ranking &ranking_out) {
    if (ranking.first) {
        // Walk over players in main ranking.
        ranking_out.push_back(ranking.first);
        if (ranking.first->next) {
            for (Ranking::Player *player = ranking.first->next; player; player = player->next) {
                Ranking::Player *previous_to_out_player = nullptr;
                bool do_push_back = false;
                // Walk over players in out ranking - where countries are unique at any point.
                for (Ranking::Player *out_player = ranking_out.first; out_player; out_player = out_player->next) {
                    if (player->country == out_player->country) {  // When there is a chance of replacing.
                        if (player->rank > out_player->rank) {  // When 2 players have the same rank, take first one.
                            if (previous_to_out_player)
                                ranking_out.replace(player, previous_to_out_player);
                            else
                                ranking_out.replace_first(player);
                        }
                        do_push_back = true;
                        break;
                    }
                    previous_to_out_player = out_player;
                }
                if (!do_push_back)
                    ranking_out.push_back(player);  // When
            }
        }
    }
}

void remove_older_than(Ranking &ranking, size_t age) {
    if (ranking.first) {
        Ranking::Player *previous_player = nullptr;
        Ranking::Player *player = ranking.first;
        while (player) {
            if (2021 - player->birthdate > age) {
                Ranking::Player *next = player->next;
                if (previous_player)
                    ranking.remove(previous_player);
                else
                    ranking.remove_first();
                player = next;
            } else {
                previous_player = player;
                player = player->next;
            }
        }
    }
}

int main() {
    Ranking ranking;
    read_ranking_from_file(ranking);
    ranking.print();

    /// 1.
    Ranking players_with_minimal_rank;
    select_players_with_score_above(ranking, players_with_minimal_rank, 2700);
    players_with_minimal_rank.print();

    /// 2.
    Ranking best_player_per_country;
    select_best_players_per_country(ranking, best_player_per_country);
    best_player_per_country.print();

    /// 3.
    remove_older_than(ranking, 18);
    ranking.print();

    return 0;
}
