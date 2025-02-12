#include <iostream>
#include <cstring>
#include <vector>
#include <string>

#include <algorithm>
#include <cassert>
#include <fstream>

/* Brings in the CPLEX function declarations */
#include <ilcplex/cplex.h>
#include "cpxmacro.h"

#include "ArgParser.h"
#include "graph/Graph.h"
#include "graph/Path.h"
#include "TSPGraphReader.h"


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

void checkSolutionGap(CPXENVptr env, CPXLPptr lp) {
    double objval, best_bound, mip_gap;
    int status;

    // get the objective value of the best found solution
    status = CPXgetobjval(env, lp, &objval);
    if (status) {
        std::cerr << "Error retrieving objective value." << std::endl;
        return;
    }

    // get the best known bound
    status = CPXgetbestobjval(env, lp, &best_bound);
    if (status) {
        std::cerr << "Error retrieving best bound." << std::endl;
        return;
    }

    // compute the relative MIP gap
    mip_gap = (objval != 0.0) ? (std::fabs(best_bound - objval) / std::fabs(objval)) * 100.0 : 0.0;


    std::cout << "\nBest Found Solution: " << objval << std::endl;
    std::cout << "Best Known Bound: " << best_bound << std::endl;
    std::cout << "MIP Gap: " << mip_gap << "%" << std::endl;
}


void add_blockrows_F(int &status, char errmsg[4096], Env env, Prob prob, int num_vertex, int num_vars, char **&Frowname) {
    const int N_ROWS_F = num_vertex - 1;
    const int N_VARS_PER_LINE_F = 2 * N_ROWS_F - 1;
    const int FNZCNT = N_VARS_PER_LINE_F * N_ROWS_F;

    std::vector<double> Frhs(N_ROWS_F, 1.0);
    std::vector<double> Frmatval(FNZCNT);
    std::vector<int> Frmatbeg(num_vertex - 1);
    for (int i = 0; i < num_vertex - 1; ++i) {
        Frmatbeg[i] = i * N_VARS_PER_LINE_F;
    }

    std::vector<int> Frmatind(FNZCNT);
    for (int i = 1, *pt = &Frmatind[0]; i < num_vertex; ++i) {
        *pt = num_vars + Graph::getSingleCoord(num_vertex, 0, i);
        int *ent_edges = pt + 1;
        int *exit_edges = pt + num_vertex - 1;
        for (int j = 1; j < num_vertex; ++j) {
            if (i != j) {
                *ent_edges++ = num_vars + Graph::getSingleCoord(num_vertex, j, i);
                *exit_edges++ = num_vars + Graph::getSingleCoord(num_vertex, i, j);
            }
        }
        pt += N_VARS_PER_LINE_F;
    }


    for (int i = 0; i < FNZCNT; ++i) {
        if (i % N_VARS_PER_LINE_F < num_vertex - 1) Frmatval[i] = 1.0;
        else Frmatval[i] = -1.0;
    }
    Frowname = new char *[num_vertex - 1];
    for (int k = 0; k < num_vertex - 1; ++k) {
        Frowname[k] = new char[32];
        snprintf(Frowname[k], 32, "Net flow of node %d", k + 1);
    }


    std::vector<char> Fsense(N_ROWS_F, 'E');


    CHECKED_CPX_CALL(CPXaddrows, env, prob, 0, N_ROWS_F, FNZCNT, &Frhs[0], &Fsense[0], &Frmatbeg[0], &Frmatind[0],
                     &Frmatval[0], nullptr, &Frowname[0]);
}

void add_blockrows_G(int &status, char errmsg[4096], Env env, Prob prob, int num_vertex, int num_vars, char **&Growname) {
    std::vector<double> Grhs(num_vertex, 1.0);
    std::vector<char> Gsense(num_vertex, 'E');

    Growname = new char *[num_vertex];
    for (int k = 0; k < num_vertex; ++k) {
        Growname[k] = new char[64];
        snprintf(Growname[k], 64, "Incoming arcs node %d", k);
    }

    std::vector<int> Grmatbeg(num_vertex);
    for (int i = 0; i < num_vertex; ++i) {
        Grmatbeg[i] = i * (num_vertex - 1);
    }

    std::vector<int> Grmatind(num_vars);
    std::vector<double> Grmatval(num_vars, 1.0);
    for (int i = 0, *G_p = &Grmatind[0]; i < num_vertex; ++i) {
        for (int j = 0; j < num_vertex; ++j) {
            if (i != j) {
                *G_p++ = Graph::getSingleCoord(num_vertex, j, i);
            }
        }
    }


    CHECKED_CPX_CALL(CPXaddrows, env, prob, 0, num_vertex, num_vars, &Grhs[0], &Gsense[0], &Grmatbeg[0], &Grmatind[0],
                     Grmatval.data(), nullptr, Growname);
}

