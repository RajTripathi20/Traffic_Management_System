/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, PHP, Ruby, 
C#, VB, Perl, Swift, Prolog, Javascript, Pascal, HTML, CSS, JS
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/

#include "output.h"
#define inp "input.txt"
#define out "solution.txt"
/*
We take input from the user, define a metric for scoring of our traffic schedule and then evaluate and print the output.
Default lights are red at every intersection.

We evaluate our solution by running a simulation of duration d, and scoring the schedule based on the vehicles that reach
their destination before t = d. We set some baseScore(which can be taken as an input) for every vehicle that completes 
its journey and then some bonusScore that is related to how fast the vehicles completes the journey. 
Eg: BaseScore = 500, and BonusScore = (d-t)*10 (if d>=t)
The totalScore of the schedule is then the sum of scores of all the vehicles
*/



/*
Input Format:
We take input from a file in the following format-

simulationTime(int) numberOfIntersections(int) numberOfStreets(int) numberOfVehicles(int) baseScore(int)
<for every Street>
startPoint(int) endPoint(int) nameWithoutSpace(String) travelTime(int) 
<for every Vehicle>
numberOfStreetsInPath(int) nameStreet1(String) nameStreet2(String) ...
*/

/*
Output Format:
We store the calculated schedule in a file in the following format-

numberOfIntersectionsToBeScheduled(int)
<for every Intersection>
intersectionIndex(int)
numberOfLightsToBeScheduled(int)
<for every light to be scheduled in the intersection>
StreetName(String) GreenDuration(int)


*/


int main()
{
    chrono::milliseconds startTime = duration_cast<chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    ProblemModel model;
    model.buildModelFromFile(inp);
    int removeStreets = model.cleanModel();
    cout << "Streets removed: " << removeStreets << endl;
    
    
    Solution* solution = new Solution(model.intersections.size());
    initSolution(model, solution);
    
    cout<<"Upper Bound is: "<<model.maxPossibleScore()<<endl;
    
    int score;
    score = optimizeGreenLightOrder(model, solution);
    cout << "Score after just reordering: " << score << endl;
    
    
    int new_score = score;
    
    do {
        score = new_score;
        new_score = weightedSchedule(model, solution);
        //new_score = optimizeGreenLightOrder(model, solution);

    }
    while(new_score>score);
    cout << "Final Score: " << score << endl;
    
    generateOutput(solution, out);
    
    
    chrono::milliseconds endTime = duration_cast<chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    cout << "Run time: " << (long)(endTime.count() - startTime.count()) << endl;

    return 0;
}

