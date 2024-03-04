#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>
#include <sstream>

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
    NFA() {}

    void addState(const string &state)
    {
        states.insert(state);
    }

    void addInput(const char &input)
    {
        alphabet.insert(input);
    }

    void setStartState(const string &start)
    {
        start_state = start;
    }

    void addAcceptState(const string &accept)
    {
        accept_states.insert(accept);
    }

    void addTransition(const string &from, const char &input, const vector<string> &to)
    {
        transition_function[{from, input}].insert(to.begin(), to.end());
    }
    void printInfo()
    {
        cout << "Danh sach trang thai: ";
        for (const auto &state : states)
            cout << state << " ";
        cout << endl;

        cout << "Bang chuyen trang thai:" << endl;
        for (const auto &transition : transition_function)
        {
            const auto &stateCharPair = transition.first;
            const auto &destination_states = transition.second;

            cout << "(" << stateCharPair.state << ", " << stateCharPair.input << ") -> ";
            for (const auto &dest_state : destination_states)
                cout << dest_state << " ";
            cout << endl;
        }

        cout << "Trang thai bat dau: " << start_state << endl;

        cout << "Trang thai ket thuc: ";
        for (const auto &accept_state : accept_states)
            cout << accept_state << " ";
        cout << endl;
    }

    void process_input(string input_string)
    {
        unordered_set<string> current_states = {start_state};
        cout << "Buoc 0: Start         " << start_state << endl;
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
                    cout << "Tu trang thai " << state << "---" << input_string[i] << "";
                    for (string dest_state : destination_states)
                    {
                        if (destination_states.size() == 1)
                        {
                            cout << "--->" << dest_state << " ";
                        }
                        else
                        {
                            cout << "--->" << dest_state << "\n";
                            cout << "                            "
                                 << "";
                        }
                    }
                    cout << endl;
                }
            }
            current_states = next_states;
        }
        cout << "-------------------------------------" << endl;
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

void printSampleNFA()
{
    cout << "NFA mau" << endl;
    cout << "q0 q1 q2           "
         << ": La cac trang thai cua NFA" << endl;
    cout << "a b                "
         << ": La cac ky tu dau vao trong bang chuyen trang thai" << endl;
    cout << "q0                 "
         << ": La trang thai bat dau (Start)" << endl;
    cout << "q2                 "
         << ": La (cac) trang thai chap nhan (Start)" << endl;
    cout << "q0 b q0            "
         << ": La quy tac chuyen trang thai trong NFA (Chuyen den 1 trang thai)" << endl;
    cout << "q0 b q0 q1         "
         << ": La quy tac chuyen trang thai trong NFA (Chuyen den nhieu trang thai)" << endl;
}
int main()
{
    NFA nfa;

    printSampleNFA();
    string filename;
    cout << "-------------------------------------" << endl;
    cout << "Nhap ten file: ";
    cin >> filename;

    ifstream file(filename);
    if (!file)
    {
        cerr << "Khong the mo file!" << endl;
        return 1;
    }
    cout << "-------------------------------------" << endl;

    string line;
    int line_num = 1;
    while (getline(file, line))
    {
        istringstream iss(line);
        string token;
        vector<string> tokens;
        while (iss >> token)
        {
            tokens.push_back(token);
        }

        if (line_num == 1)
        {
            for (const auto &state : tokens)
            {
                nfa.addState(state);
            }
        }
        else if (line_num == 2)
        {
            for (const auto &input : tokens)
            {
                nfa.addInput(input[0]);
            }
        }
        else if (line_num == 3)
        {
            nfa.setStartState(tokens[0]);
        }
        else if (line_num == 4)
        {
            for (const auto &accept : tokens)
            {
                nfa.addAcceptState(accept);
            }
        }
        else
        {
            if (tokens.size() < 3)
            {
                cerr << "Loi: Dong " << line_num << " khong hop le." << endl;
                return 1;
            }
            vector<string> to_states(tokens.begin() + 2, tokens.end());
            nfa.addTransition(tokens[0], tokens[1][0], to_states);
        }

        line_num++;
    }
    nfa.printInfo();
    string input_string;
    cout << "-------------------------------------" << endl;
    cout << "Nhap chuoi dau vao: ";
    cin >> input_string;
    cout << "-------------------------------------" << endl;
    nfa.process_input(input_string);

    return 0;
}
