#include <stack>
#include <iterator>

template<typename T>
struct node {
    term<T>* value;
    int      branch = 0;
};

// Our term iterator class
template<typename T>
class term_iterator {
    private:
        std::stack<node<T>> _path;  // Track the path from the root term to our current term
        term<T>* _lhs;
        term<T>* _rhs;           // Our right-most leaf term - the last term
        
    public:
        // Default Constructor
        term_iterator<T>() {}

        // Constructor
        term_iterator<T>(term<T>* n, bool b)
        {
            this->_lhs = n;

            //this->_path.push(n);  // Push the root term onto the stack

            node<T> current_node;
            current_node.value  = this->_lhs;
            current_node.branch = 0;

            // Iterate through and find our _rhs leaf node
            while (current_node.value != this->_lhs && current_node.branch < (current_node.value->num_children())) {
                this->_path.push(current_node);  // Push our current node on the stack

                // If we haven't iterated over all the children of this node, pick the next one and move down
                if (current_node.branch < current_node.value->num_children()) {  // Pick a branch to go down
                    current_node.value = current_node.value->children[current_node.branch].get();  // Get the raw pointer from the child's shared pointer
                    current_node.branch = 0;
                    std::cout << "moving down" << std::endl;
                }
                // If we have iterated over all the children of this node, pop back to the parent
                else {
                    this->_rhs = current_node.value;     // Track the last leaf node we've visited in _rhs
                    current_node = this->_path.top();
                    current_node.branch = 0;             // Restore the node we're leaving to branch 0
                    this->_path.pop();
                    std::cout << "popping up" << std::endl;
                }
            }
            std::cout << "Found our _rhs node" << std::endl;
        }

        term<T>&          operator*()  const;          // Dereference
        term<T>*          operator->() const;          // Call a method on
        term_iterator<T>& operator++();                // Preincrement ++ - move the iterator forwards
        term_iterator<T>& operator--();                // Preincrement --
        term_iterator<T>  operator++(int);             // Postincrement ++
        term_iterator<T>  operator--(int);             // Postincrement --
        term_iterator<T>& operator+=(unsigned int);    // Just loop ++
        term_iterator<T>& operator-=(unsigned int);    // Just loop --
        bool operator==(const term_iterator<T>& rhs) const
        {
            //return this->_path == rhs._path;
            return true;
        }
        bool operator!=(const term_iterator<T>& rhs) const
        {
            //return this->_path != rhs._path;
            return false;
        }
};

// Return value
template<typename T>
term<T>& term_iterator<T>::operator*() const {
    return *(this->_path.top().value);
};

// Return a reference to value
template<typename T>
term<T>* term_iterator<T>::operator->() const {  // It's an -> because we could use it to call a method
    return this->_path.top().value;
};

// ++ does a surprising amount of work
template<typename T>
term_iterator<T>& term_iterator<T>::operator++() {

    if(~this->_path.empty()) {
        if ((this->_path.top().value->num_children() > 0) && (this->_path.top().branch < this->_path.top().value->num_children())) {
            node<T> child_node;
            child_node.value = this->_path.top().value->children[this->_path.top().branch].get();
            this->_path.push(child_node);  // Push our selected child node onto the stack
        }
        else {
            // Go up
            this->_path.pop();
            std::cout << "popping up" << std::endl;
            // If we're already at the rightmost node, keep going up
            while(!_path.empty() && this->_path.top().branch < this->_path.top().value->num_children()) {
                this->_path.pop();
            }
        }
    }

    return *this;

};
