/*
 * IPFilterSolver.hpp
 *
 *  Created on: 4 Apr 2013
 *      Author: allan
 */

#pragma once

// C++ includes
#include <set>

// Eigen includes
#include <Eigen/Dense>
using namespace Eigen;

// Optima includes
#include <IPFilter/IPFilterErrors.hpp>
#include <IPFilter/IPFilterOptions.hpp>
#include <IPFilter/IPFilterParams.hpp>
#include <IPFilter/IPFilterResult.hpp>
#include <IPFilter/IPFilterState.hpp>
#include <Misc/QualitySolver.hpp>
#include <Utils/Filter.hpp>
#include <Utils/OptimumProblem.hpp>
#include <Utils/Outputter.hpp>
#include <Utils/Scaling.hpp>

namespace Optima {

/**
 * The primal-dual interior-point optimisation solver based on the ipfilter algorithm
 */
class IPFilterSolver
{
public:
    /**
     * Constructs a default @ref IPFilterSolver instance
     */
    IPFilterSolver();

    /**
     * Sets the options for the optimisation calculation
     */
    void SetOptions(const IPFilterOptions& options);

    /**
     * Sets the parameters of the optimisation algorithm
     */
    void SetParams(const IPFilterParams& params);

    /**
     * Sets the definition of the optimisation problem
     */
    void SetProblem(const OptimumProblem& problem);

    /**
     * Sets the scaling factors for the optimisation problem
     * @param scaling
     */
    void SetScaling(const Scaling& scaling);

    /**
     * Gets the calculation options of the optimisation solver
     */
    const IPFilterOptions& GetOptions() const;

    /**
     * Gets the algorithm params of the optimisation solver
     */
    const IPFilterParams& GetParams() const;

    /**
     * Gets the solution result of the last optimisation calculation
     */
    const IPFilterResult& GetResult() const;

    /**
     * Gets the solution state of the last optimisation calculation
     */
    const IPFilterState& GetState() const;

    /**
     * Gets the optimisation problem of the optimisation solver
     */
    const OptimumProblem& GetProblem() const;

    /**
     * Solves the optimisation problem
     *
     * This method allows the user to provide only the initial guess for the primal variables @b x.
     *
     * The initial guess of the Lagrange multipliers @b y and @b z are given by the options
     * @ref IPFilterOptions::InitialGuessOptions::y and @ref IPFilterOptions::InitialGuessOptions::z in
     * IPFilterOptions::initialguess.
     *
     * @param[in,out] x The initial guess of the primal variables as input. The optimum solution at the end of
     *     the calculation as output.
     */
    void Solve(VectorXd& x);

    /**
     * Solves the optimisation problem
     *
     * This method allows the user to provide the initial guess for the primal variables @a x as well as the Lagrange
     * multipliers @a y and @a z.
     *
     * This is usefull for sequential calculations where the i-th calculation uses the result of the (i-1)-th
     * calculation as initial guess. Therefore, convergence to an optimal point might result in less iterations.
     *
     * Note, however, that some components of @c x and @c z might be modified in order to improve robustness and
     * efficiency. The modification is given by:
     *
     *  - <tt> x = max(x, xguessmin) </tt> (see @ref IPFilterOptions::InitialGuessOptions::xmin),
     *  - <tt> z = max(z, zguessmin) </tt> (see @ref IPFilterOptions::InitialGuessOptions::zmin).
     *
     * We assume in this method that a good initial guess is provided. Therefore, we scale the primal variables @b x
     * using the initial guess @c x.
     *
     * This method uses a restart scheme in case of failure. If the provided initial guesses @c x, @c y, and @c z
     * results in any trust-region search error, the restart scheme is activated. It consists of resetting the
     * Lagrange multipliers @c z as:
     *
     *  - <tt> z = min(zguess, pow(factor, attempt)*mu) </tt>,
     *
     *  where @c zguess is given in @ref IPFilterOptions::InitialGuessOptions::z, @c factor is set in
     *  @ref IPFilterParams::Restart::factor, @c attempt is the number of restart attempts so far, and @c mu is
     *  the @f$\mu@f$ parameter where the calculation stopped.
     *
     *  Then, we start restart the interior-point calculation using the reset @c z. The values for @c x and @c y
     *  are those from where the calculation stopped.
     *
     * @param[in,out] x The initial guess of the primal variables @b x as input. The optimum solution at the end of
     *     the calculation as output.
     * @param[in,out] y The initial guess of the Lagrange multipliers @b y. The optimum solution at the end of the
     *     calculation as output.
     * @param[in,out] z The initial guess of the Lagrange multipliers @b z. The optimum solution at the end of the
     *     calculation as output.
     */
    void Solve(VectorXd& x, VectorXd& y, VectorXd& z);

private:
    /// The definition of the optimisation problem
    OptimumProblem problem;

