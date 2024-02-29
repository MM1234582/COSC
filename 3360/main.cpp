#include <iostream>
#include <vector>

int main()
{
    std::vector<int> array;
    int tempValue;

    while (std::cin >> tempValue)
    {
        array.push_back(tempValue);
    }

    for (int i = 0; i < array.size(); i++)
    {
        std::cout << array.at(i) << " ";
    }
    std::cout << std::endl;
    return 0;
}