void add_blockrows_H(int &status, char errmsg[4096], Env env, Prob prob, int num_vertex, int num_vars, char **&Hrowname) {
    std::vector<double> Hrhs(num_vertex, 1.0);
    std::vector<char> Hsense(num_vertex, 'E');

    Hrowname = new char *[num_vertex];
    for (int k = 0; k < num_vertex; ++k) {
        Hrowname[k] = new char[64];
        snprintf(Hrowname[k], 64, "Outcoming arcs node %d", k);
    }
    std::vector<int> Hrmatbeg(num_vertex);
    for (int i = 0; i < num_vertex; ++i) {
        Hrmatbeg[i] = i * (num_vertex - 1);
    }

    std::vector<int> Hrmatind(num_vars);
    for (int i = 0; i < num_vars; ++i) {
        Hrmatind[i] = i;
    }

    std::vector<double> Hrmatval(num_vars, 1.0);
    CHECKED_CPX_CALL(CPXaddrows, env, prob, 0, num_vertex, num_vars, &Hrhs[0], &Hsense[0], &Hrmatbeg[0], &Hrmatind[0],
                     Hrmatval.data(), nullptr,
                     Hrowname);
}

void add_blockrows_L(int &status, char errmsg[4096], Env env, Prob prob, int num_vertex, int num_vars) {
    std::vector<int> Lrmatbeg(num_vars);
    for (int i = 0; i < num_vars; ++i) {
        Lrmatbeg[i] = i * 2;
    }

    std::vector<int> Lrmatind(2 * num_vars);
    for (int i = 0; i < num_vars; ++i) {
        Lrmatind[2 * i] = i;
        Lrmatind[2 * i + 1] = num_vars + i;
    }

    std::vector<double> Lrmatval(2 * num_vars);
    for (int i = 0; i < num_vars; ++i) {
        Lrmatval[2 * i] = -num_vertex + 1;
        Lrmatval[2 * i + 1] = 1;
    }

    std::vector<char> Lsense(num_vars, 'L');
    std::vector<double> Lrhs(num_vars, 0.0);

    CHECKED_CPX_CALL(CPXaddrows, env, prob, 0, num_vars, 2 * num_vars, &Lrhs[0], &Lsense[0], &Lrmatbeg[0],
                     &Lrmatind[0], &Lrmatval[0],
                     nullptr, nullptr);
}

