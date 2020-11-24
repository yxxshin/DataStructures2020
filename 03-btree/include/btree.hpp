#include <cstddef>
#include <array>
#include <iostream>
#include <optional>
#include <iterator>
#include <algorithm>
#include <cstring>
#include <string>
#include <sstream>
#include <functional>

enum class NodeType { LEAF, INTERNAL };

template<typename T, size_t B = 6>
struct BTreeNode;

template<typename T, size_t B = 6>
struct BTree {
    BTreeNode<T, B>* root = nullptr;

    ~BTree() { if (root) delete root; }

    bool insert(const T&);
    bool remove(const T&);

    void for_all(std::function<void(T&)>);
    void for_all_nodes(std::function<void(const BTreeNode<T,B>&)>);

    const std::optional<T> find_rightmost_key() const;
    const std::optional<size_t> depth() const;

    std::string format(void);
};

template<typename T, size_t B>
struct BTreeNode {
    NodeType type;
    size_t n;
    std::array<T, 2 * B - 1> keys;
    std::array<BTreeNode *, 2 * B> edges;

    BTreeNode();
    BTreeNode(const T& t);
    BTreeNode(std::initializer_list<T>);

    template<typename InputIt>
    BTreeNode(InputIt begin, InputIt end);

    ~BTreeNode();

    bool insert(const T& t);
    size_t get_index(const T& t);

    void for_all(std::function<void(T&)> func);

    bool remove(const T& t);

    size_t depth(void);
    std::string format_subtree(size_t);
    std::string format_level(size_t);
    std::string format_node(void);
    std::vector<BTreeNode<T, B>*> find_nodes_at_level(size_t);

    void for_all_nodes(std::function<void(const BTreeNode&)>);

    static std::pair<BTreeNode*, size_t> search(BTreeNode<T, B>*, const T& t);
    static void split_child(BTreeNode<T, B>&, size_t);
    static bool try_borrow_from_sibling(BTreeNode<T, B>&, size_t);
    static bool borrow_from_right(BTreeNode<T, B>&, size_t);
    static bool borrow_from_left(BTreeNode<T, B>&, size_t);

    /* NOTE: If the root node has only one key, it will be empty after
      merging the children. Take care of updating the root. I guess this is
      the only way a B-tree may shrink its height. */
    static bool merge_children(BTreeNode<T, B>&, size_t);

    static T& find_rightmost_key(BTreeNode<T, B>&);
};

template<typename T,  size_t B>
bool BTree<T, B>::insert(const T& t) {
    if (!root) {
        root = new BTreeNode<T, B>(t);
        return true;
    }

    /* Make sure the root node is not full. Create an empty tree which has
       the original root as a child. Then split the original root. */
    if (root->n == 2 * B - 1) {
        BTreeNode<T, B>* new_root = new BTreeNode<T, B>{};
        new_root->edges[0] = root;
        BTreeNode<T, B>::split_child(*new_root, 0);
        root = new_root;
    }

    return root->insert(t);
}

/* By default, use in-order traversal */
template<typename T, size_t B>
void BTree<T, B>::for_all(std::function<void(T&)> func) {
    if (root)
        root->for_all(func);
}

/* This isn't necessarily the in-order traversal */
template<typename T, size_t B>
void BTree<T, B>::for_all_nodes(std::function<void(const BTreeNode<T,B>&)> func) {
    if (root)
        root->for_all_nodes(func);
}

template<typename T, size_t B>
const std::optional<T> BTree<T, B>::find_rightmost_key() const {
    if (!root)
        return std::nullopt;

    return BTreeNode<T, B>::find_rightmost_key(*root);
}

template<typename T, size_t B>
const std::optional<size_t> BTree<T, B>::depth() const {
    if (!root)
        return std::nullopt;

    return root->depth();
}

template<typename T, size_t B>
bool BTreeNode<T, B>::insert(const T& t) {
    // TODO
    size_t loca = get_index(t);

    if(type == NodeType::INTERNAL) {
        if(edges[loca]->n == 2*B - 1){  // check
            split_child(*this, loca);
            loca = get_index(t);
        }

        return edges[loca]->insert(t);
    }

    else { // type == NodeType::LEAF
        (this->n)++;
        if(this->n >= 2){
            for(size_t i = (this->n) - 2; i >= loca; i--){
                keys[i+1] = keys[i];
                if(i==0) break;
            }
        }
        keys[loca] = t;
        return true;
    }

    return true;
}

