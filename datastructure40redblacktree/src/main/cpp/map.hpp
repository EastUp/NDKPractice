//
// Created by 123 on 2020/7/16.
//

#ifndef NDKPRACTICE_MAP_HPP
#define NDKPRACTICE_MAP_HPP

#include <queue>

using namespace std;

typedef bool rb_color;
#define black false // 黑色  define 后面一定不能带分号
#define red true // 红色

template<class K, class V>
class map {
private:
    struct TreeNode;
    int count = 0;
    TreeNode *root = NULL;
public:
    map() {}

public:
    struct iterator;

    TreeNode *parent(TreeNode *pNode) {
        return pNode ? pNode->parent : NULL;
    }

    TreeNode *left(TreeNode *pNode) {
        return pNode ? pNode->left : NULL;
    }

    TreeNode *right(TreeNode *pNode) {
        return pNode ? pNode->right : NULL;
    }

    TreeNode *brother(TreeNode *pNode) {
        return left(parent(pNode)) == pNode ? right(parent(pNode)) : left(parent(pNode));
    }

    rb_color getColor(TreeNode *pNode) {
        return pNode ? pNode->color : black;
    }

    void setColor(TreeNode *pNode, rb_color color) {
        if (pNode)
            pNode->color = color;
    }

    // 左旋
    void l_rotation(TreeNode *pNode) {
        TreeNode *right = pNode->right;
        TreeNode *left = right->left;
        pNode->right = left;
        right->left = pNode;

        // 调整pNode 父亲指向
        if (!parent(pNode)) {
            root = right;
        } else if (pNode->parent->left == pNode) {
            parent(pNode)->left = right;
        } else {
            parent(pNode)->right = right;
        }

        // 修改父亲
        right->parent = parent(pNode);
        pNode->parent = right;
        if (left)
            left->parent = pNode;

    }

    // 左旋
    void r_rotation(TreeNode *pNode) {
        TreeNode *left = pNode->left;
        TreeNode *right = left->right;
        pNode->left = right;
        left->right = pNode;

        // 调整pNode 父亲指向
        if (!parent(pNode)) {
            root = left;
        } else if (pNode->parent->left == pNode) {
            parent(pNode)->left = left;
        } else {
            parent(pNode)->right = left;
        }

        // 修改父亲
        left->parent = parent(pNode);
        pNode->parent = left;
        if (right)
            right->parent = pNode;

    }


    // 调整顺序
    void solveDoubleRed(TreeNode *pNode) {
        while (pNode->parent && pNode->parent->color == red) { // 父亲是红色
            if (getColor(brother(parent(pNode))) == red) { // 情况2：叔叔是红色的 ，将叔叔和父亲染黑，然后爷爷染红；
                setColor(parent(pNode), black);
                setColor(brother(parent(pNode)), black);
                setColor(parent(parent(pNode)), red);
                pNode = parent(parent(pNode));
            } else { // 叔叔是黑色
                if (left(parent(parent(pNode))) == parent(pNode)) {
                    // 情况3：叔叔是黑色的，父亲是爷爷的左节点，且当前节点是其父节点的右孩子，将“父节点”作为“新的当前节点”，以“新的当前节点”为支点进行左旋。
                    if (right(parent(pNode)) == pNode) {
                        pNode = parent(pNode);
                        l_rotation(pNode);
                    }
                    // 情况4：叔叔是黑色的，父亲是爷爷的左节点，且当前节点是其父节点的左孩子，将“父节点”设为“黑色”，将“祖父节点”设为“红色”，以“祖父节点”为支点进行右旋；
                    setColor(parent(pNode), black);
                    setColor(parent(parent(pNode)), red);
                    r_rotation(parent(parent(pNode)));
                } else {
                    // 情况3：叔叔是黑色的，父亲是爷爷的左节点，且当前节点是其父节点的右孩子，将“父节点”作为“新的当前节点”，以“新的当前节点”为支点进行左旋。
                    if (left(parent(pNode)) == pNode) {
                        pNode = parent(pNode);
                        r_rotation(pNode);
                    }
                    // 情况4：叔叔是黑色的，父亲是爷爷的左节点，且当前节点是其父节点的左孩子，将“父节点”设为“黑色”，将“祖父节点”设为“红色”，以“祖父节点”为支点进行右旋；
                    setColor(parent(pNode), black);
                    setColor(parent(parent(pNode)), red);
                    l_rotation(parent(parent(pNode)));
                }
            }
        }

        root->color = black;
    }


