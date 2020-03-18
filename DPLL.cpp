
// DPLL algorithm.
//

#include "DPLL.h"
#include "common.h"

bool DPLL::check_sat()
{
	// bool value to record whether the literal is decided or propagated;
	// essential for jumping back;
	std::vector<std::pair<int, bool>> decision_list;

	// initial v_parent
	// for conevenience, initial (phi.num_variable+1) vectors
	for (int i = 0; i <= this->phi.num_variable; i++)
	{
		this->v_parent.push_back(std::vector<int>());
	}

	// if a clause has only one element, this element has a definite interpretation.
	for (const auto &elem : this->phi.clauses)
	{
		if (int(elem.size()) == 1)
		{
			// conflict happens
			model::const_iterator got = this->interpretation.find(VAR(elem[0]));
			if (got != this->interpretation.end())
			{
				if (POSITIVE(elem[0]) != got->second)
				{
					return false;
				}
			}
			// this variable has not been decided
			this->interpretation.insert({{VAR(elem[0]), POSITIVE(elem[0])}});
		}
	}

	while (1)
	{
		// // find if unit propagation exists and propagate the unit one
		// this->unit_propagate();

		// decide a new variable
		for (int i = 1; i <= this->phi.num_variable; i++)
		{
			model::const_iterator got = this->interpretation.find(i);
			if (got == this->interpretation.end())
			{
				//the variable has not been decided
				decision_list.push_back(std::make_pair(i, true));
				this->interpretation.insert(std::make_pair(i, true));
				break;
			}
			else
			{
				continue;
			}
		}

		// find if unit propagation exists and propagate the unit one
		int conflict_v = this->unit_propagate(decision_list);
		while (conflict_v != 0)
		{
			if (!this->go_backjump(decision_list,conflict_v))
			{
				return false;
			}
			// backjump just changes the decided ones
			// so we need to propagate the decided ones again
			conflict_v = this->unit_propagate(decision_list);
		}
		if (int(this->interpretation.size()) == this->phi.num_variable)
		{
			return true;
		}
	}

	return false;
}

int DPLL::unit_propagate(std::vector<std::pair<int, bool>> &decision_list)
{
	int unknown_literal;
	literal to_unit;
	bool clause_ans;
	bool if_loop = true;
	model::const_iterator got;
	while (if_loop)
	{
		if_loop = false;
		for (const auto &clause_ : this->phi.clauses)
		{
			clause_ans = false;
			to_unit = 0;
			unknown_literal = 0;

			for (const auto &literal_ : clause_)
			{
				got = this->interpretation.find(abs(literal_));
				if (got == this->interpretation.end())
				{
					if (unknown_literal == 0)
					{
						// not found
						unknown_literal += 1;
						to_unit = literal_;
					}
					else
					{
						// multiple unknown literals do not have unit propagation;
						unknown_literal += 1;
						break;
					}
				}
				else
				{
					// if found, calculate current satisfiability;
					clause_ans = clause_ans || (POSITIVE(literal_) == got->second);
					// this clause has already been satisfied
					if (clause_ans)
					{
						break;
					}
				}
			}
			if (unknown_literal == 1 && !clause_ans)
			{
				this->interpretation.insert(std::pair<int, bool>(VAR(to_unit), POSITIVE(to_unit)));
				for (const auto &literal_ : clause_)
				{
					if (VAR(literal_) != VAR(to_unit))
					{
						this->v_parent[VAR(to_unit)].push_back(VAR(literal_));
					}
				}
				decision_list.push_back(std::make_pair(abs(to_unit), false));
				if_loop = true;
			}
			if (unknown_literal == 0 && !clause_ans)
			{
				// conflict happens, directly go to next step: go_backjump()
				int max_dis = 0;
				for (const auto &literal_ : clause_)
				{
					// to find the conflict node [to_unit]
					// the conflict node should be the latest propagated one
					auto ele = std::find(decision_list.begin(), decision_list.end(), std::make_pair(VAR(literal_), false));
					if (ele == decision_list.end())
					{
						continue;
					}
					int nPosition = distance(decision_list.begin(), ele);
					if (nPosition > max_dis)
					{
						max_dis = nPosition;
						to_unit = VAR(literal_);
					}
				}
				for (const auto &literal_ : clause_)
				{
					if (VAR(literal_) != VAR(to_unit))
					{
						// also record confilct nodes for convenience
						this->v_parent[VAR(to_unit)].push_back(VAR(literal_));
					}
				}
				return VAR(to_unit);
			}
		}
	}
	return 0;
}

int DPLL::go_backtrace(std::vector<std::pair<int, bool>> &decision_list)
{
	bool is_decided;
	int pop_one;
	while (decision_list.size())
	{
		is_decided = decision_list.back().second;
		pop_one = decision_list.back().first;
		if (!is_decided)
		{
			decision_list.pop_back();
			this->v_parent[pop_one].clear();
			this->interpretation.erase(pop_one);
		}
		else
		{
			if (this->interpretation.find(pop_one)->second)
			{
				this->interpretation.find(pop_one)->second = false;
				return pop_one;
			}
			else
			{
				// go backtrace one more again
				decision_list.pop_back();
				this->v_parent[pop_one].clear();
				this->interpretation.erase(pop_one);
			}
		}
	}
	return 0;
}

bool DPLL::go_backjump(std::vector<std::pair<int, bool>> &decision_list, int conflict_v)
{
	int index = 0;
	int check_v;
	// in case duplicate
	std::set<int> parent_s;
	while (index < int(this->v_parent[conflict_v].size()))
	{
		check_v = this->v_parent[conflict_v][index];
		if (this->v_parent[check_v].size())
		{
			// means this variable is propagated
			// add its parents to end of (v_parent[conflict_v])
			for (auto elem : this->v_parent[check_v])
			{
				this->v_parent[conflict_v].push_back(elem);
			}
		}
		else
		{
			// if not, have found a toppest parent and we add it to to result list
			// Be careful. literal should be in its negative form.
			if (this->interpretation.find(check_v)->second)
			{
				parent_s.insert((-1) * check_v);
			}
			else
			{
				parent_s.insert(check_v);
			}
		}
		index++;
	}
	// clear parent info of conflict node
	this->v_parent[conflict_v].clear();

	// add a new clause to formula
	std::vector<int> parent;
	parent.assign(parent_s.begin(), parent_s.end());
	this->phi.clauses.push_back(parent);


	// the same as backtrace
	if (int(parent.size()) == 1)
	{
		return this->go_backtrace(decision_list);
	}

	int have_found = 0;
	bool is_decided;
	literal pop_one;
	bool flag = false;

	while (decision_list.size())
	{
		is_decided = decision_list.back().second;
		pop_one = decision_list.back().first;
		if (!is_decided)
		{
			decision_list.pop_back();
			this->v_parent[pop_one].clear();
			this->interpretation.erase(pop_one);
		}
		else
		{
			flag = false;
			for (auto elem : parent)
			{
				if (VAR(elem) == pop_one)
				{
					have_found += 1;
					flag = true;
					break;
				}
			}
			if (!flag || have_found < 2)
			{
				decision_list.pop_back();
				this->v_parent[pop_one].clear();
				this->interpretation.erase(pop_one);
			}
			else
			{
				// delete until the last second decided variable
				return true;
			}
		}
	}
	return false;
}

model DPLL::get_model()
{
	return this->interpretation;
}
