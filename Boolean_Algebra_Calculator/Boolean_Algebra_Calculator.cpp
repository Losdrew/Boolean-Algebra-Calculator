#include <iostream>
#include <stack>
#include <string>
#include <sstream>

using namespace std;

struct Leksema
{
    // letters for variables, '0' for numbers,
    // '+' for conjunction, '*' for disjunction, '`' for inversion, '>' for implication, '~' for equivalence  
    char type;
    int value; // always 0 for operators
};

bool CheckForVariables(Leksema* vars, string& formula);
void PrintVariables(Leksema* vars);
void GetNumbers(Leksema* vars);
string ReplaceVariables(string& formula, Leksema* vars);
void CheckFormula(stack<Leksema>& Stack_n, stack<Leksema>& Stack_o, Leksema& element, string& formula);
void Math(stack<Leksema>& Stack_n, stack<Leksema>& Stack_o, Leksema& element);
int GetPriority(char& ch);

int main()
{
    stack<Leksema> Stack_n; // for numbers
    stack<Leksema> Stack_o; // for operations
    Leksema element; // contains element's properties
    Leksema* vars = new Leksema[20]; // array for variables

    cout << "Available operations: + for disjunction, * for conjunction,"
        " - for negation, > for implication, ~for equivalence.\n\n";

    cout << "Enter the formula:\n";

    string formula;
    getline(cin, formula);

    int isVar = CheckForVariables(vars, formula);

    if (isVar == 1)
    {
        cout << "\nEnter values of variables separated by space in this order:\n";
        PrintVariables(vars);

        GetNumbers(vars);

        formula = ReplaceVariables(formula, vars);

    }

    delete[] vars;

    CheckFormula(Stack_n, Stack_o, element, formula);

    cout << "\nResult: " << Stack_n.top().value << "\n";

    return 0;
}

//
// Check if there are variables in the formula
//
bool CheckForVariables(Leksema* vars, string& formula)
{
    stringstream iss(formula);
    char ch;

    string notvariables = "01+*`>~() ";

    while (iss >> ch)
    {
        if (notvariables.find(ch) == string::npos)
        {
            vars->type = ch;
            vars++;
        }
    }

    vars--;

    // if not found
    if (vars->type < 0)
        return false;

    return true;
}

//
// Print variables on the screen
//
void PrintVariables(Leksema* vars)
{
    // check every element to find duplicates
    for (int i = 0; vars[i].type > 0; i++)
    {
        if (vars[i].type != 1)
        {
            for (int n = 0; vars[n].type > 0; n++)
                if (vars[i].type == vars[n].type && vars[n].type != 1 && i != n)
                    vars[n].type = 1; // marking as duplicate
        }
    }

    // print variables
    for (int i = 0; vars[i].type > 0; i++)
        if (vars[i].type != 1)
            cout << vars[i].type << " ";

    cout << endl;
}

//
// Getting values for variables
//
void GetNumbers(Leksema* vars)
{
    string numbers;
    getline(cin, numbers);

    for (unsigned int i = 0; i < numbers.length(); i++)
        if (numbers[i] == ' ')
            numbers.erase(i, 1);

    int i = 0;

    for (int n = 0; vars[n].type > 0; n++)
    {
        if (vars[n].type != 1)
        {
            vars[n].value = numbers[i] - '0';
            i++;
        }
    }
}

//
// Replace variables in formula with their values
//
string ReplaceVariables(string& formula, Leksema* vars)
{
    for (int n = 0; vars[n].type > 0; n++)
    {
        if (vars[n].type != 1)
        {
            for (unsigned int i = 0; i < formula.length(); i++)
                if (formula[i] == vars[n].type)
                    formula.replace(i, 1, to_string(vars[n].value));
        }
    }

    return formula;
}

//
// Check every symbol and add to stack
//
void CheckFormula(stack<Leksema>& Stack_n, stack<Leksema>& Stack_o, Leksema& element, string& formula)
{
    stringstream iss(formula);
    char ch;
    int value;

    while (iss.peek() != EOF)
    {
        ch = iss.peek();

        switch (ch)
        {
        case ' ':
            iss.ignore();
            continue;

        case '0':
        case '1':
            value = ch - '0';
            element.type = '0';
            element.value = value;
            Stack_n.push(element);
            iss.ignore();
            continue;

        case '+':
        case '*':
        case '`':
        case '>':
        case '~':
            if (Stack_o.size() == 0)
            {
                element.type = ch;
                element.value = 0;
                Stack_o.push(element);
                iss.ignore();
                continue;
            }

            if (Stack_o.size() != 0 && GetPriority(ch) > GetPriority(Stack_o.top().type))
            {
                element.type = ch;
                element.value = 0;
                Stack_o.push(element);
                iss.ignore();
                continue;
            }
            if (Stack_o.size() != 0 && GetPriority(ch) <= GetPriority(Stack_o.top().type))
            {
                Math(Stack_n, Stack_o, element);
                continue;
            }

        case '(':
            element.type = ch;
            element.value = 0;
            Stack_o.push(element);
            iss.ignore();
            continue;

        case ')':
            while (Stack_o.top().type != '(')
            {
                Math(Stack_n, Stack_o, element);
                continue;
            }
            Stack_o.pop();
            iss.ignore();
            continue;
        }
    }

    while (Stack_o.size() != 0)
        Math(Stack_n, Stack_o, element);
}

//
// Do math
//
void Math(stack<Leksema>& Stack_n, stack<Leksema>& Stack_o, Leksema& element)
{
    int a, b, c, d;

    a = Stack_n.top().value;
    Stack_n.pop();

    switch (Stack_o.top().type)
    {
    case '+':
        b = Stack_n.top().value;
        Stack_n.pop();
        c = max(a, b);
        element.type = '0';
        element.value = c;
        Stack_n.push(element);
        Stack_o.pop();
        break;

    case '*':
        b = Stack_n.top().value;
        Stack_n.pop();
        c = min(a, b);
        element.type = '0';
        element.value = c;
        Stack_n.push(element);
        Stack_o.pop();
        break;

    case '`':
        c = (a + 1) % 2;
        element.type = '0';
        element.value = c;
        Stack_n.push(element);
        Stack_o.pop();
        break;

    case '>':
        b = Stack_n.top().value;
        Stack_n.pop();
        d = (b + 1) % 2;
        c = max(a, d);
        element.type = '0';
        element.value = c;
        Stack_n.push(element);
        Stack_o.pop();
        break;

    case '~':
        b = Stack_n.top().value;
        Stack_n.pop();
        (a == b) ? c = 1 : c = 0;
        element.type = '0';
        element.value = c;
        Stack_n.push(element);
        Stack_o.pop();
        break;
    }
}

//
// Operations priority
//
int GetPriority(char& ch)
{
    switch (ch)
    {
    case '~':
        return 1;

    case '>':
        return 2;

    case '+':
        return 3;

    case '*':
        return 4;

    case '`':
        return 5;

    default:
        return 0;
    }
}