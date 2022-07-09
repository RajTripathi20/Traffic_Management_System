#include "solve_1.h"



int runSimulationLite(ProblemModel problem, Solution* solution, PVehicleSimultionPosition* vehicleSimultionPositionss) {
    int score = 0;
    int currentTime = 0;

    // Create list of vehicles in simulation
    int vehicleSimultionPositionssSize = problem.vehicles.size();
    int simulationVehicleStart = -(int)(vehicleSimultionPositionssSize + 1);
    for (int i = 0; i < vehicleSimultionPositionssSize; i++)
        vehicleSimultionPositionss[i]->init(simulationVehicleStart++);

    // Init green lights
    for (IntersectionSolution* intersection : solution->intersectionSolutions) {
        intersection->buildGreenLightsArray();
        intersection->LastVehiclePassTime = -1;
    }

    while (currentTime <= problem.simulationTime)
    {
        // Update vehicles
        for (int i = 0; i < vehicleSimultionPositionssSize; i++)
        {
            VehicleSimultionPosition* vehicleSimultionPosition = vehicleSimultionPositionss[i];
            if (vehicleSimultionPosition->timeTravelled > currentTime)
                break;

            //Street *street = vehicleSimultionPosition->MyVehicle->streets[vehicleSimultionPosition->pathIndex];

            IntersectionSolution* intersection = solution->intersectionSolutions[vehicleSimultionPosition->streetEndIntersections[vehicleSimultionPosition->pathIndex]];

            // Check if a vehicle already used this intersection this cycle
            if (intersection->LastVehiclePassTime == currentTime)
                continue;

            if (intersection->GreenLightsArray.size() == 0)
                continue;

            int currentGreenLightstreetIndices = intersection->GreenLightsArray[currentTime % intersection->GreenLightsArray.size()];

            // Not green light, skip to next vehicle
            if (vehicleSimultionPosition->streetIndices[vehicleSimultionPosition->pathIndex] != currentGreenLightstreetIndices)
                continue;

            // Mark intersection as used for this cycle
            intersection->LastVehiclePassTime = currentTime;

            // Process vehicle green light
            vehicleSimultionPosition->pathIndex++;
            //            Street *newStreet = vehicleSimultionPosition->getCurrentStreet();
            int newStreetLength = vehicleSimultionPosition->streetLengths[vehicleSimultionPosition->pathIndex];
            vehicleSimultionPosition->timeTravelled = currentTime + newStreetLength;
            vehicleSimultionPosition->timeRemaining -= newStreetLength;

            // Check if vehicle finished
            if (vehicleSimultionPosition->pathIndex == vehicleSimultionPosition->MyVehicle->streets.size() - 1)
            {
                // Check if finished on time - if so give bonus
                if (vehicleSimultionPosition->timeTravelled <= problem.simulationTime)
                    score += problem.baseScore + (problem.simulationTime - vehicleSimultionPosition->timeTravelled);

                // Move to end
                vehicleSimultionPosition->timeTravelled = problem.simulationTime + 1;
                memmove(&vehicleSimultionPositionss[i], &vehicleSimultionPositionss[i + 1], (sizeof(PVehicleSimultionPosition)) * (vehicleSimultionPositionssSize - (i + 1)));
                vehicleSimultionPositionss[vehicleSimultionPositionssSize - 1] = vehicleSimultionPosition;

                i--;
            }
            else
            {
                // Add the vehicle in the right place
                if (i + 1 < vehicleSimultionPositionssSize) {
                    int newPos = binarySearch(vehicleSimultionPositionss, i + 1, vehicleSimultionPositionssSize - 1, vehicleSimultionPosition->timeTravelled);
                    if (newPos != i) {
                        memmove(&vehicleSimultionPositionss[i], &vehicleSimultionPositionss[i + 1], (sizeof(PVehicleSimultionPosition)) * (newPos - i));
                        vehicleSimultionPositionss[newPos] = vehicleSimultionPosition;
                        i--;
                    }
                }
            }
        }

        currentTime++;
    }

    return score;
}

