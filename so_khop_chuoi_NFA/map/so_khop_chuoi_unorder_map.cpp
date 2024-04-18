#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;
/**
 * In bang chuyen doi trang thai cua NFA
*/
void printStateTable(const unordered_map<string, unordered_map<char, string>> &delta)
{
    cout << "State Table:" << endl;
    vector<string> states = {"q0", "q1", "q2", "q3", "q4"};
    for (const auto &state : states)
    {
        cout << state << ": ";
        for (const auto &transition : delta.at(state))
        {
            cout << transition.first << " -> " << transition.second << ", ";
        }
        cout << endl;
    }
}
/**
 * 1. Giai thuat so khop chuoi
*/
int findSubstringPosition(const string &input, const unordered_map<string, unordered_map<char, string>> &delta, const string &startState, const string &finalState)
{
    string currentState = startState;
    int aPosition = -1; // Khoi tao vi tri xuat hien ban dau cua ki tu a trong chuoi abcd. Ban dau chua co ki tu a (-1). Vi tri duoc cap nhat lai khi chuoi input co ky tu a
    int position = 0; // Vi tri duyet cac ky tu cua chuoi input

    for (char ch : input)
    {
        // Sử dụng phép toán at() de truy cap key la trang thai dau vao. Sau do lai tiep tuc truy cap vao key la ki tu chuyen ==> ta duoc trang thai chuyen qua
        currentState = delta.at(currentState).at(ch);

        // Kiem tra xem chuoi dau vao xuat hien ky ta a chua
        if (ch == 'a')
        {
            // 
            if (currentState != startState)
            {
                aPosition = position;
            }
        }
        // Neu trang thai hien tai la trang thai bat dau thi xem nhu chuoi abcd chua tung xuat hien (-1)
        if (currentState == startState)
        {
            aPosition = -1;
        }
        // Dem so luong ky tu da duyet qua
        position++;

        cout << "current state: " << currentState << endl;
        cout << "position: " <<  position << "_""aPosition: " << aPosition << endl;
        // Neu trang thai hien ta la trang thai ket thuc va ky tu a co xuat hien
        if (currentState == finalState && aPosition != -1)
        {
            return aPosition;
        }
    }

    return -1; // Khong tim thay chuoi abcd trong chuoi dau vao (input)
}


int main()
{
    unordered_map<string, unordered_map<char, string>> delta = {
        {"q0", {{'a', "q1"}, {'b', "q0"}, {'c', "q0"}, {'d', "q0"}}},
        {"q1", {{'a', "q1"}, {'b', "q2"}, {'c', "q0"}, {'d', "q0"}}},
        {"q2", {{'a', "q1"}, {'b', "q0"}, {'c', "q3"}, {'d', "q0"}}},
        {"q3", {{'a', "q1"}, {'b', "q0"}, {'c', "q0"}, {'d', "q4"}}},
        {"q4", {{'a', "q4"}, {'b', "q4"}, {'c', "q4"}, {'d', "q4"}}}};

    string startState = "q0";
    string finalState = "q4";
    string input;

    // In bang trang thai
    printStateTable(delta);
    cout << "Nhap chuoi dau vao" << endl;
    cin >> input;

    int aPosition = findSubstringPosition(input, delta, startState, finalState);

    if (aPosition >= 0)
    {
        cout << "Chuoi abcd xuat hien tai vi tri: " << aPosition << endl;
    }
    else
    {
        cout << "Chuoi abcd khong ton tai trong chuoi dau vao." << endl;
    }

    return 0;
}