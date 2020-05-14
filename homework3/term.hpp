#ifndef TERM_HPP
#define TERM_HPP

#include<memory>
#include<vector>
#include<string>
#include<algorithm>
#include<iostream>
#include<exception>
#include<iterator>
#include<sstream>
#include "sub.hpp"
#include "term_iterator.hpp"

/////////////////////////////////////////////////////////////////
//
// forward declarations;
//
/////////////////////////////////////////////////////////////////

// This just makes life a lot more convenient
template<typename T>
using term_ptr = std::shared_ptr<term<T>>;

template<typename T>
using rule = std::pair<term_ptr<T>, term_ptr<T>>;

/////////////////////////////////////////////////////////////////
//
// class definitions
//
// The overall structure is pretty simple
// variable <: term
// literal  <: term
// function <: term
//
// Each term can have subterms in it.
//
// Needs to be STL compliant
// Need copy constructors and assignment operator
// Needs to overload output operator<< (to print things out)
// Needs to have a rewrite function (will take a term, a rule, a position, and a substitution and will rewrite that)
//
// Advanced:
// Need move constructor and assignment
// Need a unify function
// Need to put everything together and make a reduce function (once you have the iterator, unify and rewrite function, reduce is really simple)
/////////////////////////////////////////////////////////////////

template<typename T>
class term {

    // Make it STL-complaint:
    public:
        std::vector<term_ptr<T>> children;

        typedef T           value_type;
        typedef T*          pointer;
        typedef T&          reference;
        typedef size_t      size_type;
        typedef ptrdiff_t   different_type;
        typedef term_iterator<T> iterator;
        typedef term_iterator<const T> const_iterator;

        // Default constructor
        term() {
        }

        // Copy constructor
        term(const term& t) {
        };

        // Move constructor - take everything out and make it your own, but set the source's data to null/default
        term(term&& t) {
        };

        // Deconstructor
        ~term() {
        };

        term& operator=(const term& t);            // Standard = operator for copy assignment
        term& operator=(term&& t);                 // Move = operator for move assignment (usually want this for STL-style classes,
                                                   //                                      is usually more efficient)
                                                   // Also, may want to swap your internals with the thing you're moving from
                                                   //   to avoid memory leaks.
        iterator begin() {return term_iterator<T>(this, true);}
        iterator end()   {return term_iterator<T>(this, false);}

        virtual int num_children() {
            return 0;
        }

        virtual std::string get_string() const {
            return "";
        }

        virtual void print() {
        }

        // For proper comma/space output detection
        virtual bool is_function_or_literal() {
            return false;
        }

};

// These are of type term<T> because they contain sub-terms
template<typename T>
class variable : public term<T> {
    std::string name;     // The name of our variable

    public:
        // Default Constructor
        variable(std::string n) {name = n;} 

        // Copy constructor
        variable(const variable& t) {
           t.name   = name;
        };

        // Move constructor - take everything out and make it your own, but set the source's data to null/default
        variable(variable&& t) {
            name     = t.name;
            t.name   = "";
        };

        // Deconstructor
        ~variable() {
        };

        std::string get_string() const {
            return name;
        }

        void print() {
            std::cout << name;
        }

        bool is_function_or_literal() {
            return false;
        }
};

template<typename T>
class literal : public term<T> {
    T value;         // Literal value

    public:

        // Constructor
        literal(T v) {value = v;}

        // Copy constructor
        literal(literal& t) {
           t.value  = value;
        };

        // Move constructor - take everything out and make it your own, but set the source's data to null/default
        literal(literal&& t) {
            value  = t.value;
            t.value  = false;
        };

        // Deconstructor
        ~literal() {
        };

        std::string get_string() const {
            std::stringstream converter;

            converter << std::boolalpha << value;
            return converter.str();
        }

        void print() {
            std::cout << std::boolalpha << value;
        }

        bool is_function_or_literal() {
            return true;
        }
};