int runSimulationLite(ProblemModel problem, Solution* solution) {
    PVehicleSimultionPosition* vehicleSimultionPositionss = new PVehicleSimultionPosition[problem.vehicles.size()];
    int vehicleSimultionPositionssSize = problem.vehicles.size();

    for (int i = 0; i < vehicleSimultionPositionssSize; i++)
        vehicleSimultionPositionss[i] = new VehicleSimultionPosition(problem.vehicles[i], 0);

    int score = runSimulationLite(problem, solution, vehicleSimultionPositionss);

    for (int i = 0; i < vehicleSimultionPositionssSize; i++)
        delete vehicleSimultionPositionss[i];
    delete[] vehicleSimultionPositionss;

    return score;
}

int weightedSchedule(ProblemModel problem, Solution* solution)  {
    
    vector<int> countTrafficIntersection;
    countTrafficIntersection.resize(problem.intersections.size(),0);
    vector<int> busiestGreenLightInIntersection;
    busiestGreenLightInIntersection.resize(problem.intersections.size(),0);
        
    PVehicleSimultionPosition* vehicleSimultionPositionss = new PVehicleSimultionPosition[problem.vehicles.size()];
    int vehicleSimultionPositionssSize = problem.vehicles.size();

    for (int i = 0; i < vehicleSimultionPositionssSize; i++)
        vehicleSimultionPositionss[i] = new VehicleSimultionPosition(problem.vehicles[i], 0);
        
        
    int score = 0;
    int currentTime = 0;

    // Create list of vehicles in simulation
   // int vehicleSimultionPositionssSize = problem.vehicles.size();
    int simulationVehicleStart = -(int)(vehicleSimultionPositionssSize + 1);
    for (int i = 0; i < vehicleSimultionPositionssSize; i++)
        vehicleSimultionPositionss[i]->init(simulationVehicleStart++);

    // Init green lights
    for (IntersectionSolution* intersection : solution->intersectionSolutions) {
        intersection->buildGreenLightsArray();
        intersection->LastVehiclePassTime = -1;
    }

    while (currentTime <= problem.simulationTime)
    {
        // Update vehicles
        for (int i = 0; i < vehicleSimultionPositionssSize; i++)
        {
            VehicleSimultionPosition* vehicleSimultionPosition = vehicleSimultionPositionss[i];
            if (vehicleSimultionPosition->timeTravelled > currentTime)
                break;

            //Street *street = vehicleSimultionPosition->MyVehicle->streets[vehicleSimultionPosition->pathIndex];

            IntersectionSolution* intersection = solution->intersectionSolutions[vehicleSimultionPosition->streetEndIntersections[vehicleSimultionPosition->pathIndex]];

            // Check if a vehicle already used this intersection this cycle
            if (intersection->LastVehiclePassTime == currentTime)
                continue;

            if (intersection->GreenLightsArray.size() == 0)
                continue;

            int currentGreenLightstreetIndices = intersection->GreenLightsArray[currentTime % intersection->GreenLightsArray.size()];

            // Not green light, skip to next vehicle
            //if (vehicleSimultionPosition->streetIndices[vehicleSimultionPosition->pathIndex] != currentGreenLightstreetIndices)
            //    continue;
            if(vehicleSimultionPosition->streetIndices[vehicleSimultionPosition->pathIndex] == currentGreenLightstreetIndices)  {
            // Mark intersection as used for this cycle
            intersection->LastVehiclePassTime = currentTime;

            // Process vehicle green light
            vehicleSimultionPosition->pathIndex++; }
            //            Street *newStreet = vehicleSimultionPosition->getCurrentStreet();
            int newStreetLength = vehicleSimultionPosition->streetLengths[vehicleSimultionPosition->pathIndex];
            vehicleSimultionPosition->timeTravelled = currentTime + newStreetLength;
            vehicleSimultionPosition->timeRemaining -= newStreetLength;

            // Check if vehicle finished
           // cout<<vehicleSimultionPosition->pathIndex<<" ";
            if (vehicleSimultionPosition->pathIndex == vehicleSimultionPosition->MyVehicle->streets.size() - 1)
            {
                // Check if finished on time - if so give bonus
                if (vehicleSimultionPosition->timeTravelled <= problem.simulationTime)
                    score += problem.baseScore + (problem.simulationTime - vehicleSimultionPosition->timeTravelled);

                // Move to end
                vehicleSimultionPosition->timeTravelled = problem.simulationTime + 1;
                memmove(&vehicleSimultionPositionss[i], &vehicleSimultionPositionss[i + 1], (sizeof(PVehicleSimultionPosition)) * (vehicleSimultionPositionssSize - (i + 1)));
                vehicleSimultionPositionss[vehicleSimultionPositionssSize - 1] = vehicleSimultionPosition;

                i--;
                                                            //cout<<"I was hereee ";

            }
            else
            {
                // Add the vehicle in the right place
                countTrafficIntersection[vehicleSimultionPosition->streetEndIntersections[vehicleSimultionPosition->pathIndex]]++;
            //    countTrafficStreet[vehicleSimultionPosition->streetIndices[vehicleSimultionPosition->pathIndex]]++;
                
                for(IntersectionSolution* s : solution->intersectionSolutions)  {
                    if(s->intersectionIndex==vehicleSimultionPosition->streetEndIntersections[vehicleSimultionPosition->pathIndex])  {
                        int counter = 0;
                        for(IntersectionLightSchedule* g : s->intersectionLightSchedules)  {
                            if(vehicleSimultionPosition->streetIndices[vehicleSimultionPosition->pathIndex]==g->street->streetIndex)  {
                               // g->Duration = g->Duration + 1;
                                //solution->intersectionSolutions[s->intersectionIndex]->intersectionLightSchedules[counter]->Duration++;
                                busiestGreenLightInIntersection[vehicleSimultionPosition->streetEndIntersections[vehicleSimultionPosition->pathIndex]]=counter;
                                //cout<<"I was here";
                            }
                            counter++;
                        }
                    }
                }
                
                
                                                //cout<<"I was heree ";
               /* for(IntersectionSolution* s : solution->intersectionSolutions)  {
                    if(s->intersectionIndex==vehicleSimultionPosition->streetEndIntersections[vehicleSimultionPosition->pathIndex])  {
                        int counter = 0;
                        for(IntersectionLightSchedule* g : s->intersectionLightSchedules)  {
                            if(vehicleSimultionPosition->streetIndices[vehicleSimultionPosition->pathIndex]==g->MyStreet->UniqueID)  {
                               // g->Duration = g->Duration + 1;
                                solution->intersectionSolutions[s->intersectionIndex]->intersectionLightSchedules[counter]->Duration++;
                                cout<<"I was here";
                                goto label;
                            }
                            counter++;
                        }
                    }
                }*/
            }
        }

        currentTime++;
    }
    
    for(float i = 0; i<=(countTrafficIntersection.size()/50.0); i++)  {
        //cout<<i<<" "<<(countTrafficIntersection.size()/50.0)<<" ";
        int busiestIntersectionID = max_element(countTrafficIntersection.begin(),countTrafficIntersection.end()) - countTrafficIntersection.begin(); 
        if(countTrafficIntersection[busiestIntersectionID]!=0)  {
            solution->intersectionSolutions[busiestIntersectionID]->intersectionLightSchedules[busiestGreenLightInIntersection[busiestIntersectionID]]->duration++;
            //cout<<i<<" "<<solution->intersectionSolutions[busiestIntersectionID]->intersectionLightSchedules[busiestGreenLightInIntersection[busiestIntersectionID]]->MyStreet->Name<<" "<<solution->intersectionSolutions[busiestIntersectionID]->intersectionLightSchedules[busiestGreenLightInIntersection[busiestIntersectionID]]->Duration<<"\n";
        }
        countTrafficIntersection[busiestIntersectionID] = 0;
    }
    

    return runSimulationLite(problem, solution);
 
    
}
