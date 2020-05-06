#ifndef TERM_HPP
#define TERM_HPP

#include<memory>
#include<vector>
#include<string>
#include<algorithm>
#include<iostream>
#include<exception>
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
    T value;
    term<T>* _left;
    term<T>* _right;
    ~term() {
        delete(_left);
        delete(_right);
    };

    // Now make it STL-complaint:
    public:
        typedef T           value_type;
        typedef T*          pointer;
        typedef T&          reference;
        typedef size_t      size_type;
        typedef ptrdiff_t   different_type;
        //typedef term_iterator<T> iterator;
        //typedef term_iterator<const T> const_iterator;

        term() {                                   // Default constructor
            _left  = nullptr;
            _right = nullptr;
        }
        //term(T v) : value(v), _left(nullptr), _right(nullptr) {}
        term(T l)      : _left(nullptr) {_left = new term<T>(l);} // Construct with one sub-term (default to left)
        term(T l, T r) : _left(nullptr), _right(nullptr) {_left = new term<T>(l); _right = new term<T>(r);} // Construct with two sub-terms

        //term(const term& t);                                // Copy constructor - bigger, so don't inline
        term(term&& t) : _left(nullptr), _right(nullptr) {  // Move constructor - take everything out and make it your own, but set the source's data to null/default
            _left  = t._left;
            _right = t._right;
            t._left  = nullptr;                     // Set the other node's children to null
            t._right = nullptr;                     // Set the other node's children to null
        };
        term& operator=(const term& t);            // Standard = operator for copy assignment
        term& operator=(term&& t);                 // Move = operator for move assignment (usually want this for STL-style classes,
                                                   //                                      is usually more efficient)
                                                   // Also, may want to swap your internals with the thing you're moving from
                                                   //   to avoid memory leaks.
//        //iterator begin() {return term_iterator<T>(_root, true);}
//        //iterator end()   {return term_iterator<T>(_root, false);}
//
        // Data methods
        void insert(T c);
        term<T>* insert(T c, term<T>* at);
        //void remove();
        //void inorder(term<T>* n);

        term(const term& t) {
            // Copy constructor
        };

        void insert(term<T> l) {
            _left = insert(l, _left);
        };
        term<T>* insert(term<T> c, term<T>* at) {
            if(!at) return new term<T>(c);                             // If at is null, return new node (Note: we are intentionally not cleaning up memory for now)
            if(c <= at->value) at->_left  = insert(c, at->_left);      // Insert left if <= current node value
            if(c  > at->value) at->_right = insert(c, at->_right);     // Insert right if > current node value
            return at;                                                 // return the tree we inserted into
        };
        
        void remove() {
          // Move the branch up if you delete it
        };
        
        // Inorder traversal - print out everything on the left, then my value, then everything on the right.
        void inorder(term<T>* n) {
            if (n) {
                inorder(n->_left);                       // Recursively crawl all the way to the left
                std::cout << n->value << std::endl;      // Print each value
                inorder(n->_right);                      // As we work back to the right
            }
        };
};

// These are of type term<T> because they contain sub-terms
template<typename T>
class variable : public term<T> { };

template<typename T>
class literal : public term<T> { };

template<typename T>
class function : public term<T> { };


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
    return out;
}


#endif // TERM_HPP
