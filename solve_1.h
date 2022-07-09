#include "initialise_solution.h"



//modelling a vehicle during a simulation
class VehicleSimultionPosition {
public:
    Vehicle* MyVehicle;
    int pathIndex;
    int timeTravelled;
    int timeRemaining;
    vector<int> streetLengths;
    vector<int> streetIndices;
    vector<int> streetEndIntersections;

    VehicleSimultionPosition(Vehicle* vehicle, int timeTravelled) {
        this->MyVehicle = vehicle;
        init(timeTravelled);

        for (int i = 0; i < MyVehicle->streets.size(); i++) {
            streetIndices.push_back(MyVehicle->streets[i]->streetIndex);
            streetEndIntersections.push_back(MyVehicle->streets[i]->endPoint);
            streetLengths.push_back(MyVehicle->streets[i]->travelTime);
        }
    }

    void init(int timeTravelled) {
        this->pathIndex = 0;
        this->timeTravelled = timeTravelled;
        this->timeRemaining = MyVehicle->timeRemaining();
    }

    Street* getCurrentStreet()
    {
        return MyVehicle->streets[pathIndex];
    }
    //Understand TimeGotHere meaning
    static bool compareByTimeTravelled(VehicleSimultionPosition* a, VehicleSimultionPosition* b) {
        return a->timeTravelled < b->timeTravelled;
    }
};




//utility function
template< typename T, typename Pred >
typename std::vector<T>::iterator
insert_sorted(std::vector<T>& vec, T const& item, Pred pred)
{
    return vec.insert(std::lower_bound(vec.begin(), vec.end(), item, pred), item);
}



typedef VehicleSimultionPosition* PVehicleSimultionPosition;

//utility function
static int binarySearch(PVehicleSimultionPosition* arr, int low, int high, int key)
{
    int mid;
    int lbound = low;
    int ubound = high;
    while (true) {
        mid = (lbound + ubound) / 2;
        if (lbound == ubound) {
            if (key > arr[mid]->timeTravelled)
                return mid;
            return mid - 1;
        }
        else if (key == arr[mid]->timeTravelled)
            return mid - 1;
        else if (key > arr[mid]->timeTravelled)
            lbound = mid + 1;
        else
            ubound = mid;
    }
}





bool optimizeGreenLightOrder_SortVehicles(VehicleSimultionPosition* a, VehicleSimultionPosition* b) {
    int aStreetsLeft = a->MyVehicle->streets.size() - a->pathIndex;
    int bStreetsLeft = b->MyVehicle->streets.size() - b->pathIndex;

    return aStreetsLeft > bStreetsLeft;
}




