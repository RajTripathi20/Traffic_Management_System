#include <stdio.h>
#include <bits/stdc++.h>
using namespace std;

//modelling streets as mentioned in the proposal
class Street  {
    public:
    int streetIndex;
    int startPoint; //index of intersection
    int endPoint; //index of intersection
    string nameOfStreet;
    int travelTime;
    int incomingUsageCount;
    
    
    
        Street(int streetIndex, int startPoint, int endPoint, string nameOfStreet, int travelTime) {
        this->streetIndex = streetIndex;
        this->startPoint = startPoint;
        this->endPoint = endPoint;
        this->nameOfStreet = nameOfStreet;
        this->travelTime = travelTime;
        this->incomingUsageCount = 0;
        //this->CarsOnStart = 0;
    }
};


//modelling vehicles as mentioned in the proposal
class Vehicle  {
    public:
    int vehicleIndex;
    vector<Street*> streets;  //path travelled by the car
    
    
    Vehicle(int vehicleIndex, vector<Street*> streets) {
        this->vehicleIndex = vehicleIndex;
        this->streets = streets;
    }
    
    int timeRemaining() {
        int time = 0;
        for (int i = 1; i < streets.size(); i++)
            time += streets[i]->travelTime;

        return time;
    }
    
};

//modelling intersections as mentioned in the proposal
class Intersection  {
    public:
    int intersectionIndex;
    vector<Street*> inStreets;
    vector<Street*> outStreets;
    
    Intersection(int intersectionIndex) {
        this->intersectionIndex = intersectionIndex;
    }
    
};



//modelling the problem mathematically
class ProblemModel  {
    public:
    int simulationTime; 
    int baseScore;
    vector<Intersection*> intersections;
    vector<Street*> streets;
    vector<Vehicle*> vehicles; //
    
    void buildModelFromFile(string file);
    int maxPossibleScore(); //Score if all vehicles reached destination in least possible time
    int cleanModel();  // Remove any redundancies like streets with no vehicles
};

int ProblemModel::maxPossibleScore()  {
    
    int maxPossibleScore = 0;
        for (Vehicle* vehicle : this->vehicles) {
            int maxBonusTime = this->simulationTime - vehicle->timeRemaining();
            // Car can not finish in time
            if (maxBonusTime < 0)
                continue;

            maxPossibleScore += maxBonusTime + this->baseScore;
        }

        return maxPossibleScore;
}

int ProblemModel::cleanModel()  {
    int streetsRemoved = 0;

        for (Intersection* intersection : intersections) {
            for (int i = intersection->inStreets.size() - 1; i >= 0; i--) {
                Street* inStreet = intersection->inStreets[i];
                if (inStreet->incomingUsageCount > 0)
                    continue;

                streetsRemoved++;
                intersection->inStreets.erase(intersection->inStreets.begin() + i);
            }
        }

        return streetsRemoved;
}

void ProblemModel::buildModelFromFile(string file)  {
    
    int simulationTime, numberOfStreets, numberOfIntersections, numberOfVehicles, baseScore;
        map<string, Street*> mapStreetWithName;
        
        ifstream fin(file);
        fin >> simulationTime;
        fin >> numberOfIntersections;
        fin >> numberOfStreets;
        fin >> numberOfVehicles;
        fin >> baseScore;

        this->simulationTime = simulationTime;
        this->baseScore = baseScore;

        // Load streets
        int streetId = 0; 
        while (streetId < numberOfStreets) {
            int start, end, t;
            string name;
            fin >> start;
            fin >> end;
            fin >> name;
            fin >> t;
            Street* street = new Street(streetId, start, end, name, t);
            mapStreetWithName[name] = street;

            this->streets.push_back(street);
            streetId++;
        }



        // Load vehicles
        int vehicleId = 0;
        while (vehicleId < numberOfVehicles) {
            int numberOfStreetsInPath;
            fin >> numberOfStreetsInPath;
            vector<Street*> streetsInPath;
            
            int vehiclePath = 0;
            while (vehiclePath < numberOfStreetsInPath) {
                string streetName;
                fin >> streetName;
                streetsInPath.push_back(mapStreetWithName[streetName]);
                vehiclePath++;
            }
            this->vehicles.push_back(new Vehicle(vehicleId, streetsInPath));
            vehicleId++;
        }
        
        
        
        
        
        

        // Build intersections
        for (int i = 0; i < numberOfIntersections; i++)
            this->intersections.push_back(new Intersection(i));

        for (Street* street : this->streets) {
            this->intersections[street->startPoint]->outStreets.push_back(street);
            this->intersections[street->endPoint]->inStreets.push_back(street);
        }

        // Count number of times a street is used as incoming street
        for (Vehicle* vehicle : this->vehicles)
            for (int i = 0; i < vehicle->streets.size() - 1; i++)
                vehicle->streets[i]->incomingUsageCount++;

        /*// Update the number of cars on the street at the start
        for (Street* street : this->Streets)
            street->CarsOnStart = 0;

        for (Car* car : this->Cars)
            car->Streets[0]->CarsOnStart++;*/

}