/**
 * Find the desired position of t in current node.
 *
 * For example, if `n` looks like the following:
 *
 * [ 3 | 9 | 13 | 27 ]
 *
 * Then,
 *     n.get_index(2) = 0
 *     n.get_index(5) = 1
 *     n.get_index(10) = 2
 *     n.get_index(19) = 3
 *     n.get_index(31) = 4
 */
template<typename T, size_t B>
size_t BTreeNode<T, B>::get_index(const T& t) {
    // TODO
    size_t temp = 0;
    for(temp = 0; temp < n; temp++){
        if(t < keys[temp])
            break;
    }
    return temp;
}

template<typename T, size_t B>
void BTreeNode<T, B>::for_all(std::function<void(T&)> func) {
    if (type == NodeType::LEAF) {
        for (auto j = 0; j < n; j++)
            func(keys[j]);
    } else {
        if (n < 1)
            return;

        for (auto j = 0; j < n; j++) {
            edges[j]->for_all(func);
            func(keys[j]);
        }

        /* The rightest edge */
        edges[n]->for_all(func);
    }
}

/* This isn't necessarily the in-order traversal */
template<typename T, size_t B>
void BTreeNode<T, B>::for_all_nodes(std::function<void(const BTreeNode<T,B>&)> func) {
    if (type == NodeType::LEAF) {
        func(*this);
    } else {
        if (n < 1)
            return;

        func(*this);

        for (auto j = 0; j < n + 1; j++) {
            edges[j]->for_all_nodes(func);
        }
    }
}

/* Assume this is called only when the child parent.edges[idx] is full, and
   the parent is not full. */
template<typename T, size_t B>
void BTreeNode<T, B>::split_child(BTreeNode<T, B>& parent, size_t idx) {
    // TODO
    parent.type = NodeType::INTERNAL;
    BTreeNode<T, B>* full_node = parent.edges[idx];
    T mid_val = full_node->keys[B-1];

    BTreeNode<T, B>* front_node = new BTreeNode<T,B>{};
    BTreeNode<T, B>* back_node = new BTreeNode<T,B>{};

    for(size_t i = 0; i <= B-2; i++){
        front_node->keys[i] = full_node->keys[i];
        front_node->n++;
    }

    for(size_t i = B; i < 2*B-1; i++){
        back_node->keys[i-B] = full_node->keys[i];
        back_node->n++;
    }

    if(full_node->type == NodeType::INTERNAL){
        front_node->type = NodeType::INTERNAL;
        back_node->type = NodeType::INTERNAL;
        for(size_t i = 0; i <= B-1; i++){
            front_node->edges[i] = full_node->edges[i];
        }
        for(size_t i = B; i <= 2*B-1; i++){
            back_node->edges[i-B] = full_node->edges[i];
        }
    }

    parent.n ++;

    if(parent.n >= 2){
        for(size_t i = parent.n-2; i >= idx; i--){
            parent.keys[i+1] = parent.keys[i];
            if(i==0) break;
        }
    }

    for(size_t i = parent.n-1; i >= idx+1; i--){
        parent.edges[i+1] = parent.edges[i];
        if(i==0) break;
    }
    parent.keys[idx] = mid_val;
    parent.edges[idx+1] = back_node;
    parent.edges[idx] = front_node;

    if(full_node->type == NodeType::INTERNAL){
        for(size_t i = 0; i <= 2*B-1; i++){
            full_node->edges[i] = nullptr;
        }
    }
    delete full_node;
}

template<typename T, size_t B>
bool BTree<T, B>::remove(const T& t) {
    if (!root)
        return false;

    root->remove(t);

    /* After merging, the size of the root may become 0. */
    if (root->n == 0 && root->type == NodeType::INTERNAL) {
        auto prev_root = root;
        root = root->edges[0];
        prev_root->type = NodeType::LEAF;
        delete prev_root;
    }
    return true;
}

