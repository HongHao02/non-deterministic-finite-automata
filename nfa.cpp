#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>

using namespace std;

// Define a custom struct for the key of the transition function map
struct StateCharPair
{
    string state;
    char input;

    // Overload the equality operator
    bool operator==(const StateCharPair &other) const
    {
        return state == other.state && input == other.input;
    }
};

// Define a custom hash function for the custom struct
namespace std
{
    template <>
    struct hash<StateCharPair>
    {
        size_t operator()(const StateCharPair &pair) const
        {
            // Hash combining algorithm
            size_t hashValue = 0;
            hash<string> hashString;
            hashValue ^= hashString(pair.state) + 0x9e3779b9 + (hash<char>()(pair.input) << 6) + (hash<char>()(pair.input) >> 2);
            return hashValue;
        }
    };
}

class NFA
{
private:
    unordered_set<string> states;
    unordered_set<char> alphabet;
    unordered_map<StateCharPair, unordered_set<string>> transition_function;
    string start_state;
    unordered_set<string> accept_states;

public:
    NFA(unordered_set<string> _states, unordered_set<char> _alphabet,
        unordered_map<StateCharPair, unordered_set<string>> _transition_function,
        string _start_state, unordered_set<string> _accept_states) : states(_states), alphabet(_alphabet), transition_function(_transition_function),
                                                                     start_state(_start_state), accept_states(_accept_states) {}

    // void process_input(string input_string)
    // {
    //     unordered_set<string> current_states = {start_state};
    //     std::cout << "Buoc 0: " << start_state << std::endl;
    //     for (size_t i = 0; i < input_string.length(); ++i)
    //     {
    //         unordered_set<string> next_states;
    //         for (string state : current_states)
    //         {
    //             StateCharPair current_pair = {state, input_string[i]};
    //             if (transition_function.find(current_pair) != transition_function.end())
    //             {
    //                 next_states.insert(transition_function[current_pair].begin(),
    //                                    transition_function[current_pair].end());
    //             }
    //         }
    //         current_states = next_states;
    //         std::cout << "Buoc " << i + 1 << ": ";
    //         for (string state : current_states)
    //         {
    //             std::cout << state << " ";
    //         }
    //         std::cout << std::endl;
    //     }
    //     std::cout << "Ket qua: ";
    //     bool accepted = false;
    //     for (string state : current_states)
    //     {
    //         if (accept_states.find(state) != accept_states.end())
    //         {
    //             std::cout << "Chuoi duoc chap nhan boi NFA." << std::endl;
    //             accepted = true;
    //             break;
    //         }
    //     }
    //     if (!accepted)
    //     {
    //         std::cout << "Chuoi khong duoc chap nhan boi NFA." << std::endl;
    //     }
    // }
    void process_input(string input_string)
    {
        unordered_set<string> current_states = {start_state};
        cout << "Buoc 0: " << start_state << endl;
        for (size_t i = 0; i < input_string.length(); ++i)
        {
            unordered_set<string> next_states;
            for (string state : current_states)
            {
                StateCharPair current_pair = {state, input_string[i]};
                if (transition_function.find(current_pair) != transition_function.end())
                {
                    auto destination_states = transition_function[current_pair];
                    next_states.insert(destination_states.begin(), destination_states.end());

                    // Hiển thị thông tin về việc chuyển trạng thái
                    cout << "Buoc " << i + 1 << ": ";
                    cout << "Tu trang thai " << state << " bang ki tu " << input_string[i] << " den ";
                    for (string dest_state : destination_states)
                        cout << dest_state << " ";
                    cout << endl;
                }
            }
            current_states = next_states;
        }
        cout << "Ket qua: ";
        bool accepted = false;
        for (string state : current_states)
        {
            if (accept_states.find(state) != accept_states.end())
            {
                cout << "Chuoi duoc chap nhan boi NFA." << endl;
                accepted = true;
                break;
            }
        }
        if (!accepted)
        {
            cout << "Chuoi khong duoc chap nhan boi NFA." << endl;
        }
    }
};

int main()
{
    unordered_set<string> states = {"q0", "q1", "q2"};
    unordered_set<char> alphabet = {'a', 'b'};
    unordered_map<StateCharPair, unordered_set<string>> transition_function;

    transition_function[{"q0", 'b'}] = {"q0"};
    transition_function[{"q0", 'a'}] = {"q0", "q1"};
    transition_function[{"q1", 'a'}] = {"q2"};

    string start_state = "q0";
    unordered_set<string> accept_states = {"q2"};

    NFA nfa(states, alphabet, transition_function, start_state, accept_states);

    string input_string;
    std::cout << "Nhap chuoi dau vao: ";
    cin >> input_string;

    nfa.process_input(input_string);

    return 0;
}
