#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <semaphore.h>

using namespace std;
mutex mtx;
unordered_set<string> current_states_copy;

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
void print_thread_start(int id, string state, char input_char)
{
    cout << "Thread " << id << " started processing state " << state << " for input: " << input_char << endl;
}
void print_unordered_set(unordered_set<string> set)
{
    string result;
    for (const std::string &str : set)
    {
        result += str + ",";
    }
    if (!result.empty())
    {
        result.pop_back();
    }
    cout << "{" << result << "}" << "";
}
void print_input_char(char input_char)
{
    cout << "--" << input_char << "-->" << "";
}

class Result
{
public:
    Result(int id, unordered_set<string> dest_states)
        : id(id), dest_states(dest_states) {}

    int getId()
    {
        return id;
    }
    unordered_set<string> getDestStates()
    {
        return dest_states;
    }

private:
    int id;
    unordered_set<string> dest_states;
};
class NFA
{
private:
    unordered_set<string> states;
    unordered_set<char> alphabet;
    unordered_map<StateCharPair, unordered_set<string>> transition_function;
    string start_state;
    unordered_set<string> accept_states;
    atomic<int> thread_counter; // Sử dụng atomic để đảm bảo thread-safe khi tăng thread_counter
    sem_t semaphore;

public:
    NFA() : thread_counter(0)
    {
        sem_init(&semaphore, 0, 1);
    } // Khởi tạo thread_counter

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

