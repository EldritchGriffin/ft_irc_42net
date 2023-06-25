#include<iostream>
#include<string>
using namespace std;
int main()
{
    string s;
    getline(cin, s);
    int sum;
    for(int i=0;i<s.size();i++)
    {
        if(s[i] == '*' || s[i] == '-' || s[i] == '+' || s[i] == '/')
            sum+=(int)s[i];
    }
    cout << sum << endl;
}

// + 43
// * 42
// - 45
// / 47