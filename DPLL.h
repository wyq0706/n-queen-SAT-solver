//
// DPLL algorithm.
//

#ifndef DPLL_DPLL_H
#define DPLL_DPLL_H

#include "common.h"
#include <set>
#include <algorithm>

class DPLL
{
public:
    /**
	 * Constructor.
	 *
	 * @param phi the formula to be checked
	 * @note Please DON'T CHANGE this signature because the grading script will directly call this function!
	 */
    DPLL(const formula &phi) : phi(phi)
    {
    }

    /**
	 * Check if the formula is satisfiable.
	 *
	 * @return true if satisfiable, and false if unsatisfiable
	 * @note Please DON'T CHANGE this signature because the grading script will directly call this function!
	 */
    bool check_sat();

    /**
	 * Get a satisfying model (interpretation) of the formula, the model must be *complete*, that is,
	 * it must assign every variable a truth value.
	 * This function will be called if and only if `check_sat()` returns true.
	 *
	 * @return an arbitrary (if there exist many) satisfying model
	 * @note Please DON'T CHANGE this signature because the grading script will directly call this function!
	 */
    model get_model();

private:
    formula phi;
    model interpretation;

    /**
     * maps every varible to where it is derived from
     * essential for backjumping
     */
    std::vector<std::vector<int>> v_parent;

    /**
	 * Tests if unit propagation could be applied:
	 * If unit propagation exists, give corresponding interpretation to atoms;
	 * and check for unit propagation again...
	 *
	 * Using parameters that are stored in the instance: formula and interpreation (I,F)
	 *
	 * return the conflict node
     * if no conflict node, return 0
	 */
    int unit_propagate(std::vector<std::pair<int, bool>> &);

    /**
     * backtrack for once
     */
    int go_backtrace(std::vector<std::pair<int, bool>> &);

    /**
     * find conflict clause
     * add conflict clause to phi
     * backjump based on past experience
     */
    bool go_backjump(std::vector<std::pair<int, bool>> &, int son);
};

#endif //DPLL_DPLL_H
