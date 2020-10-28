#include "deque.hpp"
#include "palindrome.hpp"

int main(int argc, char *argv[]) {
    Palindrome<ArrayDeque<char>> p;

    std::string s{"able was I ere I saw elba"};

    std::cout << p.is_palindrome(s) << '\n';
    return 0;
}
