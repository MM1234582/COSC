/*
I used this guide for the assignment as a reference:
https://gitgud.io/MadDawg/cosc3360_spring2024_guides/-/blob/master/assignment1guide.md?ref_type=heads

The video linked below also helped with a general understanding of the algorithm
https://www.youtube.com/watch?v=xgW8VhEOpFg
*/

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>   //Set Precision
#include <cmath>     //For power function
#include <algorithm> //For sort function

using namespace std;

struct task
{
    string name;
    int WCET;
    int period;
    int initWCET;
    task(char id, int worst, int per)
    {
        name = id;
        WCET = worst;
        initWCET = worst;
        period = per;
    }
};

struct infoFromMain
{
    std::vector<task> tasks;
    double *utilization;
    int hyperperiod;
    string schedule;
};

class TaskInterval
{
public:
    string name;
    int startTime;
    int endTime;
    bool stopped;
};

// Greatest Common Denominator Function
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

bool compareTasks(const task &a, const task &b)
{
    // Return by lesser period, if not return by name
    if (a.period == b.period)
    {
        return a.name < b.name;
    }
    return a.period < b.period;
}

string scheduler(infoFromMain input)
{
    // Sort tasks based on periods (Rate Monotonic order)
    std::sort(input.tasks.begin(), input.tasks.end(), compareTasks);

    int n = input.tasks.size();
    int hyperperiod = input.hyperperiod;

    vector<TaskInterval> intervalContainer;

    for (int i = 0; i < hyperperiod; i++) // for every tick, i
    {
        bool added = false;
        for (int j = 0; j < n; j++) // for every task, j
        {
            if (i != 0 && i % input.tasks.at(j).period == 0) // check we aren't on the first tick, and see if the task's next cycle has started
            {
                input.tasks.at(j).WCET = input.tasks.at(j).initWCET;

                // Mark intervals associated with the task as stopped
                for (int k = 0; k < intervalContainer.size(); k++)
                {
                    intervalContainer.at(k).stopped = true;
                }
            }
        }

        for (int j = 0; j < n; j++) // we again iterate through every task
        {
            if (input.tasks.at(j).WCET > 0) // if WCET is greater than 0
            {
                if (input.tasks.at(j).WCET == input.tasks.at(j).initWCET) // check if WCET value is the same as initial
                {
                    // create a new task interval and add to the end of the list
                    TaskInterval temp;
                    temp.startTime = i;
                    temp.endTime = i + 1;
                    temp.name = input.tasks.at(j).name;
                    temp.stopped = false;
                    intervalContainer.push_back(temp);
                    added = true;
                }
                else // WCET is not equal to initial WCET value
                {
                    //  search backwards for the task in the interval list
                    for (int q = intervalContainer.size() - 1; q >= 0; q--)
                    {
                        if (intervalContainer[q].name == input.tasks.at(j).name) // Found the prior process
                        {
                            if (intervalContainer[q].stopped) // If it was marked as stopped
                            {
                                // create a new interval for the interrupted task and add to the end of the list

                                // Maybe add something to increment here?

                                TaskInterval temp;
                                temp.startTime = i;
                                temp.endTime = i + 1;
                                temp.name = intervalContainer[q].name;
                                intervalContainer.push_back(temp);
                                added = true;

                                // set stopped to false for the widened interval
                                intervalContainer.at(q).stopped = false;

                                // break out of the loop after creating the interval
                                break;
                            }
                            else
                            {
                                // widen the interval
                                intervalContainer.at(q).endTime++;
                                added = true;
                                break;
                            }
                        }
                    }
                }

                // decrement the task's WCET value
                input.tasks.at(j).WCET--;

                // break out of the loop after decrementing WCET
                break;
            }
        }
        if (!added)
        {
            TaskInterval temp;
            temp.startTime = i;
            temp.endTime = i + 1;
            temp.name = "Idle";

            if (intervalContainer[intervalContainer.size() - 1].name == "Idle")
            {
                intervalContainer[intervalContainer.size() - 1].endTime++;
            }
            else
            {
                intervalContainer.push_back(temp);
            }
        }
    }

    string result;
    // Print the intervals
    for (int i = 0; i < intervalContainer.size(); i++)
    {
        int runtime = intervalContainer.at(i).endTime - intervalContainer.at(i).startTime;
        result = result + intervalContainer.at(i).name + "(" + to_string(runtime) + ")";
        if (i != intervalContainer.size() - 1)
        {
            result = result + ", ";
        }
    }
    return result;
}

void *utilizationCalculator(void *void_ptr)
{
    // Cast the void pointer to a usable type
    infoFromMain *info = (infoFromMain *)void_ptr;
    vector<int> periods;
    double temp = 0;

    // Iterate and get the hyperperiod
    for (int i = 0; i < info->tasks.size(); i++)
    {
        temp = temp + ((double)info->tasks[i].WCET / (double)info->tasks[i].period);
        periods.push_back(info->tasks[i].period);
    }
    int hyper = lcmVector(periods);
    info->hyperperiod = hyper;
    *(info->utilization) = temp;
    info->schedule = scheduler((*info));

    return nullptr;
}

int main()
{
    vector<infoFromMain> input;
    string line;
    while (getline(cin, line))
    {
        stringstream ss(line);
        char name;
        int worst;
        int per;

        infoFromMain tempInfo;

        // Gather input data
        while (ss >> name >> worst >> per)
        {
            task temp = task(name, worst, per);

            tempInfo.tasks.push_back(temp);
        }
        input.push_back(tempInfo);
    }

    // Create Threads
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

    // Join threads
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
        cout << "Rate Monotonic Algorithm execution for CPU" << i + 1 << ":" << endl;
        double n = input.at(i).tasks.size();
        double schedule = n * (pow(2, (1 / n)) - 1);
        if (*input.at(i).utilization > 1)
        {
            cout << "The task set is not schedulable" << endl;
        }
        else if (*input.at(i).utilization > schedule && schedule <= 1)
        {
            cout << "Task set schedulability is unknown" << endl;
        }
        else
        {
            cout << "Scheduling Diagram for CPU " << i + 1 << ": ";
            cout << input.at(i).schedule << endl;
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