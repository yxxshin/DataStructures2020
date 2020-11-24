#ifndef __RBTREE_H_
#define __RBTREE_H_

#include <algorithm>
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <string>
#include <sstream>
#include <valarray>
#include <utility>
#include <type_traits>
#include <iostream>
#include <optional>
#include <fstream>

static size_t null_count = 0;

template <typename T>
struct RBNode;

using color_t = bool;
constexpr static color_t RED = false;
constexpr static color_t BLK = true;

/* This is an abstraction for search-path. For debugging purpose */
struct Path;

template<typename T>
struct RBTree {
    std::unique_ptr<RBNode<T>> root = nullptr;

    ~RBTree() = default;

    bool insert(const T&);
    void remove_max();
    void remove_min();
    void remove(const T&);

    const std::optional<T> leftmost_key();
    const std::optional<T> rightmost_key();

    void traverse_inorder(std::function<void(RBNode<T>*)>);

    bool contains(const T& t);

    std::unordered_map<Path, const RBNode<T>&> collect_all_leaves() const;

    std::string format_graphviz();
};

template<typename T>
struct RBNode {
    T key;
    color_t color = RED;
    std::unique_ptr<RBNode> left = nullptr;
    std::unique_ptr<RBNode> right = nullptr;

    RBNode(const T& t);
    ~RBNode() = default;

    bool is_leaf();

    void flip_color();
    static RBNode* rotate_right(std::unique_ptr<RBNode>&);
    static RBNode* rotate_left(std::unique_ptr<RBNode>&);
    static bool is_red(const std::unique_ptr<RBNode>&);

    static RBNode* move_red_right(std::unique_ptr<RBNode>&);
    static RBNode* move_red_left(std::unique_ptr<RBNode>&);

    static RBNode* remove_max(std::unique_ptr<RBNode>&);
    static RBNode* remove_min(std::unique_ptr<RBNode>&);
    static RBNode* remove(std::unique_ptr<RBNode>&, const T&);

    static RBNode* insert(std::unique_ptr<RBNode>&, const T&);
    std::pair<RBNode<T>*, Path> search(const T&, Path);

    void traverse_inorder(std::function<void(RBNode*)>);
    size_t get_max_depth();

    std::unordered_map<size_t, const RBNode&> get_nodes_at_level(size_t);
    void _get_nodes_at_level(size_t,
                             std::unordered_map<size_t, const RBNode&>&,
                             Path);
    std::string format_level(size_t);

    std::unordered_map<Path, const RBNode<T>&> collect_all_leaves(void);
    void _collect_all_leaves(std::unordered_map<Path, const RBNode<T>&>&, Path);

    static RBNode* fix_up(std::unique_ptr<RBNode>&);

    std::string format_graphviz();

    bool contains(const T& t);

    const T& leftmost_key();
    const T& rightmost_key();
};

template<typename T>
bool RBTree<T>::insert(const T& t) {
    if (!root) {
        root = std::make_unique<RBNode<T>>(t);
        root->color = BLK;
        return true;
    }

    root.reset(RBNode<T>::insert(root, t));

    /* Change root to black. Won't affect the balance */
    if (root->color == RED)
        root->color = BLK;

    return true;
}

template<typename T>
void RBTree<T>::remove_max() {
    root.reset(RBNode<T>::remove_max(root));

    if (root)
        root->color = BLK;
}

template<typename T>
void RBTree<T>::remove_min() {
    root.reset(RBNode<T>::remove_min(root));

    if (root)
        root->color = BLK;
}

template<typename T>
void RBTree<T>::remove(const T& t) {
    root.reset(RBNode<T>::remove(root, t));

    if (root)
        root->color = BLK;
}

template <typename T>
const std::optional<T> RBTree<T>::leftmost_key() {
    if (!root)
        return std::nullopt;

    return root->leftmost_key();
}

template <typename T>
const std::optional<T> RBTree<T>::rightmost_key() {
    if (!root)
        return std::nullopt;

    return root->rightmost_key();
}

template <typename T>
bool RBTree<T>::contains(const T& t) {
    if (!root)
        return false;

    return root->contains(t);
}

/* This is an abstraction for search-path. For debugging purpose */
struct Path {
    static constexpr size_t LEFT = 0;
    static constexpr size_t RIGHT = 1;
    size_t p_;
    size_t len_;
    size_t num_black_;

    Path() : p_(0), len_(0), num_black_(0) {}
    Path(size_t p, size_t len, size_t num_black)
        : p_(p), len_(len), num_black_(num_black) {}

    static Path down_left(Path sp, color_t color) {
        auto num_black = color == BLK ? sp.num_black_ + 1 : sp.num_black_;
        return Path { (sp.p_ << 1) + LEFT, sp.len_ + 1, num_black };
    }

