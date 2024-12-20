#include <iostream>
#include <cstring>
#include <vector>
#include <string>

/* Brings in the CPLEX function declarations */
#include <algorithm>
#include <cassert>
#include <fstream>
#include <ilcplex/cplex.h>
#include "cpxmacro.h"





int getSingleCoord(int num_vertex, int i, int j) {
    assert(i!=j);
    return i * num_vertex + j - (i + (j > i));
}


void fill_colname(const int NUM_ROWS, char **ycolname, char **xcolname) {
    char **yc_p = ycolname;
    char **xc_p = xcolname;
    for (int i = 0; i < NUM_ROWS; ++i) {
        for (int j = 0; j < NUM_ROWS; ++j) {
            if (i != j) {
                *yc_p = new char[16];
                *xc_p = new char[16];
                snprintf(*yc_p++, 16, "y%d!%d", i, j);
                snprintf(*xc_p++, 16, "x%d!%d", i, j);
            }
        }
    }
    assert(yc_p - ycolname == NUM_ROWS * NUM_ROWS - NUM_ROWS);
}

int main() {
    int status;
    char errmsg[BUF_SIZE];
    DECL_ENV(env);
    DECL_PROB(env, prob);
    constexpr int NUM_ROWS = 8;
    const int NUM_VARS = NUM_ROWS * NUM_ROWS - NUM_ROWS;
    const double distance_matrix[NUM_ROWS][NUM_ROWS] = {
        { 0.00, 0.77, 1.41, 1.85, 2.00, 1.85, 1.41, 0.77 },
        { 0.77, 0.00, 0.77, 1.41, 1.85, 2.00, 1.85, 1.41 },
        { 1.41, 0.77, 0.00, 0.77, 1.41, 1.85, 2.00, 1.85 },
        { 1.85, 1.41, 0.77, 0.00, 0.77, 1.41, 1.85, 2.00 },
        { 2.00, 1.85, 1.41, 0.77, 0.00, 0.77, 1.41, 1.85 },
        { 1.85, 2.00, 1.85, 1.41, 0.77, 0.00, 0.77, 1.41 },
        { 1.41, 1.85, 2.00, 1.85, 1.41, 0.77, 0.00, 0.77 },
        { 0.77, 1.41, 1.85, 2.00, 1.85, 1.41, 0.77, 0.00 }
    };

    char **ycolname = new char *[2 * NUM_VARS];
    char **xcolname = ycolname + NUM_VARS;
    fill_colname(NUM_ROWS, ycolname, xcolname);


    char *ctype = new char[2 * NUM_VARS];
    for (int i = 0; i < NUM_VARS; ++i) {
        ctype[i] = 'B';
        ctype[i + NUM_VARS] = 'I';
    }

    std::vector<double> objCost;
    objCost.reserve(2 * NUM_VARS);
    for (int i = 0; i < NUM_ROWS; ++i) {
        for (int j = 0; j < NUM_ROWS; ++j) {
            if (i != j) {
                objCost.push_back(distance_matrix[i][j]);
            }
        }
    }
    for (int i = 0; i < NUM_VARS; ++i) {
        objCost.push_back(0.0);
    }


    const int N_ROWS_F = NUM_ROWS - 1;
    const int N_VARS_PER_LINE_F = 2 * N_ROWS_F - 1;
    const int FNZCNT = N_VARS_PER_LINE_F * N_ROWS_F;
    std::vector<double> Frhs(N_ROWS_F, 1.0);
    std::vector<double> Frmatval(FNZCNT);
    std::vector<int> Frmatbeg(NUM_ROWS - 1);
    for (int i = 0; i < NUM_ROWS - 1; ++i) {
        Frmatbeg[i] = i * N_VARS_PER_LINE_F;
    }

    std::vector<int> Frmatind(FNZCNT);
    for (int i = 1, *pt = &Frmatind[0]; i < NUM_ROWS; ++i) {
        *pt = NUM_VARS + getSingleCoord(NUM_ROWS, 0, i);
        int *ent_edges = pt + 1;
        int *exit_edges = pt + NUM_ROWS - 1;
        for (int j = 1; j < NUM_ROWS; ++j) {
            if (i != j) {
                *ent_edges++ = NUM_VARS + getSingleCoord(NUM_ROWS, j, i);
                *exit_edges++ = NUM_VARS + getSingleCoord(NUM_ROWS, i, j);
            }
        }
        pt += N_VARS_PER_LINE_F;
    }

    for (int i = 0; i < FNZCNT; ++i) {
        if (i % N_VARS_PER_LINE_F < NUM_ROWS - 1) Frmatval[i] = 1.0;
        else Frmatval[i] = -1.0;
    }

    std::vector<double> lb(2 * NUM_VARS, 0.0);
    std::vector<double> ub(NUM_VARS, 1.0);
    for (int k = 0; k < NUM_VARS; ++k) {
        ub.push_back(CPX_INFBOUND);
    }
    char **Frowname = new char *[NUM_ROWS - 1];
    for (int k = 0; k < NUM_ROWS - 1; ++k) {
        Frowname[k] = new char[32];
        snprintf(Frowname[k], 32, "Net flow of node %d", k + 1);
    }


    std::vector<char> Fsense(N_ROWS_F, 'E');


    std::vector<double> Hrhs(NUM_ROWS, 1.0);
    std::vector<char> Hsense(NUM_ROWS, 'E');

    CHECKED_CPX_CALL(CPXnewcols, env, prob, 2 * NUM_VARS, &objCost[0], &lb[0], &ub[0], &ctype[0], &ycolname[0]);
    CHECKED_CPX_CALL(CPXchgobjsen, env, prob,CPX_MIN);
    CHECKED_CPX_CALL(CPXaddrows, env, prob, 0, N_ROWS_F, FNZCNT, &Frhs[0], &Fsense[0], &Frmatbeg[0], &Frmatind[0], &Frmatval[0],
               nullptr, &Frowname[0]);

    std::vector<double> Grhs(NUM_ROWS, 1.0);
    std::vector<char> Gsense(NUM_ROWS, 'E');

    char **Growname = new char *[NUM_ROWS];
    char **Hrowname = new char *[NUM_ROWS];
    for (int k = 0; k < NUM_ROWS; ++k) {
        Growname[k] = new char[64];
        Hrowname[k] = new char[64];
        snprintf(Growname[k], 64, "Incoming arcs node %d", k);
        snprintf(Hrowname[k], 64, "Outcoming arcs node %d", k);
    }

    std::vector<int> Grmatbeg(NUM_ROWS);
    for (int i = 0; i < NUM_ROWS; ++i) {
        Grmatbeg[i] = i * (NUM_ROWS - 1);
    }

    std::vector<int> Grmatind(NUM_VARS);
    std::vector<double> Grmatval(NUM_VARS, 1.0);
    for (int i = 0, *G_p = &Grmatind[0]; i < NUM_ROWS; ++i) {
        for (int j = 0; j < NUM_ROWS; ++j) {
            if (i != j) {
                *G_p++ = getSingleCoord(NUM_ROWS, j, i);
            }
        }
    }


    CHECKED_CPX_CALL(CPXaddrows, env, prob, 0, NUM_ROWS, NUM_VARS, &Grhs[0], &Gsense[0], &Grmatbeg[0], &Grmatind[0], Grmatval.data(), nullptr,
               Growname);
    std::vector<int> Hrmatbeg(NUM_ROWS);
    for (int i = 0; i < NUM_ROWS; ++i) {
        Hrmatbeg[i] = i * (NUM_ROWS - 1);
    }

    std::vector<int> Hrmatind(NUM_VARS);
    for (int i = 0; i < NUM_VARS; ++i) {
        Hrmatind[i] = i;
    }

    std::vector<double> Hrmatval(NUM_VARS, 1.0);
    CHECKED_CPX_CALL(CPXaddrows, env, prob, 0, NUM_ROWS, NUM_VARS, &Hrhs[0], &Hsense[0], &Hrmatbeg[0], &Hrmatind[0], Hrmatval.data(), nullptr,
               Hrowname);
    std::vector<int> Lrmatbeg(NUM_VARS);
    for (int i = 0; i < NUM_VARS; ++i) {
        Lrmatbeg[i] = i * 2;
    }

    std::vector<int> Lrmatind(2 * NUM_VARS);
    for (int i = 0; i < NUM_VARS; ++i) {
        Lrmatind[2 * i] = i;
        Lrmatind[2 * i + 1] = NUM_VARS + i;
    }

    std::vector<double> Lrmatval(2 * NUM_VARS);
    for (int i = 0; i < NUM_VARS; ++i) {
        Lrmatval[2 * i] = -NUM_ROWS + 1;
        Lrmatval[2 * i + 1] = 1;
    }

    std::vector<char> Lsense(NUM_VARS, 'L');
    std::vector<double> Lrhs(NUM_VARS, 0.0);

    CHECKED_CPX_CALL(CPXaddrows, env, prob, 0, NUM_VARS, 2 * NUM_VARS, &Lrhs[0], &Lsense[0], &Lrmatbeg[0], &Lrmatind[0], &Lrmatval[0],
               nullptr, nullptr);


    CHECKED_CPX_CALL(CPXmipopt, env, prob);

    double objval;

    // status = CPXwriteprob(env, prob, "lpex3.lp", "lp");
    // if (status) {
    //     fprintf(stderr, "CPXwriteprob failed.\n");
    //     return 1206;
    // }
    //
    // std::ifstream file("./lpex3.lp");
    //
    // if (!file.is_open()) {
    //     std::cerr << "Error: Could not open the file." << std::endl;
    //     return 1;
    // }
    //
    // std::string line;
    // while (std::getline(file, line)) {
    //     std::cout << line << std::endl;
    // }
    //
    // file.close();

// Get the solution status
    if (CPXgetstat(env, prob) == CPXMIP_OPTIMAL) {
        std::cout << "An optimal integer solution has been found." << std::endl;

        CHECKED_CPX_CALL(CPXgetobjval, env, prob, &objval);
        std::cout << "After network optimization, objective is " << std::fixed << objval << std::endl;



        std::vector<double> yval(NUM_VARS);
        std::vector<double> xval(NUM_VARS);

        int cur_numrows = CPXgetnumrows(env, prob);


        CHECKED_CPX_CALL(CPXgetx, env, prob, &yval[0], 0, NUM_VARS - 1);
        CHECKED_CPX_CALL(CPXgetx, env, prob, &xval[0], NUM_VARS, 2 * NUM_VARS - 1);


        std::vector<double> slack;
        slack.reserve(NUM_VARS);
        CHECKED_CPX_CALL(CPXgetslack, env, prob, &slack[0], 0, cur_numrows - 1);


        for (int i = 0; i < cur_numrows; ++i) {
            printf("Row %d:  Slack = %10f\n", i, slack[i]);
        }
    } else {
        std::cout << "An optimal integer solution has not been found." << std::endl;
    }
    for (int i = 0; i < N_ROWS_F; ++i) {
        delete [] Frowname[i];
    }
    delete[] Frowname;

    for (int i = 0; i < NUM_ROWS; ++i) {
        delete[] Growname[i];
        delete[] Hrowname[i];
    }
    delete [] Growname;
    delete [] Hrowname;

    for (int i = 0; i < 2*NUM_VARS; ++i) {
        delete[] ycolname[i];
    }
    delete [] ycolname;
    delete [] ctype;

    if (prob != nullptr) {
        status = CPXfreeprob(env, &prob);
        if (status) {
            fprintf(stderr, "CPXfreeprob failed, error code %d.\n", status);
        }
    }

    if (env != nullptr) {
        status = CPXcloseCPLEX(&env);
        if (status) {
            fprintf(stderr, "Could not close CPLEX environment.\n");
            CPXgeterrorstring(env, status, errmsg);
            fprintf(stderr, "%s", errmsg);
        }
    }

    return 0;
}
