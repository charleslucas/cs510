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
        }

        virtual void print() {
            std::cout << "term" << std::endl;
        }

        // Preorder traversal - start at the root, print out everything on the left, then everything on the right.
        virtual void preorder(const term<T>& t) {
        };

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
            std::cout << "variable:  " << name << std::endl;
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
            return std::to_string(value);
        }

        void print() {
            std::cout << "literal:  " << value << std::endl;
        }
};

template<typename T>
class function : public term<T> {
    std::string symbol;

    public:

        // Constructor
        function(std::string s, int size, std::vector<term_ptr<T>> k) {
            symbol = s;
            for (int i; i < size; i++) {
                this->children[i] = k[i];
            }
        } 

        // Copy constructor
        function(const function& t) {
           t.symbol   = symbol;
           t.children = this->children;
        };

        // Move constructor - take everything out and make it your own, but set the source's data to null/default
        function(function&& t) {
            symbol    = t.symbol;
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
            std::cout << "function:  " << symbol << std::endl;
        }

        //// Preorder traversal - start at the root, print out everything on the left, then everything on the right.
        //void preorder() {
        //    std::cout << symbol << "(" << std::endl;      // Print this function symbol
        //    //std::cout << t->value << std::endl;              // Print each value
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
std::ostream& operator<<(std::ostream& out, term<T>& t)
{
    term_iterator<T> ptr;

    for (ptr = t.begin(); ptr != t.end(); ++ptr) {

    }

    //ptr = t.begin();
    //out <<  ptr.get_string();
    //ptr = t.end();
    //out <<  ptr.get_string();

    return out;
}


#endif // TERM_HPP
