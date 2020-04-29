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

variable_ptr<int> var(string n)
{
    return make_shared<variable<int>>(variable<int>(n));
}
literal_ptr<int> lit(int b)
{
    return make_shared<literal<int>>(literal<int>(b));
}
function_ptr<int> fun(string n, vector<term_ptr<int>> k)
{
    return make_shared<function<int>>(function<int>(n, k.size(), k));
}


//functions
function_ptr<int> tplus(term_ptr<int> x, term_ptr<int> y)
{
    return fun("+", {x,y});
}
function_ptr<int> ttimes(term_ptr<int> x, term_ptr<int> y)
{
    return fun("*", {x,y});
}
function_ptr<int> tpow(term_ptr<int> x, term_ptr<int> y)
{
    return fun("^",{x,y});
}



/////////////////////////////////
// substitution
/////////////////////////////////


int main()
{

    // We are going to try to get my rewrite system to come up with the following derivation
    // (3*x + x*3*x) * (2 * x*1)
    term_ptr<int> example = ttimes(tplus(ttimes(lit(3), var("x")), ttimes(var("x"),ttimes(lit(3),var("x")))),
                                   tplus(lit(2), ttimes(var("x"), lit(1))));

    vector<rule<int>> rules {
        // distributivity
        rule<int>(ttimes(tplus(var("a"), var("b")), var("c")),
                  tplus(ttimes(var("a"), var("c")), ttimes(var("b"), var("c")))),
        rule<int>(ttimes(var("c"), tplus(var("a"), var("b"))),
                  tplus(ttimes(var("c"), var("a")), ttimes(var("c"), var("b")))),
        //* associativity
        rule<int>(ttimes(var("a"), ttimes(var("b"),var("c"))), ttimes(ttimes(var("a"),var("b")), var("c"))),
        rule<int>(tplus(var("a"), tplus(var("b"),var("c"))), tplus(tplus(var("a"),var("b")), var("c"))),

        //* zero
        rule<int>(ttimes(var("a"), lit(0)), lit(0)),
        rule<int>(ttimes(lit(0), var("a")), lit(0)),

        //* identity
        rule<int>(ttimes(var("a"), lit(1)), var("a")),
        rule<int>(ttimes(lit(1), var("a")), var("a")),

        //* constants
        rule<int>(ttimes(var("a"), var("a")), tpow(var("a"),lit(2))),
        rule<int>(ttimes(ttimes(var("a"), lit(2)), var("a")), ttimes(lit(2), tpow(var("a"),lit(2)))),
        rule<int>(ttimes(ttimes(var("a"), lit(3)), var("a")), ttimes(lit(3), tpow(var("a"),lit(2)))),
        rule<int>(ttimes(ttimes(lit(2), var("a")), var("a")), ttimes(lit(2), tpow(var("a"),lit(2)))),
        rule<int>(ttimes(ttimes(lit(3), var("a")), var("a")), ttimes(lit(3), tpow(var("a"),lit(2)))),
        //rule<int>(ttimes(var("a"), lit(3)), ttimes(lit(3), var("a"))),

        //+ identity
        rule<int>(tplus(var("a"), lit(0)), var("a")),
        rule<int>(tplus(lit(0), var("a")), var("a"))

    };

    while(example)
    {
        cout << "=> " << *example << endl;
        example = reduce(example, rules);
    }
    return 0;
}
