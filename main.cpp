#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <list>
#include <queue>
#include <algorithm>
#include <set>

using namespace std;

void Read(int& numberOfStates, int& initalState, vector<int>& finalStates, map<pair<int, int>, string>& NFA){
    ifstream fin("date.in");

    int numberOfFinalStates;
    int startState, endState;
    string aux;
    fin >> numberOfStates >> initalState >> numberOfFinalStates;
    for(int i=0; i<numberOfFinalStates; i++){
        fin >> startState;
        finalStates.push_back(startState);
    }
    while(fin >> startState >> endState){
        getline(fin, aux);
        NFA[make_pair(startState, endState)] = aux;
    }
}

void CalculateLambdaClosures(const map<pair<int, int>, string>& NFA, vector<list<int>>& lambdaClosures, int numberOfStates){
    for(int startState=0; startState<numberOfStates; ++startState){
        list<int> goToStates;
        goToStates.push_back(startState);
        for(auto nextState=goToStates.begin(); nextState!=goToStates.end(); ++nextState)
            for(int endState=0; endState<numberOfStates; ++endState){
                map<pair<int, int>, string>::const_iterator it = NFA.find(make_pair(*nextState, endState));
                if(it != NFA.end() && it->second.find("!") != string::npos){
                    goToStates.push_back(endState);
                }
            }
        goToStates.unique();
        goToStates.sort();
        lambdaClosures.push_back(goToStates);
    }
}

void CreateDFA(const map<pair<int, int>, string>& NFA, map<pair<list<int>, list<int>>, string>& DFA, const vector<list<int>>& lambdaClosures, int numberOfStates, int initialState){
    queue<list<int>> states;
    vector<list<int>> markStateAsDone;
    states.push(lambdaClosures[initialState]);
    while(!states.empty()){
        list<int> currentState = states.front();
        states.pop();
        for(char letter = 'a'; letter<='z'; ++letter){
            list<int> newSubStates;
            for(auto subState = currentState.begin(); subState != currentState.end(); ++subState){
                for(int nextSubState = 0; nextSubState < numberOfStates; ++nextSubState){
                    auto it = NFA.find(make_pair(*subState, nextSubState));
                    if(it != NFA.end() && it->second.find(letter) != string::npos)
                        newSubStates.push_back(nextSubState);
                }
            }
            if(!newSubStates.empty()){
                list<int> newStates;
                for(auto it = newSubStates.begin(); it!=newSubStates.end(); ++it)
                    for(auto ite = lambdaClosures[*it].begin(); ite != lambdaClosures[*it].end(); ++ite)
                        newStates.push_back(*ite);
                newStates.sort();
                newStates.unique();
                markStateAsDone.push_back(currentState);
                if(find(markStateAsDone.begin(), markStateAsDone.end(), newStates) == markStateAsDone.end()){
                    markStateAsDone.push_back(newStates);
                    states.push(newStates);
                }
                auto it = DFA.find(make_pair(currentState, newStates));
                if(it != DFA.end())
                    it->second += letter;
                else DFA[make_pair(currentState, newStates)] = letter;
            }
        }
    }
}

void PrintDFA(const map<pair<list<int>, list<int>>, string>& DFA){
    for(auto itMap = DFA.begin(); itMap != DFA.end(); ++itMap){
        cout<<"q";
        for(auto itList1 = itMap->first.first.begin(); itList1 != itMap->first.first.end(); ++itList1)
            cout<< *itList1;
        cout << " -> q";
        for(auto itList2 = itMap->first.second.begin(); itList2 != itMap->first.second.end(); ++itList2)
            cout << *itList2;
        cout << " cu " << itMap->second;
        cout << endl;
    }
}

int main()
{
    int initialState;
    int numberOfStates;
    vector<int> finalSates;
    map<pair<int, int>, string> NFA;
    map<pair<list<int>, list<int>>, string> DFA;
    vector<list<int>> lambdaClosures;

    Read(numberOfStates, initialState, finalSates, NFA);
    CalculateLambdaClosures(NFA, lambdaClosures, numberOfStates);
    CreateDFA(NFA, DFA, lambdaClosures, numberOfStates, initialState);
    PrintDFA(DFA);

    cout << "\nStare initiala: q";
    for(auto it = lambdaClosures[initialState].begin(); it!= lambdaClosures[initialState].end(); ++it)
        cout << *it;
    cout << "\nStari finale:\n";
    set<list<int>> output;
    for(auto it = DFA.begin(); it!=DFA.end(); ++it){
        for(auto it1 : finalSates){
            auto ite = find(it->first.first.begin(), it->first.first.end(), it1);
            if(ite != it->first.first.end())
                output.insert(it->first.first);
        }
    }
    for(auto it = output.begin(); it != output.end(); ++it){
        cout << "q";
        for(auto ite = it->begin(); ite != it->end(); ++ite)
            cout << *ite;
        cout << endl;
    }
    return 0;
}
