#include <iostream>
#include <string>
#include <map>

using namespace std;

/**
 * In bang chuyen doi trang thai cua NFA
 */
void printTransitionStateTable(const map<string, map<char, string>> &delta)
{
    cout << "State Table:" << endl;
    // Duyet qua mang theo thu tu da sap xep
    for (const auto &state : delta)
    {
        cout << state.first << " | ";
        for (const auto &transition : state.second)
        {
            cout << transition.first << " -> " << transition.second << "| ";
        }
        cout << endl;
    }
}

/**
 * 1. Giai thuat so khop chuoi
 */
int findTPatternPosition(const string &input, const map<string, map<char, string>> &delta, const string &startState, const string &finalState)
{
    string currentState = startState;
    int aPosition = -1; // Khoi tao vi tri xuat hien ban dau cua ki tu a trong chuoi abcd. Ban dau chua co ki tu a (-1). Vi tri duoc cap nhat lai khi chuoi input co ky tu a
    int position = 0;   // Vi tri duyet cac ky tu cua chuoi input

    for (char ch : input)
    {
        // Su dung ham at() de truy cap key la 'trang thai dau vao'. Sau do lai tiep tuc truy cap vao key la 'ki tu chuyen' ==> ta duoc trang thai chuyen den
        currentState = delta.at(currentState).at(ch);

        // Kiem tra xem chuoi dau vao xuat hien ky ta a chua
        if (ch == 'a')
        {
            //Cap nhat lai vi tri neu trang thai hien tai khac startState
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
        cout << "position: " << position << ", aPosition: "<< aPosition << endl;
        cout << "----------------------------------------" << endl;
        // Neu trang thai hien ta la trang thai ket thuc va ky tu 'a' co xuat hien
        if (currentState == finalState && aPosition != -1)
        {
            return aPosition;
        }
    }
    return -1; // Khong tim thay chuoi abcd trong chuoi dau vao (input)
}

int main()
{
    map<string, map<char, string>> delta = {
        {"q0", {{'a', "q1"}, {'b', "q0"}, {'c', "q0"}, {'d', "q0"}}},
        {"q1", {{'a', "q1"}, {'b', "q2"}, {'c', "q0"}, {'d', "q0"}}},
        {"q2", {{'a', "q1"}, {'b', "q0"}, {'c', "q3"}, {'d', "q0"}}},
        {"q3", {{'a', "q1"}, {'b', "q0"}, {'c', "q0"}, {'d', "q4"}}},
        {"q4", {{'a', "q4"}, {'b', "q4"}, {'c', "q4"}, {'d', "q4"}}}};

    string startState = "q0";
    string acceptState = "q4";
    string input;

    cout << "----------------------------------------" << endl;
    // In bang trang thai
    printTransitionStateTable(delta);
    cout << "----------------------------------------" << endl;
    cout << "Nhap chuoi dau vao" << endl;
    cin >> input;
    cout << "----------------------------------------" << endl;

    int aPosition = findTPatternPosition(input, delta, startState, acceptState);

    if (aPosition >= 0)
    {
        cout << "--->Chuoi abcd xuat hien tai vi tri: " << aPosition << endl;
    }
    else
    {
        cout << "--->Chuoi abcd khong ton tai trong chuoi dau vao." << endl;
    }

    return 0;
}
