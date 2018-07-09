/**
 * Quick balanced binary search tree
 *
 * @version 2018-05-16_001
 * @author  Robert Altnoeder (r.altnoeder@gmx.net)
 *
 * Copyright (C) 2012 - 2018 Robert ALTNOEDER
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted provided that
 * the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef QTREE_H
#define	QTREE_H

#include <cstddef>
#include <new>

#include <dsaext.h>

template<typename K, typename V>
class QTree : public dsaext::Map<K, V>
{
  private:
    enum qtree_dir
    {
        DIR_LESS,
        DIR_GREATER
    };

  public:
    typedef int (*compare_func)(const K* key, const K* other);

    class Node : dsaext::Map<K, V>::Node
    {
        friend class QTree;

      private:
        const K*    key     {nullptr};
        const V*    value   {nullptr};
        Node*       less    {nullptr};
        Node*       greater {nullptr};
        Node*       parent  {nullptr};
        int         balance {0};

      public:
        Node(const K* key_ptr, const V* value_ptr)
        {
            key = key_ptr;
            value = value_ptr;
        }

        Node(const Node& orig):
            key(orig.key),
            value(orig.value)
        {
        }

        Node& operator=(const Node& orig)
        {
            if (this != &orig)
            {
                key = orig.key;
                value = orig.value;
            }
            return *this;
        }

        Node(Node&& orig) = default;
        Node& operator=(Node&& orig) = default;

        virtual ~Node() = default;

        virtual K* get_key() const
        {
            return const_cast<K*> (key);
        }

        virtual V* get_value() const
        {
            return const_cast<V*> (value);
        }

        virtual Node* successor() const
        {
            const Node* ret_node = this;

            if (ret_node->greater != nullptr)
            {
                ret_node = ret_node->greater;
                while (ret_node->less != nullptr)
                {
                    ret_node = ret_node->less;
                }
            }
            else
            {
                do
                {
                    if (ret_node->parent != nullptr)
                    {
                        if (ret_node->parent->less == ret_node)
                        {
                            ret_node = ret_node->parent;
                            break;
                        }
                    }
                    ret_node = ret_node->parent;
                }
                while (ret_node != nullptr);
            }

            return const_cast<Node*> (ret_node);
        }

        virtual Node* predecessor() const
        {
            const Node* ret_node = this;

            if (ret_node->less != nullptr)
            {
                ret_node = ret_node->less;
                while (ret_node->greater != nullptr)
                {
                    ret_node = ret_node->greater;
                }
            }
            else
            {
                do
                {
                    if (ret_node->parent != nullptr)
                    {
                        if (ret_node->parent->greater == ret_node)
                        {
                            ret_node = ret_node->parent;
                            break;
                        }
                    }
                    ret_node = ret_node->parent;
                }
                while (ret_node != nullptr);
            }

            return const_cast<Node*> (ret_node);
        }

        virtual void reuse()
        {
            key     = nullptr;
            value   = nullptr;
            less    = nullptr;
            greater = nullptr;
            parent  = nullptr;
            balance = 0;
        }
    };

  protected:
    template<typename T>
    class BaseIterator : public dsaext::QIterator<T>
    {
      public:
        BaseIterator(QTree<K, V>& qtree_ref):
            qtree_obj(qtree_ref)
        {
            if (qtree_obj.root != nullptr)
            {
                iter_node = qtree_obj.root;
                while (iter_node->less != nullptr)
                {
                    iter_node = iter_node->less;
                }
            }
            else
            {
                iter_node = nullptr;
            }
        }

        BaseIterator(QTree<K, V>& qtree_ref, Node& start_node):
            qtree_obj(qtree_ref),
            iter_node(&start_node)
        {
        }

        BaseIterator(const BaseIterator& orig) = default;
        BaseIterator& operator=(const BaseIterator& orig) = default;
        BaseIterator(BaseIterator&& orig) = default;
        BaseIterator& operator=(BaseIterator&& orig) = default;

        virtual ~BaseIterator()
        {
        }

        virtual T* next() = 0;

        virtual bool has_next() const
        {
            return iter_node != nullptr;
        }

        virtual size_t get_size() const
        {
            return qtree_obj.size;
        }

      protected:
        virtual Node* next_node()
        {
            Node* ret_node = iter_node;

            if (ret_node != nullptr)
            {
                if (iter_node->greater != nullptr)
                {
                    iter_node = iter_node->greater;
                    while (iter_node->less != nullptr)
                    {
                        iter_node = iter_node->less;
                    }
                }
                else
                {
                    do
                    {
                        if (iter_node->parent != nullptr)
                        {
                            if (iter_node->parent->less == iter_node)
                            {
                                iter_node = iter_node->parent;
                                break;
                            }
                        }
                        iter_node = iter_node->parent;
                    }
                    while (iter_node != nullptr);
                }
            }

            return ret_node;
        }
      private:
        QTree& qtree_obj;
        Node*  iter_node {nullptr};
    };

    template<typename T>
    class BaseReverseIterator : public dsaext::QIterator<T>
    {
      public:
        BaseReverseIterator(QTree<K, V>& qtree_ref):
            qtree_obj(qtree_ref)
        {
            if (qtree_obj.root != nullptr)
            {
                iter_node = qtree_obj.root;
                while (iter_node->greater != nullptr)
                {
                    iter_node = iter_node->greater;
                }
            }
            else
            {
                iter_node = nullptr;
            }
        }

        BaseReverseIterator(QTree<K, V>& qtree_ref, Node& start_node):
            qtree_obj(qtree_ref),
            iter_node(&start_node)
        {
        }

        BaseReverseIterator(const BaseReverseIterator& orig) = default;
        BaseReverseIterator& operator=(const BaseReverseIterator& orig) = default;
        BaseReverseIterator(BaseReverseIterator&& orig) = default;
        BaseReverseIterator& operator=(BaseReverseIterator&& orig) = default;

        virtual ~BaseReverseIterator()
        {
        }

        virtual T* next() = 0;

        virtual bool has_next() const
        {
            return iter_node != nullptr;
        }

        virtual size_t get_size() const
        {
            return qtree_obj.size;
        }

      protected:
        virtual Node* next_node()
        {
            Node* ret_node = iter_node;

            if (ret_node != nullptr)
            {
                if (iter_node->less != nullptr)
                {
                    iter_node = iter_node->less;
                    while (iter_node->greater != nullptr)
                    {
                        iter_node = iter_node->greater;
                    }
                }
                else
                {
                    do
                    {
                        if (iter_node->parent != nullptr)
                        {
                            if (iter_node->parent->greater == iter_node)
                            {
                                iter_node = iter_node->parent;
                                break;
                            }
                        }
                        iter_node = iter_node->parent;
                    }
                    while (iter_node != nullptr);
                }
            }

            return ret_node;
        }
      private:
        QTree& qtree_obj;
        Node*  iter_node {nullptr};
    };

  public:
    class KeysIterator : public BaseIterator<K>
    {
      public:
        KeysIterator(QTree<K, V>& qtree_ref):
            BaseIterator<K>::BaseIterator(qtree_ref)
        {
        }

        KeysIterator(QTree<K, V>& qtree_ref, Node& start_node):
            BaseIterator<K>::BaseIterator(qtree_ref, start_node)
        {
        }

        KeysIterator(const KeysIterator& orig) = default;
        KeysIterator& operator=(const KeysIterator& orig) = default;
        KeysIterator(KeysIterator&& orig) = default;
        KeysIterator& operator=(KeysIterator&& orig) = default;

        virtual ~KeysIterator()
        {
        }

        virtual K* next()
        {
            const K* iter_key {nullptr};
            Node* node = BaseIterator<K>::next_node();
            if (node != nullptr)
            {
                iter_key = node->key;
            }
            return const_cast<K*> (iter_key);
        }
    };

    class KeysReverseIterator : public BaseReverseIterator<K>
    {
      public:
        KeysReverseIterator(QTree<K, V>& qtree_ref):
            BaseReverseIterator<K>::BaseReverseIterator(qtree_ref)
        {
        }

        KeysReverseIterator(QTree<K, V>& qtree_ref, Node& start_node):
            BaseReverseIterator<K>::BaseReverseIterator(qtree_ref, start_node)
        {
        }

        KeysReverseIterator(const KeysReverseIterator& orig) = default;
        KeysReverseIterator& operator=(const KeysReverseIterator& orig) = default;
        KeysReverseIterator(KeysReverseIterator&& orig) = default;
        KeysReverseIterator& operator=(KeysReverseIterator&& orig) = default;

        virtual ~KeysReverseIterator()
        {
        }

        virtual K* next()
        {
            const K* iter_key {nullptr};
            Node* node = BaseReverseIterator<K>::next_node();
            if (node != nullptr)
            {
                iter_key = node->key;
            }
            return const_cast<K*> (iter_key);
        }
    };

    class ValuesIterator : public BaseIterator<V>
    {
      public:
        ValuesIterator(QTree<K, V>& qtree_ref):
            BaseIterator<V>::BaseIterator(qtree_ref)
        {
        }

        ValuesIterator(QTree<K, V>& qtree_ref, Node& start_node):
            BaseIterator<V>::BaseIterator(qtree_ref, start_node)
        {
        }

        ValuesIterator(const ValuesIterator& orig) = default;
        ValuesIterator& operator=(const ValuesIterator& orig) = default;
        ValuesIterator(ValuesIterator&& orig) = default;
        ValuesIterator& operator=(ValuesIterator&& orig) = default;

        virtual ~ValuesIterator()
        {
        }

        virtual V* next()
        {
            const V* iter_value {nullptr};
            Node* node = BaseIterator<V>::next_node();
            if (node != nullptr)
            {
                iter_value = node->value;
            }
            return const_cast<V*> (iter_value);
        }
    };

    class ValuesReverseIterator : public BaseReverseIterator<V>
    {
      public:
        ValuesReverseIterator(QTree<K, V>& qtree_ref):
            BaseReverseIterator<V>::BaseReverseIterator(qtree_ref)
        {
        }

        ValuesReverseIterator(QTree<K, V>& qtree_ref, Node& start_node):
            BaseReverseIterator<V>::BaseReverseIterator(qtree_ref, start_node)
        {
        }

        ValuesReverseIterator(const ValuesReverseIterator& orig) = default;
        ValuesReverseIterator& operator=(const ValuesReverseIterator& orig) = default;
        ValuesReverseIterator(ValuesReverseIterator&& orig) = default;
        ValuesReverseIterator& operator=(ValuesReverseIterator&& orig) = default;

        virtual ~ValuesReverseIterator()
        {
        }

        virtual V* next()
        {
            const V* iter_value {nullptr};
            Node* node = BaseReverseIterator<V>::next_node();
            if (node != nullptr)
            {
                iter_value = node->value;
            }
            return const_cast<V*> (iter_value);
        }
    };

    class NodesIterator : public BaseIterator<Node>
    {
      public:
        NodesIterator(QTree<K, V>& qtree_ref):
            BaseIterator<Node>::BaseIterator(qtree_ref)
        {
        }

        NodesIterator(QTree<K, V>& qtree_ref, Node& start_node):
            BaseIterator<Node>::BaseIterator(qtree_ref, start_node)
        {
        }

        NodesIterator(const NodesIterator& orig) = default;
        NodesIterator& operator=(const NodesIterator& orig) = default;
        NodesIterator(NodesIterator&& orig) = default;
        NodesIterator& operator=(NodesIterator&& orig) = default;

        virtual ~NodesIterator()
        {
        }

        virtual Node* next()
        {
            return BaseIterator<Node>::next_node();
        }
    };

    class NodesReverseIterator : public BaseReverseIterator<Node>
    {
      public:
        NodesReverseIterator(QTree<K, V>& qtree_ref):
            BaseReverseIterator<Node>::BaseReverseIterator(qtree_ref)
        {
        }

        NodesReverseIterator(QTree<K, V>& qtree_ref, Node& start_node):
            BaseReverseIterator<Node>::BaseReverseIterator(qtree_ref, start_node)
        {
        }

        NodesReverseIterator(const NodesReverseIterator& orig) = default;
        NodesReverseIterator& operator=(const NodesReverseIterator& orig) = default;
        NodesReverseIterator(NodesReverseIterator&& orig) = default;
        NodesReverseIterator& operator=(NodesReverseIterator&& orig) = default;

        virtual ~NodesReverseIterator()
        {
        }

        virtual Node* next()
        {
            return BaseReverseIterator<Node>::next_node();
        }
    };

  public:
    QTree(const compare_func compare_fn):
        compare(compare_fn)
    {
    }

    QTree(const QTree& orig) = delete;
    QTree& operator=(const QTree& orig) = delete;
    QTree(QTree&& orig) = default;
    QTree& operator=(QTree&& orig) = default;

    virtual ~QTree()
    {
        clear_impl();
    }

    virtual V* get(const K* key) const
    {
        const V* value {nullptr};
        Node* node = find_node(key);
        if (node != nullptr)
        {
            value = node->value;
        }
        return const_cast<V*> (value);
    }

    virtual typename dsaext::Map<K, V>::entry get_entry(const K* key) const
    {
        typename dsaext::Map<K, V>::entry qtree_entry {nullptr, nullptr};
        Node* node = find_node(key);
        if (node != nullptr)
        {
            qtree_entry.key = node->key;
            qtree_entry.value = node->value;
        }
        return qtree_entry;
    }

    virtual Node* get_node(const K* key) const
    {
        return find_node(key);
    }

    // @throws std::bad_alloc, dsaext::DuplicateInsertionException
    virtual void insert(const K* key, const V* value)
    {
        Node** ref_ins_node {nullptr};
        Node* parent_node {nullptr};

        if (root == nullptr)
        {
            ref_ins_node = &root;
        }
        else
        {
            parent_node = root;
            while (true)
            {
                const int cmp_rc = compare(key, parent_node->key);
                if (cmp_rc < 0)
                {
                    if (parent_node->less == nullptr)
                    {
                        ref_ins_node = &parent_node->less;
                        break;
                    }
                    else
                    {
                        parent_node = parent_node->less;
                    }
                }
                else
                if (cmp_rc > 0)
                {
                    if (parent_node->greater == nullptr)
                    {
                        ref_ins_node = &parent_node->greater;
                        break;
                    }
                    else
                    {
                        parent_node = parent_node->greater;
                    }
                }
                else
                {
                    throw dsaext::DuplicateInsertException();
                }
            }
        }

        if (ref_ins_node != nullptr)
        {
            Node* ins_node = new Node(key, value);
            *ref_ins_node = ins_node;
            ins_node->parent = parent_node;
            ++size;
            if (parent_node != nullptr)
            {
                rebalance_insert(ins_node, parent_node);
            }
        }
    }

    // @throws std::bad_alloc, dsaext::DuplicateInsertionException
    virtual void insert_node(Node* node)
    {
        if (root == nullptr)
        {
            root = node;
            ++size;
        }
        else
        {
            Node* parent_node = root;
            while (true)
            {
                int cmp_rc = compare(node->key, parent_node->key);
                if (cmp_rc < 0)
                {
                    if (parent_node->less == nullptr)
                    {
                        parent_node->less = node;
                        node->parent = parent_node;
                        ++size;
                        rebalance_insert(node, parent_node);
                        break;
                    }
                    else
                    {
                        parent_node = parent_node->less;
                    }
                }
                else
                if (cmp_rc > 0)
                {
                    if (parent_node->greater == nullptr)
                    {
                        parent_node->greater = node;
                        node->parent = parent_node;
                        ++size;
                        rebalance_insert(node, parent_node);
                        break;
                    }
                    else
                    {
                        parent_node = parent_node->greater;
                    }
                }
                else
                {
                    throw dsaext::DuplicateInsertException();
                }
            }
        }
    }

    virtual void remove(const K* key)
    {
        Node* node = find_node(key);
        if (node != nullptr)
        {
            remove_node_impl(node);
        }
    }

    virtual void remove_node(Node* node)
    {
        remove_node_impl(node);
    }

    virtual void unlink_node(Node* node)
    {
        unlink_node_impl(node);
    }

    virtual void clear()
    {
        clear_impl();
        root = nullptr;
        size = 0;
    }

    size_t get_size() const
    {
        return size;
    }

  private:
    inline void clear_impl()
    {
        Node* node = root;
        while (node != nullptr)
        {
            if (node->less != nullptr)
            {
                node = node->less;
            }
            else
            if (node->greater != nullptr)
            {
                node = node->greater;
            }
            else
            {
                Node* leaf = node;
                node = node->parent;
                if (node != nullptr)
                {
                    if (leaf == node->less)
                    {
                        node->less = nullptr;
                    }
                    else
                    {
                        node->greater = nullptr;
                    }
                }
                delete leaf;
            }
        }
    }

    inline Node* find_node(const K* key) const
    {
        Node* node = root;
        while (node != nullptr)
        {
            int cmp_rc = compare(key, node->key);
            if (cmp_rc < 0)
            {
                node = node->less;
            }
            else
            if (cmp_rc > 0)
            {
                node = node->greater;
            }
            else
            {
                break;
            }
        }

        return node;
    }

    inline void remove_node_impl(Node* rm_node)
    {
        unlink_node_impl(rm_node);
        delete rm_node;
    }

    inline void unlink_node_impl(Node* rm_node)
    {
        --size;

        if (rm_node->less == nullptr && rm_node->greater == nullptr)
        {
            // leaf node - removal without replacement
            if (root == rm_node)
            {
                // root node leaf
                root = nullptr;
            }
            else
            {
                // non-root node leaf
                Node* rot_node = rm_node->parent;

                QTree::qtree_dir dir;
                if (rot_node->less == rm_node)
                {
                    // node to remove is in the left subtree
                    // of its parent

                    // save direction
                    dir = QTree::qtree_dir::DIR_LESS;
                    rot_node->less = nullptr;
                }
                else
                {
                    // node to remove is in the right subtree
                    // of its parent

                    // save direction
                    dir = QTree::qtree_dir::DIR_GREATER;
                    rot_node->greater = nullptr;
                }
                rebalance_remove(dir, rot_node);
            }
        }
        else
        {
            Node* replace_node {nullptr};
            // not a leaf node, removal by replacement
            // at least one child, or a child and a subtree, or two subtrees
            // find replacement node
            if (rm_node->balance == -1)
            {
                replace_node = rm_node->less;
                while (replace_node->greater != nullptr)
                {
                    replace_node = replace_node->greater;
                }
            }
            else
            {
                replace_node = rm_node->greater;
                while (replace_node->less != nullptr)
                {
                    replace_node = replace_node->less;
                }
            }
            Node* rot_node = replace_node->parent;

            QTree::qtree_dir dir;
            if (rot_node->less == replace_node)
            {
                // node to remove is in the left subtree
                // of its parent

                // save direction
                dir = QTree::qtree_dir::DIR_LESS;

                if (replace_node->less != nullptr)
                {
                    // replace node by its left child
                    rot_node->less = replace_node->less;
                    replace_node->less->parent = rot_node;
                }
                else
                if (replace_node->greater != nullptr)
                {
                    // replace node by its right child
                    rot_node->less = replace_node->greater;
                    replace_node->greater->parent = rot_node;
                }
                else
                {
                    // non-root leaf node
                    rot_node->less = nullptr;
                }
            }
            else
            {
                // node to remove is in the right subtree
                // of its parent

                // save direction
                dir = QTree::qtree_dir::DIR_GREATER;

                if (replace_node->less != nullptr)
                {
                    // replace node by its left child
                    rot_node->greater = replace_node->less;
                    replace_node->less->parent = rot_node;
                }
                else
                if (replace_node->greater != nullptr)
                {
                    // replace node by its right child
                    rot_node->greater = replace_node->greater;
                    replace_node->greater->parent = rot_node;
                }
                else
                {
                    // non-root leaf node
                    rot_node->greater = nullptr;
                }
            }

            // replace rm_node with replace_node
            if (rm_node->parent == nullptr)
            {
                // Node to be removed is the root node
                root = replace_node;
            }
            else
            {
                if (rm_node->parent->less == rm_node)
                {
                    rm_node->parent->less = replace_node;
                }
                else
                {
                    rm_node->parent->greater = replace_node;
                }
            }
            if (rm_node->less != nullptr)
            {
                rm_node->less->parent = replace_node;
            }
            if (rm_node->greater != nullptr)
            {
                rm_node->greater->parent = replace_node;
            }
            replace_node->parent  = rm_node->parent;
            replace_node->less    = rm_node->less;
            replace_node->greater = rm_node->greater;
            replace_node->balance = rm_node->balance;

            if (rot_node == rm_node)
            {
                rot_node = replace_node;
            }

            rebalance_remove(dir, rot_node);
        }
    }

    inline void rebalance_insert(
        Node* sub_node,
        Node* rot_node
    )
    {
        // update balance and perform rotations
        do
        {
            if (rot_node->less == sub_node)
            {
                --(rot_node->balance);
            }
            else
            {
                ++(rot_node->balance);
            }

            if (rot_node->balance == 0)
            {
                break;
            }
            else
            if (rot_node->balance == -2)
            {
                if (sub_node->balance == -1)
                {
                    // rotate R
                    rot_node->balance = 0;
                    sub_node->balance = 0;

                    sub_node->parent = rot_node->parent;
                    if (rot_node->parent != nullptr)
                    {
                        if (rot_node->parent->less == rot_node)
                        {
                            rot_node->parent->less = sub_node;
                        }
                        else
                        {
                            rot_node->parent->greater = sub_node;
                        }
                    }
                    else
                    {
                        root = sub_node;
                    }

                    rot_node->less = sub_node->greater;
                    if (sub_node->greater != nullptr)
                    {
                        sub_node->greater->parent = rot_node;
                    }

                    sub_node->greater = rot_node;
                    rot_node->parent = sub_node;
                }
                else
                {
                    // rotate LR
                    if (sub_node->greater->balance == -1)
                    {
                        sub_node->balance = 0;
                        rot_node->balance = 1;
                    }
                    else
                    if (sub_node->greater->balance == 1)
                    {
                        sub_node->balance = -1;
                        rot_node->balance = 0;
                    }
                    else
                    {
                        sub_node->balance = 0;
                        rot_node->balance = 0;
                    }
                    sub_node->greater->balance = 0;

                    sub_node->parent         = sub_node->greater;
                    sub_node->greater        = sub_node->greater->less;
                    sub_node->parent->less   = sub_node;
                    rot_node->less           = sub_node->parent->greater;
                    sub_node->parent->parent = rot_node->parent;
                    if (sub_node->greater != nullptr)
                    {
                        sub_node->greater->parent = sub_node;
                    }
                    if (rot_node->less != nullptr)
                    {
                        rot_node->less->parent = rot_node;
                    }

                    if (rot_node->parent != nullptr)
                    {
                        if (rot_node->parent->less == rot_node)
                        {
                            rot_node->parent->less = sub_node->parent;
                        }
                        else
                        {
                            rot_node->parent->greater = sub_node->parent;
                        }
                    }
                    else
                    {
                        root = sub_node->parent;
                    }

                    rot_node->parent          = sub_node->parent;
                    sub_node->parent->greater = rot_node;
                }
                break;
            }
            else
            if (rot_node->balance == 2)
            {
                if (sub_node->balance == 1)
                {
                    // rotate L
                    rot_node->balance = 0;
                    sub_node->balance = 0;

                    sub_node->parent = rot_node->parent;
                    if (rot_node->parent != nullptr)
                    {
                        if (rot_node->parent->less == rot_node)
                        {
                            rot_node->parent->less = sub_node;
                        }
                        else
                        {
                            rot_node->parent->greater = sub_node;
                        }
                    }
                    else
                    {
                        root = sub_node;
                    }

                    rot_node->greater = sub_node->less;
                    if (sub_node->less != nullptr)
                    {
                        sub_node->less->parent = rot_node;
                    }

                    sub_node->less = rot_node;
                    rot_node->parent = sub_node;
                }
                else
                {
                    // rotate RL
                    if (sub_node->less->balance == -1)
                    {
                        sub_node->balance = 1;
                        rot_node->balance = 0;
                    }
                    else
                    if (sub_node->less->balance == 1)
                    {
                        sub_node->balance = 0;
                        rot_node->balance = -1;
                    }
                    else
                    {
                        sub_node->balance = 0;
                        rot_node->balance = 0;
                    }
                    sub_node->less->balance = 0;

                    sub_node->parent          = sub_node->less;
                    sub_node->less            = sub_node->less->greater;
                    sub_node->parent->greater = sub_node;
                    rot_node->greater         = sub_node->parent->less;
                    sub_node->parent->parent  = rot_node->parent;
                    if (sub_node->less != nullptr)
                    {
                        sub_node->less->parent = sub_node;
                    }
                    if (rot_node->greater != nullptr)
                    {
                        rot_node->greater->parent = rot_node;
                    }

                    if (rot_node->parent != nullptr)
                    {
                        if (rot_node->parent->less == rot_node)
                        {
                            rot_node->parent->less = sub_node->parent;
                        }
                        else
                        {
                            rot_node->parent->greater = sub_node->parent;
                        }
                    }
                    else
                    {
                        root = sub_node->parent;
                    }

                    rot_node->parent       = sub_node->parent;
                    sub_node->parent->less = rot_node;
                }
                break;
            }

            sub_node = rot_node;
            rot_node = rot_node->parent;
        }
        while (rot_node != nullptr);
    }

    inline void rebalance_remove(qtree_dir dir, Node* rot_node)
    {
        // update balance and perform rotations
        while (rot_node != nullptr)
        {
            if (dir == QTree::qtree_dir::DIR_LESS)
            {
                // node was removed from left subtree
                ++rot_node->balance;
                if (rot_node->balance == 1)
                {
                    break;
                }
            }
            else
            {
                // node was removed from right subtree
                --rot_node->balance;
                if (rot_node->balance == -1)
                {
                    break;
                }
            }

            if (rot_node->parent != nullptr)
            {
                if (rot_node->parent->less == rot_node)
                {
                    dir = QTree::qtree_dir::DIR_LESS;
                }
                else
                {
                    dir = QTree::qtree_dir::DIR_GREATER;
                }
            }

            // update balance and perform rotations
            if (rot_node->balance == -2)
            {
                Node* sub_node = rot_node->less;
                // 0 or -1
                if (sub_node->balance <= 0)
                {
                    // rotate R
                    sub_node->parent = rot_node->parent;
                    if (rot_node->parent != nullptr)
                    {
                        if (rot_node->parent->less == rot_node)
                        {
                            rot_node->parent->less = sub_node;
                        }
                        else
                        {
                            rot_node->parent->greater = sub_node;
                        }
                    }
                    else
                    {
                        root = sub_node;
                    }

                    rot_node->less = sub_node->greater;
                    if (sub_node->greater != nullptr)
                    {
                        sub_node->greater->parent = rot_node;
                    }

                    sub_node->greater = rot_node;
                    rot_node->parent  = sub_node;

                    if (sub_node->balance == 0)
                    {
                        rot_node->balance = -1;
                        sub_node->balance = 1;
                        break;
                    }
                    else
                    {
                        rot_node->balance = 0;
                        sub_node->balance = 0;
                    }
                }
                else
                {
                    // rotate LR
                    if (sub_node->greater->balance == -1)
                    {
                        sub_node->balance = 0;
                        rot_node->balance = 1;
                    }
                    else
                    if (sub_node->greater->balance == 1)
                    {
                        sub_node->balance = -1;
                        rot_node->balance = 0;
                    }
                    else
                    {
                        sub_node->balance = 0;
                        rot_node->balance = 0;
                    }
                    sub_node->greater->balance = 0;

                    sub_node->parent         = sub_node->greater;
                    sub_node->greater        = sub_node->greater->less;
                    sub_node->parent->less   = sub_node;
                    rot_node->less           = sub_node->parent->greater;
                    sub_node->parent->parent = rot_node->parent;
                    if (sub_node->greater != nullptr)
                    {
                        sub_node->greater->parent = sub_node;
                    }
                    if (rot_node->less != nullptr)
                    {
                        rot_node->less->parent = rot_node;
                    }

                    if (rot_node->parent != nullptr)
                    {
                        if (rot_node->parent->less == rot_node)
                        {
                            rot_node->parent->less = sub_node->parent;
                        }
                        else
                        {
                            rot_node->parent->greater = sub_node->parent;
                        }
                    }
                    else
                    {
                        root = sub_node->parent;
                    }

                    rot_node->parent          = sub_node->parent;
                    sub_node->parent->greater = rot_node;
                }
                rot_node = rot_node->parent;
                // end of R / LR rotations
            }
            else
            if (rot_node->balance == 2)
            {
                Node* sub_node = rot_node->greater;
                // 0 or 1
                if (sub_node->balance >= 0)
                {
                    // rotate L
                    sub_node->parent = rot_node->parent;
                    if (rot_node->parent != nullptr)
                    {
                        if (rot_node->parent->less == rot_node)
                        {
                            rot_node->parent->less = sub_node;
                        }
                        else
                        {
                            rot_node->parent->greater = sub_node;
                        }
                    }
                    else
                    {
                        root = sub_node;
                    }

                    rot_node->greater = sub_node->less;
                    if (sub_node->less != nullptr)
                    {
                        sub_node->less->parent = rot_node;
                    }

                    sub_node->less   = rot_node;
                    rot_node->parent = sub_node;
                    if (sub_node->balance == 0)
                    {
                        rot_node->balance = 1;
                        sub_node->balance = -1;
                        break;
                    }
                    else
                    {
                        rot_node->balance = 0;
                        sub_node->balance = 0;
                    }
                }
                else
                {
                    // rotate RL
                    if (sub_node->less->balance == -1)
                    {
                        sub_node->balance = 1;
                        rot_node->balance = 0;
                    }
                    else
                    if (sub_node->less->balance == 1)
                    {
                        sub_node->balance = 0;
                        rot_node->balance = -1;
                    }
                    else
                    {
                        sub_node->balance = 0;
                        rot_node->balance = 0;
                    }
                    sub_node->less->balance = 0;

                    sub_node->parent          = sub_node->less;
                    sub_node->less            = sub_node->less->greater;
                    sub_node->parent->greater = sub_node;
                    rot_node->greater         = sub_node->parent->less;
                    sub_node->parent->parent  = rot_node->parent;
                    if (sub_node->less != nullptr)
                    {
                        sub_node->less->parent = sub_node;
                    }
                    if (rot_node->greater != nullptr)
                    {
                        rot_node->greater->parent = rot_node;
                    }

                    if (rot_node->parent != nullptr)
                    {
                        if (rot_node->parent->less == rot_node)
                        {
                            rot_node->parent->less = sub_node->parent;
                        }
                        else
                        {
                            rot_node->parent->greater = sub_node->parent;
                        }
                    }
                    else
                    {
                        root = sub_node->parent;
                    }

                    rot_node->parent       = sub_node->parent;
                    sub_node->parent->less = rot_node;
                }
                rot_node = rot_node->parent;
                // end of L / RL rotations
            }
            rot_node = rot_node->parent;
        }
    }

  private:
    Node*  root {nullptr};
    size_t size {0};

    const compare_func compare;
};

#endif	/* QTREE_H */
