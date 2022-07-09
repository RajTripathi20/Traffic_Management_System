#include "models.h"





class IntersectionLightSchedule {
public:
    int duration;
    Street* street;

    // Temporary used during simulation
    bool greenLightUsed;

    IntersectionLightSchedule(Street* street, int duration) {
        this->street = street;
        this->duration = duration;

        this->greenLightUsed = false;
    }

    IntersectionLightSchedule* clone() {
        return new IntersectionLightSchedule(this->street, this->duration);
    }
};



class IntersectionSolution {
public:
    int intersectionIndex;
    vector<IntersectionLightSchedule*> intersectionLightSchedules;

    // Temporary used during simulation
    int CurrentGreenLight;
    int CurrentGreenLightChangeTime;
    int LastVehiclePassTime;

    // Temporary used during simulation
    vector<int> GreenLightsArray;

    IntersectionSolution(int intersectionIndex) {
        this->intersectionIndex = intersectionIndex;
        CurrentGreenLightChangeTime = 0;
        CurrentGreenLight = 0;
    }

    ~IntersectionSolution() {
        for (IntersectionLightSchedule* g : intersectionLightSchedules)
            delete g;
    }

    IntersectionSolution* clone() {
        IntersectionSolution* intersectionSolution = new IntersectionSolution(this->intersectionIndex);
        intersectionSolution->intersectionLightSchedules.resize(intersectionLightSchedules.size());
        for (int i = 0; i < intersectionLightSchedules.size(); i++)
            intersectionSolution->intersectionLightSchedules[i] = intersectionLightSchedules[i]->clone();

        return intersectionSolution;
    }

    void buildGreenLightsArray() {
        int greenLightsCycleDuration = 0;
        for (IntersectionLightSchedule* intersectionLightSchedule : intersectionLightSchedules)
            greenLightsCycleDuration += intersectionLightSchedule->duration;

        GreenLightsArray.clear();

        for (IntersectionLightSchedule* intersectionLightSchedule : intersectionLightSchedules) {
            for (int i = 0; i < intersectionLightSchedule->duration; i++)
                GreenLightsArray.push_back(intersectionLightSchedule->street->streetIndex);
        }
    }

    int countIntersectionsUsed() {
        if (intersectionLightSchedules.size() == 0)
            return 0;

        int intersectionsUsed = 0;
        for (IntersectionLightSchedule* intersectionLightSchedule : intersectionLightSchedules)
            if (intersectionLightSchedule->duration > 0)
                intersectionsUsed++;

        return intersectionsUsed;
    }

    Street* getGreenLightStreet() {
        return intersectionLightSchedules[CurrentGreenLight]->street;
    }

    bool anyIntersectionUsed() {
        return countIntersectionsUsed() > 0;
    }
};




class Solution {
public:
    vector<IntersectionSolution*> intersectionSolutions;

    Solution(int numberOfIntersections) {
        intersectionSolutions.resize(numberOfIntersections);
        for (int i = 0; i < numberOfIntersections; i++)
            intersectionSolutions[i] = new IntersectionSolution(i);
    }

/*    Solution(Solution* other) {
        intersections.resize(other->intersections.size());
        for (int i = 0; i < other->intersections.size(); i++)
            intersections[i] = other->intersections[i]->clone();
    }

    ~Solution() {
        for (SolutionIntersection* i : intersections)
            delete i;
    }

    Solution* clone() {
        return new Solution(this);
    }*/

    int countIntersectionsWithGreenLights()
    {
        int count = 0;
        for (IntersectionSolution* intersection : intersectionSolutions)
            if (intersection->anyIntersectionUsed())
                count++;

        return count;
    }
};




void initSolution(ProblemModel model, Solution* solution) {
    for (Intersection* i : model.intersections) {
        solution->intersectionSolutions[i->intersectionIndex]->intersectionLightSchedules.clear();
        for (Street* street : i->inStreets) {
            IntersectionLightSchedule* schedule = new IntersectionLightSchedule(street, 1);

            solution->intersectionSolutions[i->intersectionIndex]->intersectionLightSchedules.push_back(schedule);
        }
    }
}