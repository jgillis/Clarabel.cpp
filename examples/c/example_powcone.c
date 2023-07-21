#include "clarabel.h"
#include "utils.h"

// Power Cone Example
//
//  solve the following power cone problem
//  max  x1^0.6 y^0.4 + x2^0.1
//  s.t. x1, y, x2 >= 0
//       x1 + 2y  + 3x2 == 3
//  which is equivalent to
//  max z1 + z2
//  s.t. (x1, y, z1) in K_pow(0.6)
//       (x2, 1, z2) in K_pow(0.1)
//       x1 + 2y + 3x2 == 3

int main(void)
{
    // 6 x 6 zero matrix
    ClarabelCscMatrix_f64 *P = clarabel_CscMatrix_f64_new(
        6,
        6,
        (uintptr_t[]){0, 0, 0, 0, 0, 0, 0},
        NULL,
        NULL
    );

    double q[6] = {0., 0., -1., 0., 0., -1.};

    /* From dense matrix:
    * [[-1., 0., 0., 0., 0., 0.],
    *  [0., -1., 0., 0., 0., 0.],
    *  [0., 0., -1., 0., 0., 0.],
    *  [0., 0., 0., -1., 0., 0.],
    *  [0., 0., 0., 0., -1., 0.],
    *  [0., 0., 0., 0., 0., -1.],
    *  [1., 2., 0., 3., 0., 0.],
    *  [0., 0., 0., 0., 1., 0.]]
    */
    ClarabelCscMatrix_f64 *A = clarabel_CscMatrix_f64_new(
        8,
        6,
        (uintptr_t[]){0, 2, 4, 5, 7, 9, 10},
        (uintptr_t[]){0, 6, 1, 6, 2, 3, 6, 4, 7, 5},
        (double[]){-1.0, 1.0, -1.0, 2.0, -1.0, -1.0, 3.0, -1.0, 1.0, -1.0}
    );

    double b[8] = {0., 0., 0., 0., 0., 0., 3., 1.};

    ClarabelSupportedConeT_f64 cones[4] =
    {
        ClarabelPowerConeT_f64(0.6),
        ClarabelPowerConeT_f64(0.1),
        ClarabelZeroConeT_f64(1),
        ClarabelZeroConeT_f64(1)
    };

    // Settings
    ClarabelDefaultSettings_f64 settings = clarabel_DefaultSettingsBuilder_f64_default();
    settings.verbose = true;
    settings.max_iter = 100;

    // Build solver
    ClarabelDefaultSolver_f64 *solver = clarabel_DefaultSolver_f64_new(
        P, // P
        q, // q
        A, // A
        b, // b
        4, // n_cones
        cones,
        &settings
    );

    // Solve
    clarabel_DefaultSolver_f64_solve(solver);

    // Get solution
    ClarabelDefaultSolution_f64 solution = clarabel_DefaultSolver_f64_solution(solver);
    print_solution_f64(&solution);

    // Free the matrices and the solver
    clarabel_DefaultSolver_f64_free(solver);
    clarabel_CscMatrix_f64_free(P);
    clarabel_CscMatrix_f64_free(A);

    return 0;
}