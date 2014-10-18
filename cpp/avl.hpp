#include "avl.hpp"
#include <algorithm>
#include <iostream>
#include <queue>
#include <cstdio>
#include <cassert>
using namespace std;

template<typename T>
AVLTree<T>::AVLTree()
    : m_root(NULL)
{
}

template<typename T>
AVLTree<T>::~AVLTree()
{
    queue<Node*> queue;
    queue.push(m_root);
    while (queue.size() > 0)
    {
        Node* n = queue.front();
        if (n->left) queue.push(n->left);
        if (n->right) queue.push(n->right);
        delete n;
        queue.pop();
    }
}

template<typename T>
const typename AVLTree<T>::Node* AVLTree<T>::insert(const T& key)
{
    if (m_root == NULL)
    {
        m_root = new Node(key);
        return m_root;
    }
    Node* p = NULL;
    Node* n = m_root;
    while (n)
    {
        p = n;
        n = n->key < key ? n->right : n->left;
    }
    // insert the node
    Node* child = new Node(key);
    child->parent = p;
    if (p->key < key)
        p->right = child;
    else
        p->left = child;

    while (p != 0)
    {
        Node* temp = p->parent;
        balance(p);
        p = temp;
    }
    return child;
}

template<typename T>
void AVLTree<T>::balance(Node* node)
{
    fixHeight(node);
    fixCount(node);
    if (bfactor(node) == 2)
    {
        if (bfactor(node->right) < 0)
            rotateRight(node->right);
        rotateLeft(node);
    }
    else if (bfactor(node) == -2)
    {
        if (bfactor(node->left) > 0)
            rotateLeft(node->left);
        rotateRight(node);
    }
}

template<typename T>
void AVLTree<T>::rotateRight(Node* node)
{
    Node* parent = node->parent;
    Node* q = node->left;
    node->left = q->right;
    if (node->left)
        node->left->parent = node;
    node->parent = q;
    q->right = node;
    q->parent = parent;
    if (parent)
    {
        if (parent->left == node)
            parent->left = q;
        else
            parent->right = q;
    }
    else
    {
        // node was m_root, now q is the m_root
        m_root = q;
    }
    fixHeight(node);
    fixHeight(q);
    fixHeight(parent);
    fixCount(node);
    fixCount(q);
    fixCount(parent);
}

template<typename T>
void AVLTree<T>::rotateLeft(Node* node)
{
    Node* parent = node->parent;
    Node* q = node->right;
    node->right = q->left;
    if (node->right)
        node->right->parent = node;
    node->parent = q;
    q->left = node;
    q->parent = parent;
    if (parent)
    {
        if (parent->left == node)
            parent->left = q;
        else
            parent->right = q;
    }
    else
    {
        // node was m_root, now q is the m_root
        m_root = q;
    }
    fixHeight(node);
    fixHeight(q);
    fixHeight(parent);
    fixCount(node);
    fixCount(q);
    fixCount(parent);
}

template<typename T>
const typename AVLTree<T>::Node* AVLTree<T>::find(const T& key)
{
    Node* n = m_root;
    while(n)
    {
        if (n->key == key)
            break;
        else if (n->key < key)
            n = n->right;
        else
            n = n->left;
    }
    return n;
}

template<typename T>
bool AVLTree<T>::remove(const Node* node)
{
    if (!node) return false;

    if (!node->right)
    {
        // node only has left child. Based on AVL tree property, left child is a single node or NULL since AVL tree bfactor is within [-1, 0 , 1]
        Node* parent = node->parent;
        if (parent)
        {
            if (parent->left == node)
                parent->left = node->left;
            else
                parent->right = node->left;

            if (node->left)
                node->left->parent = parent;

            while (parent)
            {
                Node* temp = parent->parent;
                balance(parent);
                parent = temp;
            }
        }
        else
        {
            // node is root
            m_root = m_root->left;
        }
        delete node;
    }
    else
    {
        // find the min the right child tree
        Node* min = node->right;

        while (min->left)
        {
            min = min->left;
        }

        // took out min node

        if (min->parent->left == min)
        {
            min->parent->left = min->right;
            if (min->right)
                min->right->parent = min->parent;
        }
        else
        {
            min->parent->right = min->right;
            if (min->right)
                min->right->parent = min->parent;
        }

        Node* parent = min->parent;

        while (parent)
        {
            Node* temp = parent->parent;
            balance(parent);
            parent = temp;
        }

        // now we can replace node->key with min->key, however, due to node is type const Node*,
        // we can't directly replace it. we need a Node structure to store the copy of node paramter.
        // Here we reuse "min" node 
        replace(node, min);

        if (!min->parent) // min is m_root now
            m_root = min;

        delete node;
    }
}

template<typename T>
void AVLTree<T>::replace(const Node* a, Node* b)
{
    b->height = a->height;
    b->left = a->left;
    b->right = a->right;
    b->count = a->count;
    b->parent = a->parent;
    if (a->parent)
    {
        if (a->parent->left == a)
            a->parent->left = b;
        else
            a->parent->right = b;
    }
    if (a->left) a->left->parent = b;
    if (a->right) a->right->parent = b;
}

template<typename T>
bool AVLTree<T>::remove(const T& key)
{
    const Node* n = find(key);
    return remove(n);
}

template<typename T>
int AVLTree<T>::height(const Node* p)
{
    return p ? p->height : 0;
}