    /// The parameters used for the optimisation calculation
    IPFilterParams params;

    /// The options used for the optimisation calculation
    IPFilterOptions options;

    /// The result details of the last optimisation calculation
    IPFilterResult result;

    /// The scaling factors for the optimisation problem
    Scaling scaling;

    /// The output instance for printing the calculation progress
    Outputter outputter;

    /// The dimension of the objective and constraint functions respectively
    unsigned dimx, dimy;

    /// The current and next states respectively
    IPFilterState curr, next;

    /// The x-component of the normal and tangencial steps respectively
    VectorXd snx, stx;

    /// The y-component of the normal and tangencial steps respectively
    VectorXd sny, sty;

    /// The z-component of the normal and tangencial steps respectively
    VectorXd snz, stz;

    /// The norms of the normal and tangencial steps respectively
    double norm_sn, norm_st;

    /// The normal and tangencial step-lengths respectively
    double alphan, alphat;

    /// The filter used during the search for a suitable trust-region radius
    Filter filter;

    /// The current radius of the trust-region
    double delta;

    /// The initial value of the trust-region radius used for the trust-region search
    double delta_initial;

    /// The parameter c used for the calculation of the psi measure
    double c;

    /// The parameter gamma used in the neighborhood condition checking
    double gamma;

    /// The parameter M used in the neighborhood condition checking
    double M;

    /// The flag that indicates if the algorithm is currently in the restoration mode
    bool restoration;

    /// The LU decomposition of the reduced KKT matrix
    PartialPivLU<MatrixXd> lu;

    /// The reduced KKT matrix
    MatrixXd lhs;

    /// The right-hand side vector of the linear system and the auxiliary linear system solution
    VectorXd rhs, u;

    /// The gradient of the Lagrange function with respect to x at the current state
    VectorXd Lx;

    /// The Hessian of the Lagrange function with respect to x at the current state
    MatrixXd Lxx;

    QualitySolver quality;

private:
    bool AnyFloatingPointException(const IPFilterState& state) const;
    bool PassConvergenceCondition() const;
    bool PassFilterCondition() const;
    bool PassRestorationCondition(double delta) const;
    bool PassSafeStepCondition() const;

    double CalculateDeltaPositiveXZ() const;
    double CalculateDeltaXzGreaterGammaMu() const;
    double CalculateLargestBoundaryStep(const VectorXd& p, const VectorXd& dp) const;
    double CalculateLargestQuadraticStep(const VectorXd& a, const VectorXd& b, const VectorXd& c, const VectorXd& d) const;
    double CalculateNextLinearModel() const;
    double CalculatePsi(const IPFilterState& state) const;
    double CalculateSigma() const;
    double CalculateSigmaDefault() const;
    double CalculateSigmaLOQO() const;
    double CalculateSigmaQuality();
    double CalculateSigmaSafeStep() const;

    void AcceptTrialPoint();
    void ExtendFilter();
    void Initialise(const VectorXd& x, const VectorXd& y, const VectorXd& z);
    void OutputHeader();
    void OutputState();
    void ResetLagrangeMultipliersZ(IPFilterState& state) const;
    void SearchDeltaNeighborhood();
    void SearchDeltaTrustRegion();
    void SearchDeltaTrustRegionRestoration();
    void Solve();
    void SolveRestoration();
    void UpdateActiveMonitor();
    void UpdateNeighborhoodParameterM();
    void UpdateNextState(double delta);
    void UpdateNormalTangentialSteps();
    void UpdateNormalTangentialStepsRestoration();
    void UpdateSafeTangentialStep();
    void UpdateState(const VectorXd& x, const VectorXd& y, const VectorXd& z, IPFilterState& state);
};

} /* namespace Optima */