    static Path down_right(Path sp, color_t color) {
        auto num_black = color == BLK ? sp.num_black_ + 1 : sp.num_black_;
        return Path { (sp.p_ << 1) + RIGHT, sp.len_ + 1, num_black };
    }

    /* For hashtable */
    bool operator==(const Path& other) const {
        return p_ == other.p_;
    }
};

namespace std {
    template<>
    struct hash<Path> {
        std::size_t operator()(const Path& p) const {
            return std::hash<size_t>()(p.p_);
        }
    };
}

template<typename T>
bool RBNode<T>::is_leaf() {
    return !left && !right;
}

template<typename T>
bool RBNode<T>::is_red(const std::unique_ptr<RBNode<T>>& n) {
    return n && n->color == RED;
}

template<typename T>
void RBNode<T>::flip_color() {
    color = !this->color;

    if (left)
        left->color = !left->color;

    if (right)
        right->color = !right->color;
}

template<typename T>
RBNode<T>* RBNode<T>::rotate_right(std::unique_ptr<RBNode<T>>& n) {
    // TODO
    std::unique_ptr<RBNode<T>> temp = std::move(n->left);
    n->left = std::move(temp->right);
    temp->right = std::move(n);
    temp->color = temp->right->color;
    temp->right->color = RED;
    return temp.release();
}

template<typename T>
RBNode<T>* RBNode<T>::rotate_left(std::unique_ptr<RBNode<T>>& n) {
    // TODO
    std::unique_ptr<RBNode<T>> temp = std::move(n->right);
    n->right = std::move(temp->left);
    temp->left = std::move(n);
    temp->color = temp->left->color;
    temp->left->color = RED;
    return temp.release();
}

template<typename T>
RBNode<T>* RBNode<T>::insert(std::unique_ptr<RBNode<T>>& n, const T& t) {
    // TODO
    
    // normal insertion to (any) BST
    if(n == nullptr) {
        n = std::make_unique<RBNode<T>>(t);
        return n.release();
    }
    else if (t < n->key) {
        std::unique_ptr<RBNode<T>> tmp(insert(n->left, t));
        n->left = std::move(tmp);
    }
    else {  // t > n->key
        std::unique_ptr<RBNode<T>> tmp(insert(n->right, t));
        n->right = std::move(tmp);
    }
    
    // Case 1 : right child is RED but left child is BLK or doesn't exist
    if(n->right && is_red(n->right) && !is_red(n->left)){
        // rotate_left
        std::unique_ptr<RBNode<T>> tmp(rotate_left(n));
        n = std::move(tmp);
    }

    // Case 2 : left child and left grand child is RED
    if(n->left && n->left->left && is_red(n->left) && is_red(n->left->left)){
        // rotate_right
        std::unique_ptr<RBNode<T>> tmp(rotate_right(n));
        n = std::move(tmp);
    }

    // Case 3 : both left and right child are RED
    if(n->left && n->right && is_red(n->left) && is_red(n->right)){
        // flip_color
        n->flip_color();
    }

    return n.release();
}

template<typename T>
std::pair<RBNode<T>*, Path> RBNode<T>::search(const T& t, Path sp) {
    if (t > key) {
        return right ?
            right->search(t, Path::down_right(sp, color)) :
            std::pair<RBNode<T>*, Path>{ nullptr, {} };
    } else if (t < key) {
        return left ?
            left->search(t, Path::down_left(sp, color)) :
            std::pair<RBNode<T>*, Path>{ nullptr, {} };
    } else {
        return { this, sp };
    }
}

template<typename T>
RBNode<T>* RBNode<T>::fix_up(std::unique_ptr<RBNode<T>>& n) {
    // TODO

    // rotate_left right-leaning REDs
    if(n->right && is_red(n->right)) {
        std::unique_ptr<RBNode<T>> tmp(rotate_left(n));
        n = std::move(tmp);
    }
    // rotate_right RED-RED pairs
    if(n->left && n->left->left && is_red(n->left) && is_red(n->left->left)) {
        std::unique_ptr<RBNode<T>> tmp(rotate_right(n));
        n = std::move(tmp);
    }
    // flip_color
    if(n->left && n->right && is_red(n->left) && is_red(n->right)){
        n->flip_color();
    }
    return n.release();
}

template<typename T>
RBNode<T>* RBNode<T>::move_red_right(std::unique_ptr<RBNode<T>>& n) {
    // TODO
    // rotate RED links to the right
    // borrow from sibling if necessary: when n->right and n->right->left are both BLK
    // this two cases depend on the color of n->left->left
    n->flip_color();

    if(n->left && is_red(n->left->left)){
        std::unique_ptr<RBNode<T>> tmp(rotate_right(n));
        n = std::move(tmp);
        n->flip_color();
    }
    return n.release();
}

