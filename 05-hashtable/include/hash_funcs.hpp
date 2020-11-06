#ifndef _HASH_FUNCS_HPP
#define _HASH_FUNCS_HPP

struct DefaultHash {
    unsigned long operator()(const int& k) const {
        return k;
    }

    unsigned long operator()(const std::string& k) const
    {
        unsigned long hash = 0;
        for (const char& c: k) {
            hash += (unsigned long)c;
        }
        return hash;
    }
};

#endif