template<typename T>
class function : public term<T> {
    std::string symbol;

    public:

        // Constructor
        function(std::string s, int size, std::vector<term_ptr<T>> k) {
            symbol = s;
            for (int i = 0; i < size; i++) {
                this->children.push_back(k[i]);
            }
        } 

        // Copy constructor
        function(const function& t) {
           t.symbol   = symbol;
           t.children = this->children;
        };

        // Move constructor - take everything out and make it your own, but set the source's data to null/default
        function(function&& t) {
            symbol          = t.symbol;
            this->children  = t.children;
            t.symbol  = "";
            t.children.clear();
        };

        // Deconstructor
        ~function() {
        };

        int num_children() {
            return this->children.size();
        }

        std::string get_string() const {
            return symbol;
        }

        void print() {
            std::cout << symbol;
        }

        bool is_function_or_literal() {
            return true;
        }
};  


/////////////////////////////////////////////////////////////////
//
// unify
//
/////////////////////////////////////////////////////////////////

template<typename T>
bool unify(const term<T>& t1, const term<T>& t2, Sub<T>& sigma)
{
    return true;
}

/////////////////////////////////////////////////////////////////
//
// reduce
//
/////////////////////////////////////////////////////////////////

template<typename T>
term_ptr<T> reduce(term_ptr<T> t, const std::vector<rule<T>>& rules)
{
    return t;
}

/////////////////////////////////////////////////////////////////
//
// rewrite
//
/////////////////////////////////////////////////////////////////

template<typename T>
term_ptr<T> rewrite(term_ptr<T> t, term<T>& rhs, std::vector<int> path, const Sub<T>& sigma)
{
    return t;
}

/////////////////////////////////////////////////////////////////
//
// operator<<
//
/////////////////////////////////////////////////////////////////

template<typename T>
std::ostream& operator<<(std::ostream& out, term<T>& t)
{
    std::stack<node<T>> _stack;  
    std::vector<int>    _path;   // Track the path from the root term to our current term
    term<T>* _lhs;
    term<T>* _rhs;           // Our right-most leaf term - the last term
    node<T> current_node;

    _lhs = &t;
    current_node.value  = _lhs;
    current_node.branch = 0;

    // Make sure we print our value even if we're the root node and have no children
    if ((current_node.value == _lhs) && current_node.value->num_children() == 0) {
        out << current_node.value->get_string();
    }

    //expected output: ->(and(not(or(x,x)), not(true)), false)
    // Iterate through until we make our way back to the root's rightmost child
    while (!(current_node.value == _lhs && (current_node.branch == current_node.value->num_children()))) {

        // If we haven't iterated over all the children of this node, pick the next one and move down
        if (current_node.branch < current_node.value->num_children()) {  // Pick a branch to go down
            term<T>* next_term;

            _stack.push(current_node);  // Push our current node on the stack
            _path.push_back(current_node.branch);
            next_term = current_node.value->children[current_node.branch].get();    // Get the raw pointer from the child's shared pointer

            if ((current_node.branch > 0) && (current_node.value->num_children() > 0) && next_term->is_function_or_literal()) {
                out << " ";
            }
            if (current_node.branch == 0) {
                out << current_node.value->get_string() << "(";
            }

            current_node.value = next_term;
            current_node.branch = 0;
        }
        // If we have iterated over all the children of this node, pop back to the parent
        else {
            if (current_node.value->num_children() == 0) {
                _rhs = current_node.value;           // Track the last leaf node we've visited in _rhs
                out << current_node.value->get_string();
            }

            current_node.branch = 0;                 // Restore the node we're leaving to branch 0
            current_node = _stack.top();
            _stack.pop();
            _path.pop_back();
            current_node.branch++;
            // Output a , or ) depending on whether the parent has unvisited children
            if (current_node.branch < current_node.value->num_children()) {
                std::cout << ",";
            }
            else {
                std::cout << ")";
            }
        }
    }

    return out;
}


#endif // TERM_HPP