template<typename T>
int AVLTree<T>::count(const Node* p)
{
    return p ? p->count: 0;
}

template<typename T>
int AVLTree<T>::bfactor(const Node* p)
{
    return height(p->right) - height(p->left);
}

template<typename T>
void AVLTree<T>::fixHeight(Node* p)
{
    if (!p) return;
    int leftHeight = height(p->left);
    int rightHeight = height(p->right);
    p->height = std::max(leftHeight, rightHeight) + 1;
}

template<typename T>
void AVLTree<T>::fixCount(Node* p)
{
    if (!p) return;
    int leftCount = count(p->left);
    int rightCount = count(p->right);
    p->count = leftCount + rightCount + 1;
}

template<typename T>
void AVLTree<T>::breadthFirstPrint()
{
    std::queue<Node*> q;
    q.push(m_root);
    while (q.size() > 0)
    {
        Node* n = q.front();
        int indent = 0;
        Node* parent = n->parent;
        while(parent)
        {
            parent = parent->parent;
            indent++;
        }
        string indentionStr;
        if (n->parent)
        {
            if (n->parent->left == n)
                indentionStr = "..";
            else
                indentionStr = "**";
        }

        while(indent-- > 0)
        {
            cout << indentionStr; 
        }
        char buffer[256];
        sprintf(buffer, "%10f", n->key);
        cout << buffer;
        cout << "\t";
        cout << "height: " << n->height;
        cout << "\t";
        cout << "count: " << n->count;
        cout << endl;
        if (n->left)
            q.push(n->left);
        if (n->right)
            q.push(n->right);

        q.pop();
    }
}

template<typename T>
const typename AVLTree<T>::Node* AVLTree<T>::min()
{
    if (!m_root) return NULL;
    Node* n = m_root;
    while (n->left)
    {
        n = n->left;
    }
    return n;
}

template<typename T>
const typename AVLTree<T>::Node* AVLTree<T>::max()
{
    if (!m_root) return NULL;
    Node* n = m_root;
    while (n->right)
    {
        n = n->right;
    }
    return n;
}

template<typename T>
const typename AVLTree<T>::Node* AVLTree<T>::root()
{
    return m_root;
}

template<typename T>
std::vector<typename AVLTree<T>::Node*> AVLTree<T>::breadthFirstNodes()
{
    vector<Node*> res;
    if (!m_root) return res;

    queue<Node*> q;
    q.push(m_root);
    while (q.size())
    {
        Node* n = q.front();
        if (n->left) q.push(n->left);
        if (n->right) q.push(n->right);
        res.push_back(n);
        q.pop();
    }
    return res;
}

template<typename T>
void assertNode(const typename AVLTree<T>::Node* node, int height, int count, const T& key)
{
    if (node->height != height)
    {
        cerr << "Expecting height [" << height << "], but get [" << node->height << "]" << endl;
        assert(false);
    }

    if (node->count != count)
    {
        cerr << "Expecting count [" << count << "], but got [" << node->count << "]" << endl;
        assert(false);
    }

    if (node->key != key)
    {
        cerr << "Expecting key [" << key << "], but got [" << node->key << "]" << endl;
        assert(false);
    }
}

int test()
{
    AVLTree<double> t;
    for (int i = 0; i < 3; ++i)
    {
        t.insert(i);
    }

    {
        vector<AVLTree<double>::Node*> nodes = t.breadthFirstNodes();
        assertNode<double>(nodes[0], 2, 3, 1);
        assertNode<double>(nodes[1], 1, 1, 0);
        assertNode<double>(nodes[2], 1, 1, 2);
    }

    t.insert(5);
    t.insert(4);

    // test insert rotate right, then rotate left
    {
        vector<AVLTree<double>::Node*> nodes = t.breadthFirstNodes();
        assertNode<double>(nodes[0], 3, 5, 1);
        assertNode<double>(nodes[1], 1, 1, 0);
        assertNode<double>(nodes[2], 2, 3, 4);
        assertNode<double>(nodes[3], 1, 1, 2);
        assertNode<double>(nodes[4], 1, 1, 5);
    }

    t.insert(-3);
    t.insert(-2);

    // test insert rotate left, then rotate right
    {
        vector<AVLTree<double>::Node*> nodes = t.breadthFirstNodes();
        assertNode<double>(nodes[0], 3, 7, 1);
        assertNode<double>(nodes[1], 2, 3, -2);
        assertNode<double>(nodes[2], 2, 3, 4);
        assertNode<double>(nodes[3], 1, 1, -3);
        assertNode<double>(nodes[4], 1, 1, 0);
        assertNode<double>(nodes[5], 1, 1, 2);
        assertNode<double>(nodes[6], 1, 1, 5);
    }

    t.remove(4);

    t.breadthFirstPrint();

    const AVLTree<double>::Node* n1 = t.insert(-2);
    const AVLTree<double>::Node* n2 = t.insert(-2);
    cout << "breakline" << endl;
    t.breadthFirstPrint();
    t.remove(n1);
    cout << "breakline" << endl;
    t.breadthFirstPrint();
    t.remove(n2);
    cout << "breakline" << endl;
    t.breadthFirstPrint();
    
    return 0;
}

int main()
{
    if (test() != 0)
    {
        cerr << "Failed the testing" << endl;
        return 1;
    }
    cout << "Passed testing" << endl;
    return 0;
}
