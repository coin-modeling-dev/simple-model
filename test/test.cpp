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
    TmSet tmset_parameters(5, "Parameter Names", 5);

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
    for (unsigned int i = 0; i < tmset_periods.getNum() ; ++i) {
        if (i==0) {
            demand_values[i] -= tmvec_parameters.getValue(tmset_parameters.getIndxByName("initialInventory"));
        }
        tmvec_demand.setValue(demand_values[i], tmvec_demand.index(i));
    }

    tmvec_demand.display();

    // Model Sailco

    cout << "Building data structures for model sailco" << endl;

    // TensorModel: Sailco
    TensorModel tm_sailco;
    double infty = tm_sailco.getInfinity();

    // Columns
    auto regular = tm_sailco.createColumn("regular", tmset_periods);
    auto extra = tm_sailco.createColumn("extra", tmset_periods);
    auto inventory = tm_sailco.createColumn("inventory", tmset_periods);


    // Constraints
    auto ctCapacity = tm_sailco.createRow("ctCapacity", tmset_periods);
    auto ctBoat = tm_sailco.createRow("ctBoat", tmset_periods);


    // Initialize Data Structures (rows, columns, objective)
    tm_sailco.initializeDataStructures();

    // Bounds and objective coefficients for variables
    for (unsigned int j = 0; j < tmset_periods.getNum(); ++j) {
        regular->loadBnd(regular->index(j), 0, infty);
        regular->loadObj(regular->index(j), tmvec_parameters.getValue(tmset_parameters.getIndxByName("regularcost")));
        extra->loadBnd(regular->index(j), 0, infty);
        extra->loadObj(regular->index(j), tmvec_parameters.getValue(tmset_parameters.getIndxByName("extracost")));
        inventory->loadBnd(inventory->index(j), 0, infty);
        inventory->loadObj(inventory->index(j), tmvec_parameters.getValue(tmset_parameters.getIndxByName("inventoryCost")));
    }

    // Bounds for constraints
    for (unsigned int j = 0; j < tmset_periods.getNum(); ++j) {
        ctCapacity->loadBnd(ctCapacity->index(j), -infty, tmvec_parameters.getValue(tmset_parameters.getIndxByName("capacity")));
        double demand = tmvec_demand.getValue(tmvec_demand.index(j));
        ctBoat->loadBnd(ctBoat->index(j), demand, demand);
    }

    // ctCapacity coefficients
    for (unsigned int i = 0; i < tmset_periods.getNum(); ++i) {
        // ctCapacity
        int row_index = ctCapacity->operator()(i);
        //regular
        int col_index = regular->operator()(i);
        tm_sailco.loadMat(row_index, col_index, 1.0);
    }

    // ctBoat coefficients
    for (int i = 0; i < tmset_periods.getNum(); ++i) {
        // ctBoat
        int row_index = ctBoat->operator()(i);

        // regular
        int col_index = regular->operator()(i);
        tm_sailco.loadMat(row_index, col_index, 1.0);

        // extra
        col_index = extra->operator()(i);
        tm_sailco.loadMat(row_index, col_index, 1.0);

        // inventory
        col_index = inventory->operator()(i);
        tm_sailco.loadMat(row_index, col_index, -1.0);

        // lagged inventory
        if (i > 0) {
            col_index = inventory->operator()(i-1);
            tm_sailco.loadMat(row_index, col_index, 1.0);
        }

    }

    // Load the data into the Model
    tm_sailco.loadSM_Data();

    // Display the model data
    tm_sailco.display(cout, "matrixFile.out");

}