template<typename T, size_t B>
bool BTreeNode<T, B>::remove(const T& t) {
    // TODO
    // find index
    size_t idx = 0;
    while (idx < n && keys[idx] < t){
        idx++;
    }

    // found node
    if(idx < n && keys[idx] == t){
        // if target is LEAF
        if(type == NodeType::LEAF){
            // remove from LEAF
            for(size_t i = idx + 1; i < n; i++){
                keys[i - 1] = keys[i];
            }
            n--;
        }
        else {  // if target is INTERNAL
            // remove from INTERNAL
            // Node is replaced by predecessor if left child has more keys than B
            if(edges[idx]->n >= B){
                // get predecessor
                BTreeNode<T, B>* temp = edges[idx];
                while(temp->type != NodeType::LEAF) {
                    temp = temp->edges[temp->n];
                }
                T pred = temp->keys[temp->n - 1];
                pred = temp->find_rightmost_key(*edges[idx]);
                keys[idx] = pred;
                edges[idx]->remove(pred);
            }

            // Node can also be replaced by successor if right child has more keys than B
            else if(edges[idx+1]->n >= B){
                // get successor
                BTreeNode<T, B>* temp = edges[idx+1];
                while(temp->type != NodeType::LEAF){
                    temp = temp->edges[0];
                }
                T succ = temp->keys[0];
                keys[idx] = succ;
                edges[idx+1]->remove(succ);
            }
            
            else {
                merge_children(*this, idx);
                edges[idx]->remove(t);
            }
        }
    } 
    // didn't find node
    else {
        // not existing in tree
        if(type == NodeType::LEAF){
            return false;
        }

        bool flag = ( (idx == n) ? true : false );

        if(flag && idx > n) {
            edges[idx - 1] -> remove(t);
        }
        else {
            edges[idx] -> remove(t);
        }

        if(edges[idx]->n < B-1) {
            // fill 
            if(idx != 0 && edges[idx-1]->n >= B){
                borrow_from_left(*this, idx);
            }

            else if(idx != n && edges[idx+1]->n >= B){
                borrow_from_right(*this, idx);
            }

            else {
                if(idx != n){
                    merge_children(*this, idx);
                }
                else {
                    merge_children(*this, idx - 1);
                }
            }
        }
    }
    return true;
}

/**
 * Try to borrow a key from sibling.
 *
 * @e: The index of the edge that are trying to borrow a key
 * @return true if borrowing succeed, false otherwise
 */
template<typename T, size_t B>
bool BTreeNode<T, B>::try_borrow_from_sibling(BTreeNode<T, B>&node, size_t e) {
    // TODO
    return false;
}

template<typename T, size_t B>
bool BTreeNode<T, B>::borrow_from_right(BTreeNode<T, B>& node, size_t edge) {
    // TODO
    BTreeNode<T, B>* child = node.edges[edge];
    BTreeNode<T, B>* sibling = node.edges[edge + 1];

    // borrow
    child->keys[child->n] = node.keys[edge];

    if(child->type != NodeType::LEAF) {
        child->edges[(child->n)+1] = sibling->edges[0];
    }

    node.keys[edge] = sibling->keys[0];

    // filling space of sibling
    for(size_t i = 1; i < sibling->n; i++){
        sibling->keys[i-1] = sibling->keys[i];
    }

    if(sibling->type != NodeType::LEAF){
        for(size_t i = 1; i <= sibling->n; i++){
            sibling->edges[i - 1] = sibling->edges[i];
        }
    }

    child->n++;
    sibling->n--;
    return true;
}

template<typename T, size_t B>
bool BTreeNode<T, B>::borrow_from_left(BTreeNode<T, B>& node, size_t edge) {
    // TODO
    BTreeNode<T, B>* child = node.edges[edge];
    BTreeNode<T, B>* sibling = node.edges[edge-1];

    // make space for child
    if(child->n != 0){
        for(size_t i = child->n - 1; i >= 0; i--) {
            child->keys[i+1] = child->keys[i];
            if(i == 0) break;
        }
    }

    if(child->type != NodeType::LEAF){
        for(size_t i = child->n; i >= 0; i--) {
            child->edges[i+1] = child->edges[i];
            if(i == 0) break;
        }
    }

    // borrow!
    child->keys[0] = node.keys[edge - 1];

    if(child->type != NodeType::LEAF){
        child->edges[0] = sibling->edges[sibling->n];
    }

    node.keys[edge - 1] = sibling->keys[sibling->n - 1];
    child->n ++;
    sibling->n --;
    return true;
}

