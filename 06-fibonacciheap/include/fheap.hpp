#ifndef __FHEAP_H_
#define __FHEAP_H_

#include <iostream>
#include <initializer_list>
#include <optional>
#include <vector>
#include <cmath>
#include <memory>
#include <queue>

template <typename T>
class PriorityQueue {
    public:
        virtual void insert(const T& item) = 0;
        virtual std::optional<T> extract_min() = 0;
        virtual bool is_empty() const = 0;
};

template <typename T>
class FibonacciNode {
    public:
        // constructors
        FibonacciNode();
        FibonacciNode(const T& item)
            :key(item), degree(0), child(nullptr), right(nullptr) {}

        // destructor
	// If you want to implement custom destructor, remove the default keyword
	// and implement your custom destructor.
        ~FibonacciNode() = default;

        T key;
        size_t degree;

        std::shared_ptr<FibonacciNode<T>> right;
        std::shared_ptr<FibonacciNode<T>> child;
        // NOTE: If you set left/parent pointer to shared_ptr, circular reference may cause!
        // So, left/parent pointer should be set to weak_ptr.
        std::weak_ptr<FibonacciNode<T>> left;
        std::weak_ptr<FibonacciNode<T>> parent;
};

template <typename T>
class FibonacciHeap : public PriorityQueue<T> {
    public:
        // Default Constructor
        FibonacciHeap()
            : min_node(nullptr), size_(0) {}

        // Constructor with Value
        FibonacciHeap(const T& item)
            : min_node(nullptr), size_(0) { insert(item); }

        // Disable copy constructor.
        FibonacciHeap(const FibonacciHeap<T> &);

        // Constructor with initializer_list
        // ex) FibonacciHeap<int> fheap = {1, 2, 3, 4};
        FibonacciHeap(std::initializer_list<T> list): min_node(nullptr), size_(0) {
            for(const T& item : list) {
                insert(item);
            }
        }

        // Destructor
        ~FibonacciHeap();

        // Insert item into the heap.
        void insert(const T& item) override;

        // Return raw pointer of the min node.
        // If the heap is empty, return nullptr.
        FibonacciNode<T>* get_min_node() { return min_node.get(); }

        // Return minimum value of the min node.
        // If the heap is empty, return std::nullopt.
        std::optional<T> get_min() const;

        // 1. Return minimum value of the min node
        // 2. Remove the node which contains minimum value from the heap.
        // If the heap is empty, return std::nullopt;
        std::optional<T> extract_min() override;

        // Return true if the heap is empty, false if not.
        bool is_empty() const override { return !size_; }

        // Return the number of nodes the heap contains.
        size_t size() const { return size_; }


    private:
        // Points to the node which has minimum value.
        std::shared_ptr<FibonacciNode<T>> min_node;

        // Value that represents how many nodes the heap contains.
        size_t size_;

        void insert(std::shared_ptr<FibonacciNode<T>>& node);

        // After extract, clean up the heap.
        void consolidate();

        // Combine two nodes.
        void merge(std::shared_ptr<FibonacciNode<T>>& x, std::shared_ptr<FibonacciNode<T>>& y);
};

template <typename T>
FibonacciHeap<T>::~FibonacciHeap() {
    // TODO
    size_t temp = size_;
    for(size_t i = 0; i < temp; i++) {
        extract_min();
    }
}

template <typename T>
std::optional<T> FibonacciHeap<T>::get_min() const {
    // TODO
    if(size_ == 0){
        return std::nullopt;
    }
    else {
        return min_node->key;
    }
}

template <typename T>
void FibonacciHeap<T>::insert(const T& item) {
    // TODO
    std::shared_ptr<FibonacciNode<T>> insertNode = std::make_shared<FibonacciNode<T>>(item);
    // initialization of insertNode
    insertNode->left = insertNode;
    insertNode->right = insertNode;
    size_++;
    // put insertNode and set min_node
    if (min_node != nullptr) {
        (min_node->left.lock())->right = insertNode;
        insertNode->right = min_node;
        insertNode->left = min_node->left;
        min_node->left = insertNode;
        if(insertNode->key < min_node->key) {
            min_node = insertNode;
        }
    }
    else {
        min_node = insertNode;
    }
    
}

template <typename T>
void FibonacciHeap<T>::insert(std::shared_ptr<FibonacciNode<T>>& node) {
    // TODO
    if(min_node != nullptr) {
        // set parents of min_node's childs
        std::shared_ptr<FibonacciNode<T>> tempNode = node;       
        std::shared_ptr<FibonacciNode<T>> temp = min_node->left.lock();
        (min_node->left.lock())->right = node;
        min_node->left = node->left;
        (node->left.lock())->right = min_node;
        node->left = temp;
    } 
}

