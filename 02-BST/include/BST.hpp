#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>
#include <iterator>
#include <memory>


template <typename T>
class TreeNode
{
    public:
        T element;
        std::unique_ptr<TreeNode<T>> left;
        std::unique_ptr<TreeNode<T>> right;

        TreeNode<T>(const T& e)
            :element{e}, left{nullptr}, right{nullptr} {}

        ~TreeNode() {}

};


template <typename T>
struct BST
{
    public:
        std::unique_ptr<TreeNode<T>> root = nullptr;

        ~BST() {}

        bool insert(const T& key);
        bool search(const T& key);
        bool remove(const T& key);

    private:
        bool insert(std::unique_ptr<TreeNode<T>>& t, const T& key);
        bool search(std::unique_ptr<TreeNode<T>>& t, const T& key);
        bool remove(std::unique_ptr<TreeNode<T>>& t, const T& key);
        std::unique_ptr<TreeNode<T>>& FindMaxLSubTree(std::unique_ptr<TreeNode<T>>& t);

};

template <typename T>
bool BST<T>::insert(const T& key) {
    return insert(root, key);
}

template <typename T>
bool BST<T>::search(const T& key) {
    return search(root, key);
}

template <typename T>
bool BST<T>::remove(const T& key) {
    return remove(root, key);
}

template <typename T>
std::unique_ptr<TreeNode<T>>& BST<T>::FindMaxLSubTree(std::unique_ptr<TreeNode<T>>& t){
    if(t->right == nullptr) {
        return t;
    }

    else {
        return FindMaxLSubTree(t->right);
    }
}

template <typename T>
bool BST<T>::insert(std::unique_ptr<TreeNode<T>>& t, const T& key) {

    // TODO
    // if insertion fails (i.e. if the key already exists in tree), return false
    // otherwise, return true

    // insert node
    if(t == nullptr){
        t = std::make_unique< TreeNode<T> >(key);
        return true;
    }

    // there are nodes
    else {
        if(t->element == key)
            return false;
        
        else if(t->element > key){
            return insert(t->left, key);
        }

        else {  // if(t->element < key)
            return insert(t->right, key);
        }
    }

}

template <typename T>
bool BST<T>::search(std::unique_ptr<TreeNode<T>>& t, const T& key) {

    // TODO
    // if key exists in tree, return true
    // otherwise, return false

    if(t == nullptr){
        return false;
    }

    else {
        if(t->element == key)
            return true;
        
        else if(t->element > key)
            return search(t->left, key);
        
        else
            return search(t->right, key);
    }

}

template <typename T>
bool BST<T>::remove(std::unique_ptr<TreeNode<T>>& t, const T& key) {

    // TODO
    // if key does not exist in tree, return false
    // otherwise, return true

    // not in tree
    if(t == nullptr)
        return false;

    // found
    else if(t->element == key){
        // 1. is a leaf
        if(t->left == nullptr && t->right == nullptr){
            t.reset();
            return true;
        }
    
        // 2. has one child
        else if(t->left == nullptr || t->right == nullptr){
            // has left child
            if(t->right == nullptr){
                t = std::move(t->left);
                return true;
            }

            // has right child
            else{   // t->left == nullptr
                t = std::move(t->right);
                return true;
            }
        }

        // 3. has two children
        else {
            // find maximum of L_subtree
            std::unique_ptr<TreeNode<T>>& SwapNode = FindMaxLSubTree(t->left);
            t->element = SwapNode->element;
            SwapNode = std::move(SwapNode->left);
            return true;
        }
    }
        
    // didn't found
    else{
        // key is bigger than node
        if(key > t->element){
            return remove(t->right, key);
        }
        else {  // if(t->element < key)
            return remove(t->left, key);
        }
    }
}