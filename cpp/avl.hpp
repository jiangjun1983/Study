#include <vector>

template<typename T>
class AVLTree
{
    public:
        AVLTree();
        ~AVLTree();
        struct Node
        {
            T key;
            int height;
            int count;
            Node* left;
            Node* right;
            Node* parent;
            Node(const T& k) { key = k; height = 1; count = 1; left = 0; right = 0; parent = 0; }
        };
        std::vector<Node*> breadthFirstNodes();
        void breadthFirstPrint();
        const Node* insert(const T& key);
        bool remove(const Node* node);
        bool remove(const T& key);

    private:
        void balance(Node* node);
        int bfactor(const Node* p);
        int count(const Node* p);
        const Node* find(const T& key);
        void fixCount(Node* p);
        void fixHeight(Node* p);
        int height(const Node* p);
        const Node* max();
        const Node* min();
        void replace(const Node* a, Node* b);
        const Node* root();
        void rotateLeft(Node* node);
        void rotateRight(Node* node);
        Node* m_root;
};