    void addTransition(const string &from, const char &input, const unordered_set<string> &to)
    {
        // Sử dụng emplace để thêm một cặp key-value vào unordered_map
        transition_function.emplace(StateCharPair{from, input}, unordered_set(to.begin(), to.end()));
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

    // void process_input(string input_string)
    // {
    //     unordered_set<string> current_states = {start_state};
    //     cout << "Buoc 0: Start         " << start_state << endl;

    //     for (char input_char : input_string)
    //     {
    //         vector<thread> current_threads;
    //         unordered_set<string> current_states_copy;

    //         for (const auto &state : current_states)
    //         {
    //             StateCharPair current_pair = {state, input_char};
    //             if (transition_function.find(current_pair) != transition_function.end())
    //             {
    //                 auto destination_states = transition_function[current_pair];

    //                 for (const auto &dest_state : destination_states)
    //                 {

    //                     current_threads.emplace_back([&]()
    //                                                  {

    //                     int current_thread_counter = ++thread_counter; // Tăng thread_counter và lấy giá trị hiện tại
    //                     cout << "Thread " << current_thread_counter << " started processing state " << dest_state << " for input: " << input_char << endl;
    //                     unordered_set<string> new_states = process_state(dest_state, input_char);

    //                     for (const auto &new_state : new_states)
    //                     {
    //                         cout<< new_state << " ";
    //                         current_states_copy.insert(new_state);
    //                     }
    //                     cout<< " " << endl;

    //                     cout << "Thread " << current_thread_counter << " finished processing state " << dest_state << endl; });

    //                 }
    //             }
    //         }

    //         // Đợi cho tất cả các luồng kết thúc
    //         for (auto &thread : current_threads)
    //         {
    //             if (thread.joinable())
    //             {
    //                 thread.join();
    //             }
    //         }

    //         current_states = current_states_copy;
    //     }

    //     // Kiểm tra xem các trạng thái hiện tại có phải là trạng thái kết thúc không
    //     bool accepted = false;
    //     cout << "-------------------------------------" << endl;
    //     for (const auto &state : current_states)
    //     {
    //         cout << "---> " << state  << endl;
    //         if (accept_states.find(state) != accept_states.end())
    //         {
    //             cout << "Chuoi duoc chap nhan boi NFA." << endl;
    //             accepted = true;
    //             break;
    //         }
    //     }
    //     if (!accepted)
    //     {
    //         cout << "Chuoi khong duoc chap nhan boi NFA." << endl;
    //     }
    // }
    void process_input(string input_string)
    {
        unordered_set<string> current_states = {start_state};
        cout << "Buoc 0: Start         " << start_state << endl;
        print_unordered_set(current_states);

        for (char input_char : input_string)
        {
            // cout << "[------------------" << input_char << "------------------]" << endl;
            vector<thread> current_threads;
            current_states_copy.clear();
            int count = 0;
            print_input_char(input_char);
            for (const auto &state : current_states)
            {
                count++;
                int current_thread_counter = ++thread_counter; // Tăng thread_counter và lấy giá trị hiện tại
                // print_thread_start(current_thread_counter, state, input_char);
                // print_thread_start(count, state, input_char);
                // cout << "Thread start processing state " << state << endl;
                current_threads.emplace_back([&]()
                                             {
                                                 // Result new_states = process_state(state, input_char, current_thread_counter);
                                                 Result new_states = process_state(state, input_char, count);
                                                 // Chờ semaphore
                                                 sem_wait(&semaphore);
                                                 // cout << "Thread finshed processing state " << state << endl;
                                                 for (const auto &new_state : new_states.getDestStates())
                                                 {
                                                     // cout << "Dest state from " << state << "->" << new_state << endl;
                                                     current_states_copy.insert(new_state);
                                                 }
                                                 // cout << "Thread " << new_states.getId() << " finished processing state " << state << endl;
                                                 sem_post(&semaphore);
                                                 // std::this_thread::sleep_for(std::chrono::milliseconds(234));
                                             });
            }

            // Đợi cho tất cả các luồng kết thúc
            for (auto &thread : current_threads)
            {
                if (thread.joinable())
                {
                    thread.join();
                }
            }

            // // Thu thập các trạng thái mới vào vector
            // unordered_set<string> new_states_vector;
            // for (const auto &state : current_states_copy)
            // {
            //     new_states_vector.insert(state);
            // }

            // In ra các trạng thái mới
            // cout << "------New States List------" << endl;
            // for (const auto &state : current_states_copy)
            // {
            //     cout << state << endl;
            // }
            current_states = current_states_copy;
            print_unordered_set(current_states);
        }

        // Kiểm tra xem các trạng thái hiện tại có phải là trạng thái kết thúc không
        bool accepted = false;
        cout << "" << endl;
        cout << "-------------------------------------" << endl;
        for (const auto &state : current_states)
        {
            // cout << "---> " << state << endl;
            if (accept_states.find(state) != accept_states.end())
            {

                cout << state << " thuoc F" << endl;
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

private:
    // Ham thuc thi cua thread
    Result process_state(const string &state, const char char_input, int current_thread_counter)
    {
        // Thời gian bắt đầu của luồng
        auto start_time = chrono::steady_clock::now();

        // Xử lý trạng thái cho ký tự đầu vào
        // cout << "Processing state " << state << " for input: " << char_input << endl;

        unordered_set<string> destination_states;

        // Kiểm tra xem có thể chuyển từ trạng thái hiện tại với ký tự đầu vào tương ứng hay không
        StateCharPair current_pair = {state, char_input};
        if (transition_function.find(current_pair) != transition_function.end())
        {
            destination_states = transition_function[current_pair];
        }

        // Thời gian kết thúc của luồng
        auto end_time = chrono::steady_clock::now();
        auto elapsed_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
        // cout << "Thread finished processing state " << state << " in " << elapsed_time << " milliseconds." << endl;
        // Trả về các trạng thái đích tương ứng
        return Result(current_thread_counter, destination_states);
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
    // cout << "Nhap ten file: " << endl;
    // cin >> filename;
    filename = "D:\\SCHOOL_YEAR\\YEAR_4\\HK2\\KiemChungMoHinh\\non-deterministic-finite-automata\\NFA_CONCURENCY\\nfa.txt";

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
            unordered_set<string> to_states(tokens.begin() + 2, tokens.end());
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
