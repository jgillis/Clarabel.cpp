#include "utils.h"

#include <Clarabel>
#include <Eigen/Eigen>
#include <vector>

using namespace clarabel;
using namespace std;
using namespace Eigen;

int main(void)
{
    /* From dense matrix:
     * [[6., 0.],
     *  [0., 4.]]
     */
    MatrixXd P_dense(2, 2);
    P_dense <<
        6., 0.,
        0., 4.;

    SparseMatrix<double> P = P_dense.sparseView();
    P.makeCompressed();

    Vector<double, 2> q = { -1., -4. };

    MatrixXd A_dense(5, 2);
    A_dense <<
         1., -2., // <-- LHS of equality constraint (lower bound)
         1.,  0., // <-- LHS of inequality constraint (upper bound)
         0.,  1., // <-- LHS of inequality constraint (upper bound)
        -1.,  0., // <-- LHS of inequality constraint (lower bound)
         0., -1.; // <-- LHS of inequality constraint (lower bound)

    SparseMatrix<double> A = A_dense.sparseView();
    A.makeCompressed();

    Vector<double, 5> b = { 0., 1., 1., 1., 1. };

    vector<SupportedConeT<double>> cones
    {
        ZeroConeT<double>(1),
        NonnegativeConeT<double>(4),
    };

    // Settings
    DefaultSettings<double> settings = DefaultSettingsBuilder<double>::default_settings().build();

    // Build solver
    clarabel::eigen::DefaultSolver<double> solver(P, q, A, b, cones, settings);

    // Solve
    solver.solve();

    // Get solution
    DefaultSolution<double> solution = solver.solution();
    utils::print_solution(solution);

    return 0;
}