template<typename T>
RBNode<T>* RBNode<T>::remove_max(std::unique_ptr<RBNode<T>>& n) {
    // TODO
    
    // lean RED child to right
    if( n->left && is_red(n->left) ){
        std::unique_ptr<RBNode<T>> tmp(rotate_right(n));
        n = std::move(tmp);
    }
    
    // remove node on bottom level ( must be RED )
    if( n->right == nullptr ){
        return nullptr;
    }

    // borrow from sibling if necessary
    if( n->right && !is_red(n->right) && !is_red(n->right->left) ) {
        std::unique_ptr<RBNode<T>> tmp(move_red_right(n));
        n = std::move(tmp);
    }

    // move down one level
    std::unique_ptr<RBNode<T>> temp(remove_max(n->right));
    n->right = std::move(temp);

    return fix_up(n);
}

template<typename T>
RBNode<T>* RBNode<T>::move_red_left(std::unique_ptr<RBNode<T>>& n) {
    // TODO
    n->flip_color();
    if (n->right->left && is_red(n->right->left)) {
        std::unique_ptr<RBNode<T>> tmp(rotate_right(n->right));
        n->right = std::move(tmp);
        std::unique_ptr<RBNode<T>> temp(rotate_left(n));
        n = std::move(temp);
        n->flip_color();
    }
    return n.release();   
}

template<typename T>
RBNode<T>* RBNode<T>::remove_min(std::unique_ptr<RBNode<T>>& n) {
    // TODO
    // remove node on bottom level (n must be RED)
    if(n->left == nullptr) {
        return nullptr;
    }

    // push red link down if necessary
    if( n->left && !is_red(n->left) && !is_red(n->left->left)) {
        std::unique_ptr<RBNode<T>> tmp(move_red_left(n));
        n = std::move(tmp);
    }

    // move down one level
    std::unique_ptr<RBNode<T>> temp(remove_min(n->left));
    n->left = std::move(temp);

    // fix right-leaning RED links, and flip_color() on the way up
    return fix_up(n);
}

template<typename T>
RBNode<T>* RBNode<T>::remove(std::unique_ptr<RBNode<T>>& n, const T& t) {
    // TODO
    // LEFT
    if(t < n->key) {
        // push RED right if necessary
        if( n->left && !is_red(n->left) && !is_red(n->left->left)) {
            std::unique_ptr<RBNode<T>> tmp(move_red_left(n));
            n = std::move(tmp);
        }
        // move down
        std::unique_ptr<RBNode<T>> temp(remove(n->left, t));
        n->left = std::move(temp);
        
    }
    // RIGHT or EQUAL
    else {
        // rotate to push RED right
        if(n->left && is_red(n->left)) {
            std::unique_ptr<RBNode<T>> tmp(rotate_right(n));
            n = std::move(tmp);
        }
        // EQUAL at bottom : delete node
        if( t == n->key && n->right == nullptr) {
            return nullptr;
        }
        // push RED right if necessary
        if(n->right && !is_red(n->right) && !is_red(n->right->left)) {
            std::unique_ptr<RBNode<T>> tmp(move_red_right(n));
            n = std::move(tmp);
        }
        // EQUAL not at bottom
        if( t == n->key ) {
            n->key = n->right->leftmost_key();
            std::unique_ptr<RBNode<T>> temp(remove_min(n->right));
            n->right = std::move(temp);
        }
        else {
            // move down
            std::unique_ptr<RBNode<T>> tmp(remove(n->right, t));
            n->right = std::move(tmp);
        }
    }
    // fix on the way up
    return fix_up(n);
}

template<typename T>
const T& RBNode<T>::leftmost_key() {
    if (!left)
        return key;

    return left->leftmost_key();
}

template<typename T>
const T& RBNode<T>::rightmost_key() {
    if (!right)
        return key;

    return right->rightmost_key();
}

template<typename T>
bool RBNode<T>::contains(const T& t) {
    if (t == key)
        return true;
    if (t < key)
        return left && left->contains(t);
    else /* t > key */
        return right && right->contains(t);
}

template<typename T>
void RBTree<T>::traverse_inorder(std::function<void(RBNode<T>*)> f) {
    if (root)
        root->traverse_inorder(f);
}

template<typename T>
void RBNode<T>::traverse_inorder(std::function<void(RBNode*)> f) {
    if (left)
        left->traverse_inorder(f);

    f(this);

    if (right)
        right->traverse_inorder(f);
}

