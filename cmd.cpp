#include<iostream>
#include<vector>
#include<string>



std::vector<std::string> split(std::string str)
{
    std::vector<std::string> result;
    std::string tmp = "";
    for (int i = 0; i < str.length(); i++)
    {
        if (str[i] == ' ')
        {
            result.push_back(tmp);
            tmp = "";
        }
        else
            tmp += str[i];
    }
    result.push_back(tmp);
    return result;
}

int main()
{
    std::string str = "hello";
    std::vector<std::string> result = split(str);
    for (int i = 0; i < result.size(); i++)
        std::cout << result[i] << std::endl;
    return 0;
}