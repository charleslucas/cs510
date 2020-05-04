#ifndef TREE_HPP
#define TREE_HPP

#include <cstddef>
#include <iostream>

template<typename T>
struct node {
    T value;
    node<T>* left;
    node<T>* right;
    node(T v) : value(v), left(nullptr), right(nullptr) {}
    ~node();  // Normally we would clean up memory here
};

template<typename T>
class tree {
    private:
        node<T>* _root;

    // Now make it STL-complaint:
    public:
        typedef T           value_type;
        typedef T*          pointer;
        typedef T&          reference;
        typedef size_t      size_type;
        typedef ptrdiff_t   different_type;

        tree()         : _root(nullptr) {}         // Default constructor
        tree(T c)      : _root(nullptr) {_root = new node<T>(c);} // Construct with a value
        tree(const tree& t);                       // Copy constructor - bigger, so don't inline
        tree(tree&& t) : _root(nullptr) {          // Move constructor - take everything out and make it your own, but set the source's data to null/default
            _root = t._root;
            t._root = nullptr;                     // Set the other node's root to be null
        };
        tree& operator=(const tree& t);            // Standard = operator for copy assignment
        tree& operator=(tree&& t);                 // Move = operator for move assignment (usually want this for STL-style classes,
                                                   //                                      is usually more efficient)
                                                   // Also, may want to swap your internals with the thing you're moving from
                                                   //   to avoid memory leaks.

        // Data methods
        void insert(T c);
        node<T>* insert(T c, node<T>* at);
        void remove();
        void inorder(node<T>* n);
};

template<typename T>
tree<T>::tree(const tree& t) {
    // Copy constructor
};
template<typename T>
void tree<T>::insert(T c) {
    _root = insert(c, _root);
};
template<typename T>
node<T>* tree<T>::insert(T c, node<T>* at) {
    if(!at) return node<T>(c);                                   // If at is null, return new node
    if(c <= at->value) at->left  = insert(c, at->left);      // Insert left if <= current node value
    if(c  > at->value) at->right = insert(c, at->right);     // Insert right if > current node value
    return at;                                                // return the tree we inserted into
};
template<typename T>
void tree<T>::remove() {
  // Move the branch up if you delete it
};
// Inorder traversal - print out everything on the left, then my value, then everything on the right.
template<typename T>
void tree<T>::inorder(node<T>* n) {
    if (n) {
        inorder(n->left);
        std::cout << n->value << std::endl;
        inorder(n->right);
    }
};


// Our tree iterator class
template<typename T>
class tree_iterator {
    private:
        node<T>* _pos;  // Position - where it is in the tree
    public:
        tree_iterator<T>(node<T>* root)
        {
            _pos = root;
            if(_pos) {
                for(; _pos->left; _pos = _pos->left);
            }
        }

        T& operator*()  const;                      // Dereference
        T* operator->() const;                      // Call a method on
        tree_iterator& operator++();                // Preincrement ++ - move the iterator forwards
        tree_iterator& operator--();                // Preincrement --
        tree_iterator  operator++(int);             // Postincrement ++
        tree_iterator  operator--(int);             // Postincrement --
        tree_iterator& operator+=(unsigned int);    // Just loop ++
        tree_iterator& operator-=(unsigned int);    // Just loop --
};

// Return value
template<typename T>
T& tree_iterator<T>::operator*() const {
    return _pos->value;
}

// Return a reference to value
template<typename T>
T* tree_iterator<T>::operator->() const {  // It's an -> because we could use it to call a method
    return &_pos->value;
}

//template<typename T>
//tree_iterator<T>::operator++() {
//    if(_pos) {
//        inorder(_pos->left);
//        return *this;
//        inorder(_pos->right);
//    }
//}

#endif // TREE_HPP