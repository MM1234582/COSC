#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <climits>
#include <fstream>

using namespace std;

struct task
{
    char name;
    int WCET;
    int period;
    task(char id, int worst, int per)
    {
        name = id;
        WCET = worst;
        period = per;
    }
};

struct infoFromMain
{
    std::vector<task> tasks;
    double *utilization;
    int hyperperiod;
};

int gcd(int a, int b)
{
    while (b != 0)
    {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Function to calculate LCM of two numbers
int lcm(int a, int b)
{
    return (a * b) / gcd(a, b);
}

// Function to calculate LCM of a vector of numbers
int lcmVector(const std::vector<int> &numbers)
{

    int result = numbers[0];
    for (size_t i = 1; i < numbers.size(); ++i)
    {
        result = lcm(result, numbers[i]);
    }

    return result;
}

void *utilizationCalculator(void *void_ptr)
{
    // Write the code for the thread function here
    infoFromMain *info = (infoFromMain *)void_ptr;
    vector<int> periods;
    double temp = 0;
    for (int i = 0; i < info->tasks.size(); i++)
    {
        temp = temp + ((double)info->tasks[i].WCET /
                       (double)info->tasks[i].period);
        periods.push_back(info->tasks[i].period);
    }
    int hyper = lcmVector(periods);
    info->hyperperiod = hyper;
    *(info->utilization) = temp;

    return nullptr;
}
/*
void scheduler(infoFromMain input)
{
    int currentTime = 0;
    vector<pair<int, char>> schedule;
    vector<int> remainingTime(input.tasks.size(), 0);

    while (currentTime < input.hyperperiod)
    {
        // Find the task with the earliest deadline
        int earliestIndex = -1;
        int earliestDeadline = INT_MAX; // Include <climits>

        for (int i = 0; i < input.tasks.size(); i++)
        {
            if (remainingTime[i] > 0 && remainingTime[i] < earliestDeadline)
            {
                earliestDeadline = remainingTime[i];
                earliestIndex = i;
            }
        }

        // If no task is available, mark as idle
        if (earliestIndex == -1)
        {
            schedule.push_back({1, 'I'});
            currentTime++;
        }
        else
        {
            // Execute the task
            schedule.push_back({input.tasks[earliestIndex].WCET, input.tasks[earliestIndex].name});
            remainingTime[earliestIndex]--;
            currentTime++;
        }

        // Update remaining time for each task
        for (int i = 0; i < input.tasks.size(); i++)
        {
            remainingTime[i] = (remainingTime[i] + 1) % input.tasks[i].period;
        }
    }

    // Print the scheduling diagram
    for (auto entry : schedule)
    {
        if (entry.second == 'I')
        {
            cout << "Idle(" << entry.first << "), ";
        }
        else
        {
            cout << entry.second << "(" << entry.first << "), ";
        }
    }
}
*/
int main()
{
    vector<infoFromMain> input;
    ifstream fin("input.txt");
    string line;
    while (getline(fin, line))
    {
        stringstream ss(line);
        char name;
        int worst;
        int per;

        infoFromMain tempInfo;
        while (ss >> name >> worst >> per)
        {
            // cout << "Name: " << name << " WCET: " << worst << "Period: " << per << endl;
            task temp = task(name, worst, per);

            tempInfo.tasks.push_back(temp);
        }
        input.push_back(tempInfo);
    }
    /*

    for (int i = 0; i < input.size(); i++)
    {
        for (int j = 0; j < input.at(i).tasks.size(); j++)
            cout << input.at(i).tasks.at(j).name << " (WCET: " <<
input.at(i).tasks.at(j).WCET << ", Period: " <<
input.at(i).tasks.at(j).period << ")";
    }
    */
    int nThreads = input.size();
    pthread_t *tid = new pthread_t[nThreads];
    double *result = new double[nThreads];
    for (int i = 0; i < nThreads; i++)
    {
        input.at(i).utilization = &result[i];
        if (pthread_create(&tid[i], NULL, utilizationCalculator, &input[i]))
        {
            std::cerr << "Error creating thread" << std::endl;
            return 1;
        }
    }

    for (int i = 0; i < nThreads; i++)
    {
        pthread_join(tid[i], NULL);
    }

    for (int i = 0; i < nThreads; i++)
    {

        cout << "CPU " << i + 1 << std::endl;
        cout << "Task scheduling information: ";
        for (int j = 0; j < input.at(i).tasks.size(); j++)
        {
            cout << input.at(i).tasks.at(j).name << " (WCET: " << input.at(i).tasks.at(j).WCET << ", Period: " << input.at(i).tasks.at(j).period << ")";
            if (j != input.at(i).tasks.size() - 1)
            {
                cout << ", ";
            }
        }
        cout << std::endl;
        cout << "Task set utilization: " << std::fixed << std::setprecision(2) << *input.at(i).utilization << std::endl;
        cout << "Hyperperiod: " << input.at(i).hyperperiod << endl;
        cout << "Rate Monotonic Algorithm execution for CPU" << i + 1
             << ":" << endl;
        double n = input.at(i).tasks.size();
        double schedule = n * (pow(2, (1 / n)) - 1);
        if (*input.at(i).utilization > schedule && schedule <= 1)
        {
            cout << "Task set schedulability is unknown" << endl;
        }
        else
        {
            cout << "Scheduling Diagram for CPU " << i + 1 << ": ";
            // scheduler(input.at(i));
        }

        cout << endl
             << endl;
    }

    if (tid != nullptr)
        delete[] tid;
    if (result != nullptr)
        delete[] result;

    return 0;
}