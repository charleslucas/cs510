#ifndef TERM_HPP
#define TERM_HPP

#include<memory>
#include<vector>
#include<string>
#include<algorithm>
#include<iostream>
#include<exception>
#include<iterator>
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
        iterator begin() {return term_iterator<T>(nullptr, true);}
        iterator end()   {return term_iterator<T>(nullptr, false);}

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

};

template<typename T>
class function : public term<T> {
    std::string symbol;
    std::vector<term_ptr<T>> children;

    public:

        // Constructor
        function(std::string s, int size, std::vector<term_ptr<T>> k) {
            symbol = s;
            for (int i; i < size; i++) {
                children[i] = k[i];
            }
        } 

        // Copy constructor
        function(const function& t) {
           t.symbol   = symbol;
           t.children = children;
        };

        // Move constructor - take everything out and make it your own, but set the source's data to null/default
        function(function&& t) {
            symbol    = t.symbol;
            children  = t.children;
            t.symbol  = "";
            t.children.clear();
        };

        // Deconstructor
        ~function() {
        };

        // Inorder traversal - print out everything on the left, then my value, then everything on the right.
        //void inorder(term<T>* n) {
        //    if (n) {
        //        inorder(n->_left);                       // Recursively crawl all the way to the left
        //        std::cout << n->value << std::endl;      // Print each value
        //        inorder(n->_right);                      // As we work back to the right
        //    }
        //};
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
}

/////////////////////////////////////////////////////////////////
//
// rewrite
//
/////////////////////////////////////////////////////////////////

template<typename T>
term_ptr<T> rewrite(term_ptr<T> t, term<T>& rhs, std::vector<int> path, const Sub<T>& sigma)
{
}

/////////////////////////////////////////////////////////////////
//
// operator<<
//
/////////////////////////////////////////////////////////////////

template<typename T>
std::ostream& operator<<(std::ostream& out, const term<T>& t)
{
//    inorder(t);

    return out;
}


#endif // TERM_HPP