template<typename T>
size_t RBNode<T>::get_max_depth() {
    if (is_leaf())
        return 1;

    if (left && right)
        return 1 + std::max(left->get_max_depth(), right->get_max_depth());
    else if (left && !right)
        return 1 + left->get_max_depth();
    else
        return 1 + right->get_max_depth();
}

template<typename T>
std::unordered_map<size_t, const RBNode<T>&>
RBNode<T>::get_nodes_at_level(size_t lvl) {
    std::unordered_map<size_t, const RBNode<T>&> ns;

    _get_nodes_at_level(lvl, ns, Path{});

    return ns;
}

template<typename T>
void RBNode<T>::_get_nodes_at_level(
    size_t lvl, std::unordered_map<size_t, const RBNode<T>&>& ns, Path sp) {
    if (lvl == 0) {
        ns.insert({ sp.p_, std::as_const(*this) });
        return;
    }

    if (left)
        left->_get_nodes_at_level(lvl - 1, ns, Path::down_left(sp, color));

    if (right)
        right->_get_nodes_at_level(lvl - 1, ns, Path::down_right(sp, color));
}

template<typename T>
std::string RBNode<T>::format_level(size_t lvl) {
    static const std::string red = "\033[1;31m";
    static const std::string reset = "\033[0m";

    std::ostringstream os;

    auto ns = get_nodes_at_level(lvl);
    for (auto i = 0; i < (1 << lvl); i++) {
        const auto& pn = ns.find(i);
        if (pn != ns.end()) {
            const auto& n = pn->second;
            if (n.color == RED) os << red << n.key << reset << ' ';
            else os << n.key << ' ';
        } else {
            os << "- ";
        }
    }

    return os.str();
}

template<typename T>
std::unordered_map<Path, const RBNode<T>&>
RBTree<T>::collect_all_leaves() const {
    std::unordered_map<Path, const RBNode<T>&> all_leaves;

    if (!root)
        return all_leaves;

    return root->collect_all_leaves();
}

template<typename T>
std::unordered_map<Path, const RBNode<T>&>
RBNode<T>::collect_all_leaves() {
    std::unordered_map<Path, const RBNode<T>&> all_leaves;

    _collect_all_leaves(all_leaves, Path {});

    return all_leaves;
}

template<typename T>
void
RBNode<T>::_collect_all_leaves(std::unordered_map<Path,const RBNode<T>&>& ls,
                               Path p) {
    if (is_leaf())
        ls.insert({ p, std::as_const(*this) });

    if (left)
        left->_collect_all_leaves(ls, Path::down_left(p, left->color));

    if (right)
        right->_collect_all_leaves(ls, Path::down_right(p, right->color));
}

template<typename T>
RBNode<T>::RBNode(const T& t)
    : key(t), color(RED), left(nullptr), right(nullptr) {}

template<typename T>
std::ostream& operator<<(std::ostream& os, const RBTree<T>& rbtree) {
    if (!rbtree.root)
        return os;

    auto max_depth = rbtree.root->get_max_depth();
    for (auto i = 0; i < max_depth; i++)
        os << rbtree.root->format_level(i) << '\n';

    return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const RBNode<T>& rbnode) {
    auto max_depth = rbnode.get_max_depth();
    for (auto i = 0; i < max_depth; i++)
        os << rbnode.format_level(i) << '\n';

    return os;
}

template <typename T>
std::string RBTree<T>::format_graphviz() {
    std::ostringstream os;
    if(!root) {
        os << "None\n";
        return os.str();
    }
    os << "graph RBTree {\n"
       << "\tnode [fontname=\"Arial\"];\n"
       << root->format_graphviz()
       << "}\n";

    return os.str();
}

template <typename T>
std::string RBNode<T>::format_graphviz() {
    std::ostringstream os;

    if(left) {
        os << '\t' << key << " -- " << left->key;
        if(is_red(left))
            os << "[color=red,penwidth=3.0]";
        else
            os << "[penwidth=3.0]";
        os << ";\n" << left->format_graphviz();
    } else {
        os << "\tnull" << null_count << "[shape=point];\n"
           << "\t" << key << " -- " << "null" << null_count++ << ";\n";
    }

    if(right) {
        os << '\t' << key << " -- " << right->key;
        if(is_red(right))
            os << "[color=red,penwidth=3.0]";
        else
            os << "[penwidth=3.0]";
        os << ";\n" << right->format_graphviz();
    } else {
        os << "\tnull" << null_count << "[shape=point];\n"
           << "\t" << key << " -- " << "null" << null_count++ << ";\n";
    }

    return os.str();
}

#endif // __RBTREE_H_