int optimizeGreenLightOrder(ProblemModel problem, Solution* solution)
{
    //VehicleSimultionPositionBytimeTravelled vehicleSimultionPositionByTimeTravelled = new VehicleSimultionPositionByTimeTravelled();

    int score = 0;
    int currentTime = 0;

    vector<VehicleSimultionPosition*> allSimulationVehicles;
    map<int, vector<VehicleSimultionPosition*>> vehicleQueueByIntersection;

    // Create vehicle queue in each intersection
    int simulationVehicleStart = -(int)(problem.vehicles.size() + 1);
    for (Vehicle* vehicle : problem.vehicles) {
        VehicleSimultionPosition* simulationVehicle = new VehicleSimultionPosition(vehicle, simulationVehicleStart);
        allSimulationVehicles.push_back(simulationVehicle);
        vehicleQueueByIntersection[vehicle->streets[0]->endPoint].push_back(simulationVehicle);
        simulationVehicleStart++;
    }

    // Init green lights
    for (IntersectionSolution* intersection : solution->intersectionSolutions) {
        intersection->CurrentGreenLight = 0;
        if (intersection->intersectionLightSchedules.size() > 0)
            intersection->CurrentGreenLightChangeTime = intersection->intersectionLightSchedules[0]->duration;
        else
            // Skip this
            intersection->CurrentGreenLightChangeTime = problem.simulationTime + 1;
    }

    unordered_set<int> firstVehiclePerStreetFound;
    vector<VehicleSimultionPosition*> firstVehiclePerStreet;

    while (currentTime <= problem.simulationTime)
    {
        // Update traffic lights cycle
        for (IntersectionSolution* intersection : solution->intersectionSolutions) {
            if (intersection->intersectionLightSchedules.size() == 0)
                continue;

            // Update intersection green light
            if (intersection->CurrentGreenLightChangeTime <= currentTime)
            {
                intersection->CurrentGreenLight = (intersection->CurrentGreenLight + 1) % intersection->intersectionLightSchedules.size();
                intersection->CurrentGreenLightChangeTime = currentTime + intersection->intersectionLightSchedules[intersection->CurrentGreenLight]->duration;
            }

            // Update intersection vehicle
            vector<VehicleSimultionPosition*>* vehicleQueue = &vehicleQueueByIntersection[intersection->intersectionIndex];

            // If it's possible to change the current green light - try selecting the best one
            if (!intersection->intersectionLightSchedules[intersection->CurrentGreenLight]->greenLightUsed)
            {
                firstVehiclePerStreetFound.clear();
                firstVehiclePerStreet.clear();

                for (int i = 0; i < vehicleQueue->size(); i++)
                {
                    VehicleSimultionPosition* vehicleSimultionPosition = (*vehicleQueue)[i];
                    if (vehicleSimultionPosition->timeTravelled > currentTime)
                        break;

                    int streetId = vehicleSimultionPosition->getCurrentStreet()->streetIndex;
                    if (firstVehiclePerStreetFound.find(streetId) == firstVehiclePerStreetFound.end())
                    {
                        firstVehiclePerStreetFound.insert(streetId);
                        firstVehiclePerStreet.push_back(vehicleSimultionPosition);
                    }
                }

                if (firstVehiclePerStreet.size() == 0)
                    continue;

                sort(firstVehiclePerStreet.begin(), firstVehiclePerStreet.end(), optimizeGreenLightOrder_SortVehicles);

                for (VehicleSimultionPosition* vehicleSimultionPosition : firstVehiclePerStreet) {
                    // Find required green light
                    int requiredGreenLight = -1;
                    Street* street = vehicleSimultionPosition->getCurrentStreet();
                    for (int g = 0; g < intersection->intersectionLightSchedules.size(); g++)
                        if (street->streetIndex == intersection->intersectionLightSchedules[g]->street->streetIndex) {
                            requiredGreenLight = g;
                            break;
                        }

                    // Required green light not found - skip
                    if (requiredGreenLight == -1)
                        continue;

                    // Required green light already used - skip
                    if (intersection->intersectionLightSchedules[requiredGreenLight]->greenLightUsed)
                        continue;

                    // Swap possible only if it's the same green light duration
                    if (intersection->intersectionLightSchedules[requiredGreenLight]->duration != intersection->intersectionLightSchedules[intersection->CurrentGreenLight]->duration)
                        continue;

                    // Swap green lights - now the vehicle can continue!
                    IntersectionLightSchedule* tmp = intersection->intersectionLightSchedules[requiredGreenLight];
                    intersection->intersectionLightSchedules[requiredGreenLight] = intersection->intersectionLightSchedules[intersection->CurrentGreenLight];
                    intersection->intersectionLightSchedules[intersection->CurrentGreenLight] = tmp;
                    tmp->greenLightUsed = true;
                    break;
                }
            }

            bool vehiclePassed = false;
            for (int i = 0; i < vehicleQueue->size(); i++)
            {
                VehicleSimultionPosition* vehicleSimultionPosition = (*vehicleQueue)[i];
                if (vehicleSimultionPosition->timeTravelled > currentTime)
                    break;

                Street* street = vehicleSimultionPosition->getCurrentStreet();
                if (vehiclePassed)
                    continue;

                // Not green light
                if (street->streetIndex != intersection->getGreenLightStreet()->streetIndex)
                    continue;

                // Mark intersection as used for this cycle
                vehiclePassed = true;

                intersection->intersectionLightSchedules[intersection->CurrentGreenLight]->greenLightUsed = true;

                // Process vehicle green light
                vehicleSimultionPosition->pathIndex++;
                Street* newStreet = vehicleSimultionPosition->getCurrentStreet();
                vehicleSimultionPosition->timeTravelled = currentTime + newStreet->travelTime;
                vehicleSimultionPosition->timeRemaining -= newStreet->travelTime;

                // Check if vehicle finished
                vehicleQueue->erase(vehicleQueue->begin() + i);
                i--;

                if (vehicleSimultionPosition->pathIndex == vehicleSimultionPosition->MyVehicle->streets.size() - 1)
                {
                    // Check if finished on time - if so give bonus
                    if (vehicleSimultionPosition->timeTravelled <= problem.simulationTime)
                        score += problem.baseScore + (problem.simulationTime - vehicleSimultionPosition->timeTravelled);
                }
                else
                {
                    // Vehicle not finished - add it to the next intersection
                    insert_sorted(vehicleQueueByIntersection[newStreet->endPoint], vehicleSimultionPosition, VehicleSimultionPosition::compareByTimeTravelled);
                }
            }
        }

        currentTime++;
    }

    for (VehicleSimultionPosition* vehicle : allSimulationVehicles)
        delete vehicle;

    return score;
}