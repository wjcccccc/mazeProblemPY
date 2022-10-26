//
//  main.cpp
//  stackPostfix
//
//  Created by 陳椲博 on 2022/10/26.
//

#include <iostream>
#include <stack>
using namespace std;

int precedence(char operators){
    if (operators == '+' || operators == '-')
        return 1;
    if (operators == '*' || operators == '/')
        return 2;
    if (operators == '^')
        return 3;
    return 0;
}

string infixToPostfix(string infix){
    int i = 0;
    string postfix = "";
    stack<int>s;
    
    while (infix[i]!='\0') {
        if ((infix[i] >= 'a' && infix[i] <= 'z') || (infix[i] >= 'A' && infix[i] <= 'Z')) {
            postfix = infix[i] + postfix;
            i++;
        }
        else if (infix[i] == '('){
            s.push(infix[i]);
            i++;
        }
        else if (infix[i] == ')'){
            while (s.top()!= '(') {
                postfix += s.top();
                s.pop();
            }
            s.pop();
            i++;
        }
        else{
            while (!s.empty() && precedence(infix[i]) <= precedence(s.top()) ){
                postfix = s.top();
                s.pop();
            }
            s.push(infix[i]);
            i++;
            
        }
    }
    while (!s.empty()) {
        postfix += s.top();
        s.pop();
    }
    
    cout << "postfix is : "<< postfix << endl;
    return postfix;
}
int main(){
    string infix = "((a+(b*c))-d)";
    string postfix;
    postfix = infixToPostfix(infix);
    
    return 0;
}