template<typename T, size_t B>
bool BTreeNode<T, B>::merge_children(BTreeNode<T, B>& node, size_t idx) {
    // TODO
    BTreeNode<T, B>* child = node.edges[idx];
    BTreeNode<T, B>* sibling = node.edges[idx + 1];

    child->keys[child->n] = node.keys[idx];
    child->n++;

    for (size_t i = 0; i < sibling->n; i++) {
        size_t temp = child->n;
        child->keys[i + temp] = sibling->keys[i];
    }

    if (child->type != NodeType::LEAF) {
        for (size_t i = 0; i <= sibling->n; i++) {
            size_t temp = child->n;
            child->edges[i + temp] = sibling->edges[i];
        }
    }

    for (size_t i = idx + 1; i < node.n; i++) {
        node.keys[i - 1] = node.keys[i];
    }
    for (size_t i = idx + 2; i <= node.n; i++) {
        node.edges[i - 1] = node.edges[i];
    }
    child->n += sibling->n;
    node.n--;

    if(sibling->type == NodeType::INTERNAL){
        for(size_t i = 0; i <= sibling->n; i++){
            sibling->edges[i] = nullptr;
        }
    }
    delete sibling;
    return true;
}

template<typename T, size_t B>
T& BTreeNode<T, B>::find_rightmost_key(BTreeNode<T, B>& node) {
    if (node.type == NodeType::LEAF)
        return node.keys[node.n - 1];

    return find_rightmost_key(*node.edges[node.n]);
}

template<typename T, size_t B>
std::pair<BTreeNode<T, B>*, size_t>
BTreeNode<T, B>::search(BTreeNode<T, B>* node, const T& t) {
    if (node->type == NodeType::LEAF) {
        for (auto i = 0; i < node->keys.size(); i++)
            if (t == node->keys[i])
                return { node, i };

        return { nullptr, -1 };
    }

    size_t i;
    for (i = 0; i < node->n; i++) {
        if (t == node->keys[i])
            return { node, i };

        if (t < node->keys[i]) {
            return search(node->edges[i], t);
        }
    }

    return search(node->edges[i], t);
}

template<typename T, size_t B>
size_t BTreeNode<T, B>::depth(void) {
    if (type == NodeType::LEAF)
        return 0;

    return 1 + edges[0]->depth();
}

template <typename T, size_t B>
std::string BTree<T, B>::format(void) {
    if (!root)
        return std::string{};

    return root->format_subtree(root->depth());
}

template<typename T, size_t B>
std::string BTreeNode<T, B>::format_subtree(size_t depth) {
    std::ostringstream os;

    for (auto i = 0; i <= depth; i++)
        os << format_level(i) << '\n';

    return os.str();
}

template<typename T, size_t B>
std::string BTreeNode<T, B>::format_level(size_t level) {
    std::ostringstream os;
    auto nodes_at_level = find_nodes_at_level(level);

    for (auto node : nodes_at_level)
        os << node->format_node() << ' ';

    return os.str();
}


template<typename T, size_t B>
std::string BTreeNode<T, B>::format_node(void) {
    std::ostringstream os;

    os << '[';
    for (auto i = 0; i < n - 1; i++)
        os << keys[i] << '|';
    os << keys[n - 1];
    os << ']';

    return os.str();
}

template<typename T, size_t B>
std::vector<BTreeNode<T, B>*> BTreeNode<T, B>::find_nodes_at_level(size_t lv) {
    std::vector<BTreeNode<T, B>*> nodes;

    if (lv == 0) {
        nodes.emplace_back(this);
        return nodes;
    } else {
        std::vector<BTreeNode<T, B>*> tmp;
        for (auto i = 0; i < n + 1; i++) {
            tmp = edges[i]->find_nodes_at_level(lv - 1);
            std::copy(tmp.begin(), tmp.end(), std::back_inserter(nodes));
        }

        return nodes;
    }
}

template<typename T, size_t B>
BTreeNode<T, B>::BTreeNode() : n(0), type(NodeType::LEAF) {}

template<typename T, size_t B>
BTreeNode<T, B>::BTreeNode(const T& t) : n(1), type(NodeType::LEAF) {
    keys[0] = t;
}

/* Assume the input initializer list is sorted */
template<typename T, size_t B>
BTreeNode<T, B>::BTreeNode(std::initializer_list<T> l)
    : n(l.size()), type(NodeType::LEAF) {
    std::copy(l.begin(), l.end(), keys.begin());
}

/* Assume the input iterator is sorted. */
template<typename T, size_t B>
template<typename InputIt>
BTreeNode<T, B>::BTreeNode(InputIt begin, InputIt end)
    : n(end - begin), type(NodeType::LEAF) {
    std::copy(begin, end, keys.begin());
}

template<typename T, size_t B>
BTreeNode<T, B>::~BTreeNode() {
    if (this->type == NodeType::LEAF) {
        return;
    }

    for (auto i = 0; i < n + 1; i++){
        if (edges[i]) delete edges[i];
    }
}