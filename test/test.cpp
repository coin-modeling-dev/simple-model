//
// Created by kingaj on 10/9/18.
//

#include "../src/TensorModel.h"
#include "../src/TmVector.h"

using namespace tensormodel;

int main() {
    cout << "Declaring sets and parameters for model sailco" << endl;

    // Set: Periods
    TmSet tmset_periods(4, "Period");

    // Set: Parameters
    TmSet tmset_parameters(0, "Parameter Names", 5);

    tmset_parameters.addName("regularcost");
    tmset_parameters.addName("extracost");
    tmset_parameters.addName("capacity");
    tmset_parameters.addName("initialInventory");
    tmset_parameters.addName("inventoryCost");

    tmset_parameters.display();

    // Data: parameter values
    TmVector tmvec_parameters(tmset_parameters);
    tmvec_parameters.setName("Parameter Values");

    tmvec_parameters.setValue(400.0, tmset_parameters.getIndxByName("regularcost"));
    tmvec_parameters.setValue(450.0, tmset_parameters.getIndxByName("extracost"));
    tmvec_parameters.setValue(40.0, tmset_parameters.getIndxByName("capacity"));
    tmvec_parameters.setValue(10.0, tmset_parameters.getIndxByName("initialInventory"));
    tmvec_parameters.setValue(20.0, tmset_parameters.getIndxByName("inventoryCost"));

    tmvec_parameters.display();


    // Data: Demand
    TmVector tmvec_demand(tmset_periods);
    tmvec_demand.setName("Demand Values");

    double demand_values[4] = {40, 60, 72, 25};
    for (int i = 0; i < tmset_periods.getNum() ; ++i) {
        if (i==0) {
            demand_values[i] -= tmvec_parameters.getValue(tmset_parameters.getIndxByName("initialInventory"));
        }
        tmvec_demand.setValue(demand_values[i], i);
    }

    tmvec_demand.display();

    // Model Sailco

    cout << "Building data structures for model sailco" << endl;

    // TensorModel: Sailco
    TensorModel tm_sailco;
    double infty = tm_sailco.getInfinity();

    // Periods are stages
    int nStages = tmset_periods.getNum();

    // Variables
    auto regular   = new p_TmVariable [nStages];
    auto extra     = new p_TmVariable [nStages];
    auto inventory = new p_TmVariable [nStages];

    for (int j = 0; j < nStages; ++j) {
        regular[j] = tm_sailco.createColumn("regular", j);
        regular[j]->setHasObjective();
        extra[j] = tm_sailco.createColumn("extra", j);
        extra[j]->setHasObjective();
        inventory[j] = tm_sailco.createColumn("inventory",j);
        inventory[j]->setHasObjective();
    }

    // Constraints
    auto ctCapacity = new p_TmVariable [nStages];
    auto ctBoat = new p_TmVariable [nStages];

    for (int j = 0; j < nStages; ++j) {
        ctCapacity[j] = tm_sailco.createColumn("ctCapacity", j);
        ctBoat[j] = tm_sailco.createColumn("ctBoad", j);
    }


    // Initialize Data Structures (rows, columns, objective)
    tm_sailco.initializeDataStructures();

    // Bounds and objective coefficients for variables
    for (int j = 0; j < nStages; ++j) {
        regular[j]->loadBnd(regular[j]->index(), 0, infty);
        regular[j]->loadObj(regular[j]->index(), tmvec_parameters.getValue(tmset_parameters.getIndxByName("regularcost")));
        extra[j]->loadBnd(extra[j]->index(), 0, infty);
        extra[j]->loadObj(extra[j]->index(), tmvec_parameters.getValue(tmset_parameters.getIndxByName("extracost")));
        inventory[j]->loadBnd(inventory[j]->index(), 0, infty);
        inventory[j]->loadObj(inventory[j]->index(), tmvec_parameters.getValue(tmset_parameters.getIndxByName("inventorycost")));
    }

    // Bounds for constraints
    for (int j = 0; j < nStages; ++j) {
        ctCapacity[j]->loadBnd(ctCapacity[j]->index(), -infty, tmvec_parameters.getValue(tmset_parameters.getIndxByName("capacity")));
        double demand = tmvec_demand.getValue(j);
        ctBoat[j]->loadBnd(ctBoat[j]->index(), demand, demand);
    }

    // ctCapacity coefficients
    for (int i = 0; i < nStages; ++i) {
        // ctCapacity
        int row_index = ctCapacity[i]->operator()();
        //regular
        int col_index = regular[i]->operator()();
        tm_sailco.loadMat(row_index, col_index, 1.0);
    }

    // ctBoat coefficients
    for (int i = 0; i < nStages; ++i) {
        // ctBoat
        int row_index = ctBoat[i]->operator()();

        // regular
        int col_index = regular[i]->operator()();
        tm_sailco.loadMat(row_index, col_index, 1.0);

        // extra
        col_index = extra[i]->operator()();
        tm_sailco.loadMat(row_index, col_index, 1.0);

        // inventory
        col_index = inventory[i]->operator()();
        tm_sailco.loadMat(row_index, col_index, -1.0);

        // lagged inventory
        if (i > 0) {
            col_index = inventory[i-1]->operator()();
            tm_sailco.loadMat(row_index, col_index, 1.0);
        }

    }

    // Load the data into the Model
    tm_sailco.loadSM_Data();

    // Display the model data
    tm_sailco.display(cout, "matrixFile.out");















}

