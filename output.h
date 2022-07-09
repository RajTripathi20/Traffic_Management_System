#include "solve_2.h"



void generateOutput(Solution* solution, string file) {
    stringstream sout;

    sout << solution->countIntersectionsWithGreenLights() << endl;

    for (IntersectionSolution* i : solution->intersectionSolutions) {
        // Count non-zero duration streets
        int streetCount = 0;
        for (IntersectionLightSchedule* c : i->intersectionLightSchedules)
            if (c->duration > 0)
                streetCount++;

        // Skip intersection - no streets
        if (streetCount == 0)
            continue;

        sout << i->intersectionIndex << endl;
        sout << streetCount << endl;
        for (IntersectionLightSchedule* c : i->intersectionLightSchedules)
            if (c->duration > 0)
                sout << c->street->nameOfStreet << " " << c->duration << endl;
    }

    ofstream fout;
    fout.open(file);
    fout << sout.str();
}
