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
        std::stack<node<T>> _stack;  
        std::vector<int>    _path;   // Track the path from the root term to our current term
        term<T>* _lhs;
        term<T>* _rhs;           // Our right-most leaf term - the last term
        
    public:
        // Default Constructor
        term_iterator<T>() {}

        // Constructor
        term_iterator<T>(term<T>* n, bool b)
        {
            this->_lhs = n;

            node<T> current_node;
            current_node.value  = this->_lhs;
            current_node.branch = 0;

            // begin():  Leave our iterator at the first node
            if (b) {
                this->_stack.push(current_node);  // Push our current node on the stack
                _path.push_back(current_node.branch);
            }  
            else { // end():  Iterate through and find our _rhs leaf node
                while (!(current_node.value == this->_lhs && (current_node.branch == current_node.value->num_children()))) {

                    // If we haven't iterated over all the children of this node, pick the next one and move down
                    if (current_node.branch < current_node.value->num_children()) {  // Pick a branch to go down
                        this->_stack.push(current_node);  // Push our current node on the stack
                        _path.push_back(current_node.branch);
                        current_node.value = current_node.value->children[current_node.branch].get();  // Get the raw pointer from the child's shared pointer
                        current_node.branch = 0;
                    }
                    // If we have iterated over all the children of this node, pop back to the parent
                    else {
                        if (current_node.value->num_children() == 0) {
                            this->_rhs = current_node.value;     // Track the last leaf node we've visited in _rhs
                        }
                        current_node.branch = 0;                 // Restore the node we're leaving to branch 0
                        current_node = this->_stack.top();
                        this->_stack.pop();
                        this->_path.pop_back();
                        current_node.branch++;
                    }
                }

                // Add one blank node past the end
                current_node.value = nullptr;
                current_node.branch = 0;
                this->_stack.push(current_node);  // Push our current node on the stack
            }
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
            return this->_path == rhs._path;
        }
        bool operator!=(const term_iterator<T>& rhs) const
        {
            return this->_path != rhs._path;
        }
};

// Return value
template<typename T>
term<T>& term_iterator<T>::operator*() const {
    return *(this->_stack.top().value);
};

// Return a reference to value
template<typename T>
term<T>* term_iterator<T>::operator->() const {  // It's an -> because we could use it to call a method
    return this->_stack.top().value;
};

template<typename T>
term_iterator<T>& term_iterator<T>::operator++() {

    if(~this->_stack.empty()) {
        if ((this->_stack.top().value->num_children() > 0) && (this->_stack.top().branch < this->_stack.top().value->num_children())) {
            node<T> child_node;
            child_node.value = this->_stack.top().value->children[this->_stack.top().branch].get();
            this->_stack.push(child_node);  // Push our selected child node onto the stack
        }
        else {
            // Go up, unless we're at the root and have visited all it's children
            if (!(this->_stack.top().value == this->_lhs && (this->_stack.top().branch == this->_stack.top().value->num_children()))) {
                this->_stack.pop();
                this->_stack.top().branch++;

                // Keep going up until we find a node that has unvisited children or the root
                while(this->_stack.top().value != this->_lhs && (this->_stack.top().branch != this->_stack.top().value->num_children())) {
                    this->_stack.pop();
                    this->_stack.top().branch++;
                }
            }
        }
    }

    return *this;

};