    iterator insert(K key, V value) {
        if (!root) {
            root = new TreeNode(NULL, NULL, NULL, key, value, black);
            count = 1;
            return iterator(root);
        }

        // 最好我们插入红色节点，它不会违反性质 5 ，但是有可能违反性质 4
        TreeNode *node = root;
        TreeNode *parent = NULL;
        while (node) {
            parent = node;
            if (node->key > key)
                node = node->left;
            else if (node->key < key)
                node = node->right;
            else {
                node->value = value;
                return iterator(node);
            }
        }

        TreeNode *new_node = new TreeNode(NULL, NULL, parent, key, value, red);

        if (key < parent->key)
            parent->left = new_node;
        else
            parent->right = new_node;

        // 调整顺序
        solveDoubleRed(new_node);

        count++;

        return iterator(new_node);

    }

    TreeNode *findTree(K key) {
        TreeNode *node = root;
        while (node) {
            if (key == node->key) {
                return node;
            } else if (key > node->key) {
                node = node->right;
            } else {
                node = node->left;
            }
        }
        return NULL;
    }

    // 关键代码
    void solveLostBlack(TreeNode *pNode) {
        while (pNode != root && pNode->color == black) {
            if (left(parent(pNode)) == pNode) {// 当前节点是父亲节点的左节点
                TreeNode *sib = brother(pNode);
                if (getColor(sib) == red) { // 想办法把兄弟节点变成黑色  情况 1
                    setColor(sib, black);
                    setColor(parent(pNode), red);
                    l_rotation(parent(pNode));
                    sib = brother(pNode);
                }
                if (getColor(left(sib)) == black && getColor(right(sib)) == black) { // 情况2
                    setColor(sib, red);
                    pNode = parent(pNode);
                } else {
                    // 情况 3
                    if (getColor(right(sib)) == black) {
                        setColor(sib, red);
                        setColor(left(sib), black);
                        r_rotation(sib);
                        sib = brother(pNode);
                    }
                    // 情况 4
                    setColor(sib, getColor(parent(pNode)));
                    setColor(parent(pNode), black);
                    setColor(right(sib), black);
                    l_rotation(parent(pNode));

                    // 相当于两行代码 ：break ，将根节点染黑
                    pNode = root;
                }
            } else {// 当前节点是父亲节点的右节点
                TreeNode *sib = brother(pNode);
                if (getColor(sib) == red) { // 想办法把兄弟节点变成黑色
                    setColor(sib, black);
                    setColor(parent(pNode), red);
                    r_rotation(parent(pNode));
                    sib = brother(pNode);
                }

                if (getColor(left(sib)) == black && getColor(right(sib)) == black) {
                    setColor(sib, red);
                    pNode = parent(pNode);
                } else {
                    // 情况 3  远侄子是 黑
                    if (getColor(left(sib)) == black) {
                        setColor(sib, red);
                        setColor(right(sib), black);
                        l_rotation(sib);
                        sib = brother(pNode);
                    }
                    // 情况 4
                    setColor(sib, getColor(parent(pNode)));
                    setColor(parent(pNode), black);
                    setColor(left(sib), black);
                    r_rotation(parent(pNode));

                    pNode = root;
                }
            }
        }
        // 当遇到一个红色节点，把红色节点染黑即可
        pNode->color = black;
    }

