#ifndef TREE_HPP
#define TREE_HPP

#include<stack>
#include<iterator>
#include<memory>
#include<iostream>

/**
 * node is used in the implementation, and should not be used outside this file.
 *
 * represents a node in a binary search tree.
 * It has a value, left branch and right branch.
 */
template<typename T>
struct node
{
    T value;
    std::shared_ptr<node<T>> left = nullptr;
    std::shared_ptr<node<T>> right = nullptr;
    node<T>(T v) : value(v), left(nullptr), right(nullptr) {}
    node<T>() = delete;
};

/**
 * a tree_iterator iterates through a tree with an inorder traversal.
 */
template<typename T>
class tree_iterator
{
    // since we have to suspend at several points
    // we nned to keep track of the path to the current node
    std::stack<std::shared_ptr<node<T>>> _path;
    std::shared_ptr<node<T>> _root;
public:
    typedef T                               value_type;
    typedef T*                              pointer;
    typedef T&                              reference;
    typedef size_t                          size_type;
    typedef ptrdiff_t                       difference_type;
    typedef std::bidirectional_iterator_tag iterator_category;

    tree_iterator<T>() = delete;
    tree_iterator<T>(std::shared_ptr<node<T>> n, bool begin);
    tree_iterator<T>(const tree_iterator<T>& i) : _path(i._path), _root(i._root) {}

    T& operator*()  {return _path.top()->value;}
    T* operator->() {return &_path.top()->value;}
    tree_iterator<T>& operator++();
    tree_iterator<T>& operator--();
    tree_iterator<T>  operator++(int)
    {
        tree_iterator<T> tmp(*this);
        ++this;
        return tmp;
    }
    tree_iterator<T>  operator--(int)
    {
        tree_iterator<T> tmp(*this);
        --this;
        return tmp;
    }
    tree_iterator<T>& operator+=(unsigned int n)
    {
        for(int i = 0; i < n; i++) ++*this;
        return *this;
    }
    tree_iterator& operator-=(unsigned int n)
    {
        for(int i = 0; i < n; i++) --*this;
        return *this;
    }
    bool operator==(const tree_iterator<T>& rhs) const
    {
        return _path == rhs._path;
    }
    bool operator!=(const tree_iterator<T>& rhs) const
    {
        return _path != rhs._path;
    }
};

/**
 * A Binary search tree.
 *
 * This class offeres O(log(n)) average insert and find,
 * but it can degrade into O(n)
 */
template<typename T>
class tree
{
private:
    std::shared_ptr<node<T>> _root = nullptr;
public:
    typedef T                               value_type;
    typedef T*                              pointer;
    typedef T&                              reference;
    typedef size_t                          size_type;
    typedef ptrdiff_t                       difference_type;
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef tree_iterator<T>                iterator;
    typedef tree_iterator<const T>          const_iterator;

    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    tree() { _root = nullptr;}
    tree(const tree<T>& rhs)
    {
        for(T& x : rhs)
        {
            insert(x);
        }
    }
    void insert(const T& elem);
    tree(std::initializer_list<T> l)
    {
        for(const T& x : l)
        {
            insert(x);
        }
    }

    iterator begin() {return tree_iterator<T>(_root, true);}
    iterator end() {return tree_iterator<T>(_root, false);}

    const_iterator cbegin()   {return tree_iterator<const T>(_root, true);}
    const_iterator cend()     {return tree_iterator<const T>(_root, false);}
    reverse_iterator rbegin() {return std::reverse_iterator<iterator>(end());}
    reverse_iterator rend()   {return std::reverse_iterator<iterator>(begin());}
    const_reverse_iterator crbegin()
    {
        return std::reverse_iterator<const_iterator>(cend());
    }
    const_reverse_iterator crend()
    {
        return std::reverse_iterator<const_iterator>(cbegin());
    }
};


template<typename T>
tree_iterator<T>::tree_iterator(std::shared_ptr<node<T>> n, bool begin)
{ 
    _root = n;
    if(begin)
    {
        for(; n->left; n = n->left)
        {
            _path.push(n);
        }
        _path.push(n);
    }
}

template<typename T>
tree_iterator<T>& tree_iterator<T>::operator++()
{
    if(!_path.empty())
    {
        if(_path.top()->right)
        {
            _path.push(_path.top()->right);
            while(_path.top()->left)
            {
                _path.push(_path.top()->left);
            }
        }
        else
        {
            std::shared_ptr<node<T>> child = _path.top();
            _path.pop();
            while(!_path.empty() && _path.top()->right == child)
            {
                child = _path.top();
                _path.pop();
            }
        }
    }
    return *this;
}
template<typename T>
tree_iterator<T>& tree_iterator<T>::operator--()
{
    if(!_path.empty())
    {
        if(_path.top()->left)
        {
            _path.push(_path.top()->left);
            while(_path.top()->right)
            {
                _path.push(_path.top()->right);
            }
        }
        else
        {
            std::shared_ptr<node<T>> child = _path.top();
            _path.pop();
            while(!_path.empty() && _path.top()->left == child)
            {
                child = _path.top();
                _path.pop();
            }
        }
    }
    else
    {
        _path.push(_root);
        while(_path.top()->right)
        {
            _path.push(_path.top()->right);
        }
    }
    return *this;
}

template<typename T>
std::shared_ptr<node<T>> insert_node(const T& elem, std::shared_ptr<node<T>> at)
{
    if(!at)
    {
        return std::make_shared<node<T>>(node<T>(elem));
    }
    if(elem < at->value)
    {
        at->left = insert_node(elem, at->left);
    }
    if(elem > at->value)
    {
        at->right = insert_node(elem, at->right);
    }
    return at;
}

template<typename T>
void tree<T>::insert(const T& elem)
{
    _root = insert_node(elem, _root);
}
#endif // TREE_HPP