int main(int argc, char *argv[]) {
    CommandLineOptions cli{parse_arguments(argc, argv)};
    try {
        const TSPGraphReader tspReader{};
        Graph g{tspReader.read(cli.filename)};
        std::vector<std::vector<double>> distance_matrix{g};

        int status;
        char errmsg[BUF_SIZE];
        DECL_ENV(env);
        DECL_PROB(env, prob);
        if (cli.timeout_ms > 0.0) {
            CPXsetintparam(env, CPX_PARAM_TILIM, cli.timeout_ms);
        }
        // CPXsetdblparam(env, CPXPARAM_MIP_Tolerances_MIPGap, 1e-6);
        int num_vertex = distance_matrix.size();
        int num_vars = num_vertex * num_vertex - num_vertex;

        char **ycolname = new char *[2 * num_vars];
        char **xcolname = ycolname + num_vars;
        fill_colname(num_vertex, ycolname, xcolname);


        char *ctype = new char[2 * num_vars];
        for (int i = 0; i < num_vars; ++i) {
            ctype[i] = 'B';
            ctype[i + num_vars] = 'I';
        }

        std::vector<double> objCost;
        objCost.reserve(2 * num_vars);
        for (int i = 0; i < num_vertex; ++i) {
            for (int j = 0; j < num_vertex; ++j) {
                if (i != j) {
                    objCost.push_back(distance_matrix[i][j]);
                }
            }
        }
        for (int i = 0; i < num_vars; ++i) {
            objCost.push_back(0.0);
        }

        std::vector<double> lb(2 * num_vars, 0.0);
        std::vector<double> ub(num_vars, 1.0);
        ub.reserve(2 * num_vars);
        for (int k = 0; k < num_vars; ++k) {
            ub.push_back(CPX_INFBOUND);
        }
        CHECKED_CPX_CALL(CPXnewcols, env, prob, 2 * num_vars, &objCost[0], &lb[0], &ub[0], &ctype[0], &ycolname[0]);
        CHECKED_CPX_CALL(CPXchgobjsen, env, prob, CPX_MIN);

        char **Frowname, **Growname, **Hrowname;
        add_blockrows_F(status, errmsg, env, prob, num_vertex, num_vars, Frowname);
        add_blockrows_G(status, errmsg, env, prob, num_vertex, num_vars, Growname);
        add_blockrows_H(status, errmsg, env, prob, num_vertex, num_vars, Hrowname);
        add_blockrows_L(status, errmsg, env, prob, num_vertex, num_vars);

        CHECKED_CPX_CALL(CPXmipopt, env, prob);

        double objval;
        std::string filename = "assignment1_" + cli.getFilenameExtension()+ ".lp";
        status = CPXwriteprob(env, prob, filename.c_str(), "lp");
        if (status) {
            fprintf(stderr, "CPXwriteprob failed.\n");
            return 1206;
        }
        std::ifstream file(filename);


        if (CPXgetstat(env, prob) == CPXMIP_OPTIMAL) {
            std::cout << "An optimal integer solution has been found." << std::endl;
        } else if (CPXgetstat(env, prob) == CPXMIP_OPTIMAL_TOL) {
            std::cout << "A quasi optimal  solution has been found." << std::endl;
        }

        if (CPXgetstat(env, prob) == CPXMIP_OPTIMAL || CPXgetstat(env, prob) == CPXMIP_OPTIMAL_TOL) {
            CHECKED_CPX_CALL(CPXgetobjval, env, prob, &objval);
            std::cout << "After network optimization, objective is " << std::fixed << objval << std::endl;


            std::vector<double> yval(num_vars);
            std::vector<double> xval(num_vars);



            CHECKED_CPX_CALL(CPXgetx, env, prob, &yval[0], 0, num_vars - 1);
            CHECKED_CPX_CALL(CPXgetx, env, prob, &xval[0], num_vars, 2 * num_vars - 1);

            std::vector<std::vector<int>> pairs;
            for (int i = 0; i < num_vars; ++i) {
                if (yval[i] > 1e-7) {
                    int a, b;
                    Graph::getDoubleCoord(num_vertex, i, a, b);
                    pairs.push_back({a,b});
                }
            }
            Path tour{pairs};
            std::cout <<"\ntour " << tour << std::endl;


            // int nonnully = 0;
            // std::vector<unsigned int> seen(num_vertex, 0);
            // const std::vector<unsigned int> xpected(num_vertex, 2);
            // for (int i = 0; i < num_vars; ++i)
            //     if (yval[i] > 1e-7) {
            //         int a, b;
            //         Graph::getDoubleCoord(num_vertex, i, a, b);
            //         std::cout << nonnully << ": " << std::string("[") + std::to_string(a) + "," + std::to_string(b) +
            //                 std::string("] ");
            //         ++seen[a];++seen[b];++nonnully;
            //     }
            // assert(seen == xpected);

            if (CPXgetstat(env, prob) == CPXMIP_OPTIMAL_TOL) {
                checkSolutionGap(env, prob);
            }
        }

        for (int i = 0; i < num_vertex - 1; ++i) {
            delete[] Frowname[i];
        }
        delete[] Frowname;

        for (int i = 0; i < num_vertex; ++i) {
            delete[] Growname[i];
            delete[] Hrowname[i];
        }
        delete[] Growname;
        delete[] Hrowname;

        for (int i = 0; i < 2 * num_vars; ++i) {
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
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
