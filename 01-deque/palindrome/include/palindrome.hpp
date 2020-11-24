#include <string>

#include "deque.hpp"

template<typename Deque>
class Palindrome {
public:
    bool is_palindrome(const std::string&);
    void reset_deque();

private:
    Deque deque;
};

template<typename Deque>
bool Palindrome<Deque>::is_palindrome(const std::string& s1) {
    // TODO
    for(int i = 0; i < s1.size(); i++){
        deque.push_back(s1[i]);
    }

    while( deque.size() > 1 ){
        std::optional<char> front = deque.remove_front();
        std::optional<char> back = deque.remove_back();
        if(front != back)
            return false;
    }
    return true;
}

template<typename Deque>
void Palindrome<Deque>::reset_deque() {
    while (!deque.empty())
        deque.remove_front();
}