    bool remove(K key) {
        TreeNode *current = findTree(key);

        if (current == NULL) {
            return false;
        }

        if (current->left != NULL && current->right != NULL) {
            TreeNode *successor = current->successor();
            current->key = successor->key;
            current->value = successor->value;
            current = successor;
        }
        TreeNode *replace = current->left ? current->left : current->right;

        if (replace != NULL) {
            // 父亲  , current->parent 会不会有空的情况？
            if (current->parent == NULL) { // 根节点
                root = replace;
            } else if (current->parent->left == current) {
                current->parent->left = replace;
            } else {
                current->parent->right = replace;
            }

            replace->parent = current->parent;

            if (current->color == black) {
                solveLostBlack(replace);
            }

            delete (current);
        } else if (current->parent == NULL) { // 删除根节点的情况
            delete (root);
            root = NULL;
        } else {
            // 为什么不先移除，是因为在修正的时候需要去获取叔叔和侄子节点来分情况判断
            if (current->color == black) {
                solveLostBlack(current);
            }
            // 再来移除
            if (current->parent) {
                if (current->parent->left == current) {
                    current->parent->left = NULL;
                } else {
                    current->parent->right = NULL;
                }
            }
            delete (current);
        }

        count--;
        return true;
    }

    int size() {
        return count;
    }

    bool isEmpty() {
        return count == 0;
    };

    void levelTraverse(void(*log)(K, V, bool)) {
        if (root == NULL)
            return;

        TreeNode *node = root;
        queue<TreeNode *> nodes;
        nodes.push(node);
        while (!nodes.empty()) {
            node = nodes.front();
            log(node->key, node->value, node->color);
            nodes.pop();

            if (node->left) {
                nodes.push(node->left);
            }

            if (node->right) {
                nodes.push(node->right);
            }
        }
    }
};

template<class K, class V>
struct map<K, V>::TreeNode {
public:
    TreeNode *left;
    TreeNode *right;
    TreeNode *parent;
    K key;
    V value;
    // 颜色
    rb_color color;
public:
    TreeNode(TreeNode *left, TreeNode *right, TreeNode *parent, K key, V value, rb_color color)
            : left(left), right(right), parent(parent), key(key), value(value), color(color) {}

    // 找到前驱
    TreeNode *precursor() {
        if (left) {
            TreeNode *node = left;
            while (node->right) {
                node = node->right;
            }
            return node;
        } else {
            if (right) { // 左子树为NULL，右子树不为NULL
                return parent;
            } else { // 左右子树都为 NULL 的情况
                if (parent->right = this) {
                    return parent;
                } else {
                    TreeNode *myp = this;
                    while (myp) {
                        if (myp->parent->left = myp)// 在左子树
                            myp = myp->parent;
                        else { // 在根节点的右子树中
                            return myp->parent;
                        }
                    }
                    return myp;

                }
            }
        }

    }

    // 找后驱
    TreeNode *successor() {
        if (right) {
            TreeNode *node = right;
            while (node->left) {
                node = node->left;
            }
            return node;
        } else {
            if (left) { // 左子树不为NULL，右子树为NULL
                return parent;
            } else { // 左右子树都为 NULL 的情况
                if (parent->left = this) {
                    return parent;
                } else {
                    TreeNode *myp = this;
                    while (myp) {
                        if (myp->parent->right = myp)// 在右子树
                            myp = myp->parent;
                        else { // 在根节点的左子树中
                            return myp->parent;
                        }
                    }
                    return myp;

                }
            }
        }
    }

};

template<class K, class V>
struct map<K, V>::iterator {
private:
    TreeNode *node;
public:
    iterator(TreeNode *node) : node(node) {}

    // 找前驱
    iterator &operator--() {
        node = node->precursor();
        return *this;
    }

    // 找后继
    iterator &operator++() {
        node = node->successor();
        return *this;
    }

    V operator*() {
        return node->value;
    }

};


#endif //NDKPRACTICE_MAP_HPP
