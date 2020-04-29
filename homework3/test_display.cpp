#include "term.hpp"
#include <vector>
#include <unordered_map>
#include <iostream>
#include <memory>
using namespace std;

//////////////////////////////////////////////////////////////////
//
// This is mostly to show off what a term rewritting system can do
// I've made a bunch of common rules for boolean algebra,
// and we are going to take a moderately large expression, and reduce it.
//
// The example problem was taken form a 251 exam,
// so you know it's hard!
//
//////////////////////////////////////////////////////////////////

template<typename T>
using term_ptr = shared_ptr<term<T>>;
template<typename T>
using variable_ptr = shared_ptr<variable<T>>;
template<typename T>
using literal_ptr = shared_ptr<literal<T>>;
template<typename T>
using function_ptr = shared_ptr<function<T>>;

// a rule is a pair of terms
template<typename T>
using rule = pair<term_ptr<T>, term_ptr<T>>;

/////////////////////////////////
// Boolean algebra
/////////////////////////////////

// helper functions for making variables, literals and funcitons

variable_ptr<bool> var(string n)
{
    return make_shared<variable<bool>>(variable<bool>(n));
}
literal_ptr<bool> lit(bool b)
{
    return make_shared<literal<bool>>(literal<bool>(b));
}
function_ptr<bool> fun(string n, vector<term_ptr<bool>> k)
{
    return make_shared<function<bool>>(function<bool>(n, k.size(), k));
}


//functions
function_ptr<bool> tand(term_ptr<bool> x, term_ptr<bool> y)
{
    return fun("and", {x,y});
}
function_ptr<bool> tor(term_ptr<bool> x, term_ptr<bool> y)
{
    return fun("or", {x,y});
}
function_ptr<bool> tnot(term_ptr<bool> x)
{
    return fun("not",{x});
}
function_ptr<bool> to(term_ptr<bool> x, term_ptr<bool> y)
{
    return fun("->",{x,y});
}


// the rule ->(a, false) => !(a)
rule<bool> contra(to(var("a"), lit(false)), tnot(var("a")));



/////////////////////////////////
// substitution
/////////////////////////////////


int main()
{
    // (!(a \/ a) /\ !true) -> false
    term_ptr<bool> example = to(tand(tnot(tor(var("x"), var("x"))), tnot(lit(true))), lit(false));

    cout << *example << endl;
}