template <typename T>
std::optional<T> FibonacciHeap<T>::extract_min() {
    // TODO
    if(min_node == nullptr) {
        return std::nullopt;
    }

    if(size_ == 1){
        T returnValue = min_node->key;
        min_node->right.reset();
        min_node->left.reset();
        min_node.reset();
        // min_node = nullptr;
        size_--;
        return returnValue;
    }

    // Heap is not empty
    // Move min_node to any node in root
    std::shared_ptr<FibonacciNode<T>> minNodeTemp = min_node;
    T returnValue = min_node->key;

    if(min_node->right == min_node) {
        // min_node is the only root, but is going to be deleted
        std::shared_ptr<FibonacciNode<T>> ttemp = min_node->child;
        min_node = ttemp;
        
        // find the smallest node of ttemp's siblings
        while(true) {
            ttemp = ttemp->right;
            if(ttemp == min_node) {
                break;
            }
            if(ttemp->key < min_node->key) {
                min_node = ttemp;
            }
        }
    }
    else {
        min_node = min_node->right;
        min_node->left = minNodeTemp->left;
        (minNodeTemp->left.lock())->right = min_node;

        // if min_node had childs, insert them
        if(minNodeTemp->child != nullptr) {
            insert(minNodeTemp->child);
        }
    }

    minNodeTemp->right.reset();
    minNodeTemp->left.reset();
    minNodeTemp.reset();
    // set min_node as the smallest among its siblings
    std::shared_ptr<FibonacciNode<T>> min_temp = min_node;
    while(true) {
        min_temp = min_temp -> right;
        if(min_temp == min_node) break;
        if(min_temp->key < min_node->key)
            min_node = min_temp;
    }
    
    // settings for remaining root nodes
    size_--;
    
    consolidate();

    return returnValue;
}

template <typename T>
void FibonacciHeap<T>::consolidate() {
    // TODO
    // should set the array size as log n base phi + 1, but just setted as n * 2
    // Array Initialization
    size_t sizeArray = size_*2;
    std::shared_ptr<FibonacciNode<T>>* array = new std::shared_ptr<FibonacciNode<T>>[sizeArray];
    for(size_t i = 0; i < sizeArray; i++) {
        array[i] = nullptr;
    }

    // Consolidate
    std::shared_ptr<FibonacciNode<T>> thistempNode = min_node;  // main temp node
    std::shared_ptr<FibonacciNode<T>> alreadyNode;      // node which is already in the array
    std::shared_ptr<FibonacciNode<T>> swaptempNode;     // temp node used when swapping
    std::shared_ptr<FibonacciNode<T>> preNode;          // next searching node

    while(true) {
        // set preNode at the first (b/c thistempNode's location might change)
        preNode = thistempNode->right;
        size_t thisDegree = thistempNode->degree;
        // one cycle tour finish -> break
        if (array[thisDegree] == thistempNode) {
            break;
        }
        while(array[thisDegree] != nullptr) {
            alreadyNode = array[thisDegree];
            if(thistempNode->key >= alreadyNode->key && thistempNode != min_node) {
                // swap two nodes so that thistempNode->key is always smaller than alreadyNode->key
                swaptempNode = thistempNode;
                thistempNode = alreadyNode;
                alreadyNode = swaptempNode;
            }
            if(alreadyNode == min_node && alreadyNode->key > thistempNode->key) {
                if(alreadyNode == preNode) {
                    // preNode jump
                    preNode = preNode -> right;
                }
                min_node = thistempNode;
            }
            // merge two nodes
            merge(thistempNode, alreadyNode);

            array[thisDegree] = nullptr;
            thisDegree++;
        }

        // update array and degree
        if(thistempNode->key < min_node->key) {
            min_node = thistempNode;
        }
        array[thisDegree] = thistempNode;        
        thistempNode = preNode;
    }

    delete[] array;
}

template <typename T>
void FibonacciHeap<T>::merge(std::shared_ptr<FibonacciNode<T>>& x, std::shared_ptr<FibonacciNode<T>>& y) {
    // TODO
    // y becomes a child of x
    (y->right)->left = y->left;
    (y->left.lock())->right = y->right;

    // x has no siblings
    if(x->right == x) {
        min_node = x;
    }

    // connection
    y->left = y;
    y->right = y;
    y->parent = x;
    if(x->child == nullptr) {
        x->child = y;
    }
    else {
        y->right = x->child;
        y->left = (x->child)->left;
        ((x->child)->left.lock())->right = y;
        (x->child)->left = y;
    }

    x->degree++;
}

#endif // __FHEAP_H_
