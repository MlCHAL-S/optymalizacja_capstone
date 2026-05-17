#include <ilcplex/ilocplex.h>

#include <iomanip>
#include <iostream>
#include <vector>

ILOSTLBEGIN

int main() {
    IloEnv env;

    try {
        IloModel model(env);

        const int n = 3;

        std::vector<double> a = {
            1.0,
            -0.4861111111,
            0.75
        };

        const double b = 1.6666666667;
        const double q = 0.1388888889;

        std::vector<int> yUpperBound = {
            10,
            10,
            10
        };

        const double sUpperBound = 20.0;

        std::vector<double> profit = {
            10.0,
            2.0,
            6.0
        };

        const double sPenalty = 1.0;

        std::vector<double> mirCoef = {
            1.0,
            -1.0,
            0.25
        };

        const double mirRhsConst = 1.0;
        const double mirSlackCoef = 0.4166666667;

        IloIntVarArray y(env, n);

        for (int j = 0; j < n; ++j) {
            y[j] = IloIntVar(env, 0, yUpperBound[j]);
        }

        IloNumVar s(env, 0.0, sUpperBound, ILOFLOAT);

        IloExpr objective(env);

        for (int j = 0; j < n; ++j) {
            objective += profit[j] * y[j];
        }

        objective -= sPenalty * s;

        model.add(IloMaximize(env, objective));
        objective.end();

        IloExpr knapsack(env);

        for (int j = 0; j < n; ++j) {
            knapsack += a[j] * y[j];
        }

        model.add(knapsack <= b + q * s);
        knapsack.end();

        IloExpr mir(env);

        for (int j = 0; j < n; ++j) {
            mir += mirCoef[j] * y[j];
        }

        model.add(mir <= mirRhsConst + mirSlackCoef * s);
        mir.end();

        IloCplex cplex(model);
        cplex.solve();

        std::cout << std::fixed << std::setprecision(6);

        std::cout << "Status: " << cplex.getStatus() << "\n";
        std::cout << "Objective value: " << cplex.getObjValue() << "\n\n";

        for (int j = 0; j < n; ++j) {
            std::cout << "y" << j + 1 << " = " << cplex.getValue(y[j]) << "\n";
        }

        std::cout << "s = " << cplex.getValue(s) << "\n\n";

        double knapsackLhs = 0.0;
        double mirLhs = 0.0;

        for (int j = 0; j < n; ++j) {
            knapsackLhs += a[j] * cplex.getValue(y[j]);
            mirLhs += mirCoef[j] * cplex.getValue(y[j]);
        }

        double knapsackRhs = b + q * cplex.getValue(s);
        double mirRhs = mirRhsConst + mirSlackCoef * cplex.getValue(s);

        std::cout << "Knapsack constraint:\n";
        std::cout << "LHS = " << knapsackLhs << "\n";
        std::cout << "RHS = " << knapsackRhs << "\n\n";

        std::cout << "MIR inequality:\n";
        std::cout << "LHS = " << mirLhs << "\n";
        std::cout << "RHS = " << mirRhs << "\n";

    } catch (const IloException& e) {
        std::cerr << "CPLEX error: " << e << "\n";
        env.end();
        return 1;
    }

    env.end();
    return 0;
}