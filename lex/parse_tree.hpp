#include "automaton/Node.hpp"

namespace lex {

    class ParseTree {
    public:
        ParseTree(const std::string& input);

    public:
        bool construct();

    private:
        const std::string _input;

        unsigned int _cur = 0;
        unsigned int _ahead = 0;
        automaton::Node* _treeRoot = nullptr;
    };
}