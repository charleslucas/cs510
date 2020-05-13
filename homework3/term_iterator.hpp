#include <stack>
#include <iterator>

// Our term iterator class
template<typename T>
class term_iterator {
    private:
        std::stack<term<T>*> _path;  // Track the path from the root term to our current term
    public:
        // Constructor
        // Go left, and keep pushing the parent onto the stack
        term_iterator<T>(term<T>* n)
        {
            if(n) {
                for(; n->left; n = n->left) {
                    this._path.push(n);
                }
                this._path.push(n);
            }
        }

        T&                operator*()  const;          // Dereference
        T*                operator->() const;          // Call a method on
        term_iterator<T>& operator++();                // Preincrement ++ - move the iterator forwards
        term_iterator<T>& operator--();                // Preincrement --
        term_iterator<T>  operator++(int);             // Postincrement ++
        term_iterator<T>  operator--(int);             // Postincrement --
        term_iterator<T>& operator+=(unsigned int);    // Just loop ++
        term_iterator<T>& operator-=(unsigned int);    // Just loop --
        bool operator==(const term_iterator<T>& rhs) const
        {
            return this._path == rhs._path;
        }
        bool operator!=(const term_iterator<T>& rhs) const
        {
            return this._path != rhs._path;
        }

};

// Return value
template<typename T>
T& term_iterator<T>::operator*() const {
    return this._path.top()->value;
};

// Return a reference to value
template<typename T>
T* term_iterator<T>::operator->() const {  // It's an -> because we could use it to call a method
    return &this._path.top()->value;
};

// ++ does a surprising amount of work
template<typename T>
term_iterator<T>& term_iterator<T>::operator++() {
    if(~this._path.empty()) {
        if(this._path->right()) {
            this._path.push(this._path.top()->right);  // Go to the right - if I'm at the rightmost part of the tree then I've visited every value
            while(this._path.top()->left) {
                this._path.push(this._path.top()->left); // Keep iterating to the left, we want to be as far to the left as we can
            }
        }
        else {
            term<T>* child = this._path.top();
            this._path.pop();                                                 // Go up
            while(!_path.empty() && this._path.top()->right == child) {       // If I came from the right, don't go straight back down to the right
                child = this._path.top();
                this._path.pop();
            }
        }
    }
};
