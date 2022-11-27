#include <iostream>

namespace automaton {

    class Node {
    public:
        inline Node() { }
    };
    
    class RawNode : public Node {
    public:
        inline RawNode(unsigned int value): _value(value) { } 

    private:
        const unsigned int _value;
    };

    class CatNode : Node {
    public:
        inline CatNode(Node* left, Node* right): _left(left), _right(right) { }

    private:
        Node* _left, _right;
    }

    // class
}