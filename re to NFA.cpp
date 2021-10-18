#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <stack>
#include <fstream>
using namespace std;

class NFA {
    string name;    // NFA name
    string re;      // ������ʽ    
    string re_;     // �����ӷ���������ʽ
    string pe;      // ������ʽ�ĺ�׺��ʽ
    int stateNum;   // ״̬��
    pair<int, int> se;      // �����յ�״̬���
    vector<vector<pair<int, char> > > graph;  // NFA״̬��ϵͼ
public:
    NFA(string name, string re);
    void insertContact();
    int priority(char c);
    void re2Pe();
    int newState();
    void pe2NFA();
    void printNFA(ofstream& fout);
};
NFA::NFA(string name, string re) {
    this->name = name; this->re = re;
    re_ = pe = "";
    stateNum = 0;
    graph.push_back(vector<pair<int, char> >());
}
// �������ӷ� .
void NFA::insertContact() {
    for (int i = 0; i < re.size() - 1; i++) {
        re_ += re[i];
        if (re[i] != '(' && re[i + 1] != ')' && re[i] != '|' && re[i + 1] != '|'
            && re[i + 1] != '*') re_ += '.';
    }
    re_ += re.back();
}
// ��������ȼ�
int NFA::priority(char c) {
    if (c == '*') return 3;
    else if (c == '.') return 2;
    else if (c == '|') return 1;
    else return 0;
}
// ������ʽת��Ϊ��׺��ʽ
void NFA::re2Pe() {
    stack<char> op;
    for (auto c : re_) {
        if (c == '(') op.push(c);
        else if (c == ')') {
            while (op.top() != '(') {
                pe += op.top();
                op.pop();
            }
            op.pop();
        }
        else if (c == '*' || c == '.' || c == '|') {
            while (op.size()) {
                if (priority(c) <= priority(op.top())) {
                    pe += op.top();
                    op.pop();
                }
                else break;
            }
            op.push(c);
        }
        else pe += c;
    }
    while (op.size()) {
        pe += op.top();
        op.pop();
    }
}
// ������״̬
int NFA::newState() {
    graph.push_back(vector<pair<int, char> >());    // ������״̬�ı߼�
    return ++stateNum;
}
// ��׺ת��ΪNFA
void NFA::pe2NFA() {
    stack<pair<int, int> > states;      // ״̬ջ
    int s, e;       // ״̬�������յ�״̬���
    for (auto c : pe) {
        if (c != '*' && c != '.' && c != '|') {
            s = newState();
            e = newState();
            states.push(make_pair(s, e));
            graph[s].push_back(make_pair(e, c));
            continue;
        }
        switch (c)
        {
        case '*': {
            pair<int, int> origin = states.top(); states.pop();
            s = newState();
            e = newState();
            states.push(make_pair(s, e));
            graph[s].push_back(make_pair(origin.first, ' '));
            graph[s].push_back(make_pair(e, ' '));
            graph[origin.second].push_back(make_pair(e, ' '));
            graph[origin.second].push_back(make_pair(origin.first, ' '));
            break;
        }
        case '.': {
            pair<int, int> right = states.top(); states.pop();
            pair<int, int> left = states.top(); states.pop();
            states.push(make_pair(left.first, right.second));
            graph[left.second].push_back(make_pair(right.first, ' '));
            break;
        }
        case '|': {
            pair<int, int> down = states.top(); states.pop();
            pair<int, int> up = states.top(); states.pop();
            s = newState();
            e = newState();
            states.push(make_pair(s, e));
            graph[s].push_back(make_pair(up.first, ' '));
            graph[s].push_back(make_pair(down.first, ' '));
            graph[up.second].push_back(make_pair(e, ' '));
            graph[down.second].push_back(make_pair(e, ' '));
            break;
        }
        default:
            break;
        }
    }
    se = make_pair(states.top().first, states.top().second);
}
// ���NFA
void NFA::printNFA(ofstream& fout) {
    fout << "name: " << name << "\n"
        << "re: " << re << "\n"
        << "pe: " << pe << "\n"
        << "stateNum: " << stateNum << "\n"
        << "start: " << se.first << "\n"
        << "end: " << se.second << endl;
    for (int i = 1; i <= stateNum; i++) {
        for (auto edge : graph[i]) {
            fout << i << "----" << edge.second << "---->" << edge.first << "\n";
        }
    }
    fout << endl;
}

int main(void)
{
    ifstream fin("re.txt");
    ofstream fout("NFA.txt");   // ָ������ļ�
    string name, re;
    while (fin >> name >> re) {
        NFA nfa(name, re);
        nfa.insertContact();
        nfa.re2Pe();
        nfa.pe2NFA();
        nfa.printNFA(fout);
    }
    return 0;
}
