#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu 
{

    template<class Key, class T, class Compare = std::less<Key>>
    class map
    {
    public:
        typedef pair<const Key, T> value_type;
        struct node
        {
            value_type* value;
            node* left, * right;
            int level;
            node() :value(NULL), left(NULL), right(NULL), level(1) {}
            node(const Key& k, const T& d, node* l = NULL, node* r = NULL, int lv = 1) :left(l), right(r), level(lv)
            {
                value = new value_type(k, d);
            }
            node(const value_type& v, node* l = NULL, node* r = NULL, int lv = 1) :left(l), right(r), level(lv)
            {
                value = new value_type(v);
            }
            node(node*& other) :level(other->level), left(NULL), right(NULL)
            {
                value = new value_type(*(other->value));
            }
            ~node()
            {
                delete value;
            }
        };
        node* root;
        int currentSize = 0;
        int Min(int x, int y)
        {
            return x < y ? x : y;
        }
        class const_iterator;
        class iterator
        {
        public:
            map* cmap;
            node* cnode;
        public:
            iterator() :cmap(NULL), cnode(NULL) {}
            iterator(const iterator& other) :cmap(other.cmap), cnode(other.cnode) {}
            iterator(map* m, node* n) :cmap(m), cnode(n) {}
            iterator operator++(int)
            {
                if (*this == cmap->end())
                    throw invalid_iterator();
                iterator tmp(*this);
                cnode = cmap->next(cnode);
                return tmp;
            }
            iterator& operator++()
            {
                if (*this == cmap->end())
                    throw invalid_iterator();
                cnode = cmap->next(cnode);
                return *this;
            }
            iterator operator--(int)
            {
                if (*this == cmap->begin())
                    throw invalid_iterator();
                iterator tmp(*this);
                if (*this == cmap->end())
                {
                    node* n = cmap->root;
                    while (n && n->right)
                        n = n->right;
                    cnode = n;
                }
                else
                    cnode = cmap->prev(cnode);
                return tmp;
            }
            iterator& operator--()
            {
                if (*this == cmap->begin())
                    throw invalid_iterator();
                if (*this == cmap->end())
                {
                    node* n = cmap->root;
                    while (n && n->right)
                        n = n->right;
                    cnode = n;
                }
                else
                    cnode = cmap->prev(cnode);
                return *this;
            }
            value_type& operator*() const
            {
                return *(cnode->value);
            }
            bool operator==(const iterator& rhs) const
            {
                return cnode == rhs.cnode && cmap == rhs.cmap;
            }
            bool operator==(const const_iterator& rhs) const
            {
                return cnode == rhs.cnode && cmap == rhs.cmap;
            }
            bool operator!=(const iterator& rhs) const
            {
                return cnode != rhs.cnode || cmap != rhs.cmap;
            }
            bool operator!=(const const_iterator& rhs) const
            {
                return cnode != rhs.cnode || cmap != rhs.cmap;
            }
            value_type* operator->() const noexcept
            {
                return cnode->value;
            }
        };
        class const_iterator
        {
        public:
            const map* cmap;
            node* cnode;
        public:
            const_iterator() :cmap(NULL), cnode(NULL) {}
            const_iterator(const iterator& other) :cmap(other.cmap), cnode(other.cnode) {}
            const_iterator(const const_iterator& other) :cmap(other.cmap), cnode(other.cnode) {}
            const_iterator(const map* m, node* n) :cmap(m), cnode(n) {}
            const_iterator operator++(int)
            {
                if (*this == cmap->cend())
                    throw invalid_iterator();
                const_iterator tmp(*this);
                cnode = cmap->next(cnode);
                return tmp;
            }
            const_iterator& operator++()
            {
                if (*this == cmap->cend())
                    throw invalid_iterator();
                cnode = cmap->next(cnode);
                return *this;
            }
            const_iterator operator--(int)
            {
                if (*this == cmap->cbegin())
                    throw invalid_iterator();
                const_iterator tmp(*this);
                if (*this == cmap->cend())
                {
                    node* n = cmap->root;
                    while (n && n->right)
                        n = n->right;
                    cnode = n;
                }
                else
                    cnode = cmap->prev(cnode);
                return tmp;
            }
            const_iterator& operator--()
            {
                if (*this == cmap->cbegin())
                    throw invalid_iterator();
                if (*this == cmap->cend())
                {
                    node* n = cmap->root;
                    while (n && n->right)
                        n = n->right;
                    cnode = n;
                }
                else
                    cnode = cmap->prev(cnode);
                return *this;
            }
            value_type& operator*() const
            {
                return *(cnode->value);
            }
            bool operator==(const iterator& rhs) const
            {
                return cnode == rhs.cnode && cmap == rhs.cmap;
            }
            bool operator==(const const_iterator& rhs) const
            {
                return cnode == rhs.cnode && cmap == rhs.cmap;
            }
            bool operator!=(const iterator& rhs) const
            {
                return cnode != rhs.cnode || cmap != rhs.cmap;
            }
            bool operator!=(const const_iterator& rhs) const
            {
                return cnode != rhs.cnode || cmap != rhs.cmap;
            }
            value_type* operator->() const noexcept
            {
                return cnode->value;
            }
        };
        //构造函数
        map() :root(NULL), currentSize(0) {}
        map(const map& other) :root(NULL), currentSize(other.currentSize)
        {
            if(other.root!=NULL)
                copy(root, other.root);
        }
        //=重载
        map& operator=(const map& other)
        {
            if (&other == this)
                return*this;
            clear();
            copy(root, other.root);
            currentSize = other.currentSize;
            return*this;
        }
        //析构函数
        ~map()
        {
            clear();
            currentSize = 0;
        }
        //下标
        T& at(const Key& key)
        {
            node* t = findNode(key);
            if (t == NULL)
                throw index_out_of_bound();
            else
                return t->value->second;
        }
        const T& at(const Key& key) const
        {
            node* t = findNode(key);
            if (t == NULL)
                throw index_out_of_bound();
            else
                return t->value->second;
        }
        T& operator[](const Key& key)
        {
            node* t = findNode(key);
            T tmp;
            if (t == NULL)
            {
                value_type value(key, tmp);
                pair<iterator, bool> p = insert(value);
                return p.first.cnode->value->second;
            }
            return t->value->second;
        }
        //下标重载
        const T& operator[](const Key& key) const
        {
            node* t = findNode(key);
            if (t == NULL)
                throw index_out_of_bound();
            else
                return t->value->second;
        }
        iterator begin()
        {
            node* tmp = root;
            while (tmp && tmp->left)
                tmp = tmp->left;
            return iterator(this, tmp);
        }
        const_iterator cbegin() const
        {
            node* tmp = root;
            while (tmp && tmp->left)
                tmp = tmp->left;
            return const_iterator(this, tmp);
        }
        iterator end()
        {
            return iterator(this, NULL);
        }
        const_iterator cend() const
        {
            return const_iterator(this, NULL);
        }
        bool empty() const
        {
            return currentSize == 0;
        }
        size_t size() const
        {
            return currentSize;
        }
        void clear()
        {
            makeEmpty(root);
            root = NULL;
            currentSize = 0;
        }
        pair<iterator, bool> insert(const value_type& value)
        {
            pair<iterator, bool> ans = _insert(value, root);
            return ans;
        }
        void erase(iterator pos) 
        {
            if (pos.cmap != this || pos == this->end() || pos.cnode == NULL)
                throw invalid_iterator();
            remove(pos.cnode->value->first, root);
            --currentSize;
        }
        size_t count(const Key& key) const
        {
            node* tmp = root;
            while (tmp)
            {
                if (!(Compare()(key, tmp->value->first) || Compare()(tmp->value->first, key)))
                    return 1;
                else if (Compare()(key, tmp->value->first))
                    tmp = tmp->left;
                else
                    tmp = tmp->right;
            }
            return 0;
        }
        iterator find(const Key& key)
        {
            node* tmp = findNode(key);
            if (tmp != NULL)
                return iterator(this, tmp);
            return end();
        }
        const_iterator find(const Key& key) const
        {
            node* tmp = findNode(key);
            if (tmp != NULL)
                return const_iterator(this, tmp);
            return cend();
        }

        //辅助函数
        //复制
        void copy(node*& n, node* other)
        {
            if (other)
                n = new node(other);
            else
                return;
            if (other->left)
                copy(n->left, other->left);
            if (other->right)
                copy(n->right, other->right);
        }
        //查找
        node* findNode(const Key& key) const
        {
            node* t = root;
            while (t != NULL && (Compare()(key, t->value->first) || Compare()(t->value->first, key)))
            {
                if (Compare()(key, t->value->first))
                    t = t->left;
                else
                    t = t->right;
            }
            return t;
        }
        //清除
        void makeEmpty(node*& t)
        {
            if (t != NULL)
            {
                makeEmpty(t->left);
                makeEmpty(t->right);
                delete t;
                t = NULL;
            }
        }
        //直接前驱
        node* prev(node* n) const
        {
            if (n->left)
            {
                node* ans = n->left;
                while (ans->right)
                    ans = ans->right;
                return ans;
            }
            else
            {
                node* tmp = root;
                Key k = n->value->first;
                node* m = NULL;
                while (tmp)
                {
                    if (Compare()(tmp->value->first, k))
                    {
                        m = tmp;
                        tmp = tmp->right;
                    }
                    else
                        tmp = tmp->left;
                }
                return m;
            }
        }
        //直接后继
        node* next(node* n) const
        {
            if (n->right)
            {
                node* ans = n->right;
                while (ans->left)
                    ans = ans->left;
                return ans;
            }
            else
            {
                node* tmp = root;
                Key k = n->value->first;
                node* m = NULL;
                while (tmp)
                {
                    if (Compare()(k, tmp->value->first))
                    {
                        m = tmp;
                        tmp = tmp->left;
                    }
                    else
                        tmp = tmp->right;
                }
                return m;
            }
        }
        //insert辅助
        pair<iterator, bool> _insert(const value_type& value, node*& t)
        {
            if (t == NULL)
            {
                t = new node(value, NULL, NULL);
                ++currentSize;
                LL(t);
                RR(t);
                pair<iterator, bool> tmp = sjtu::pair<iterator, bool>(iterator(this, t), 1);
                return tmp;
            }
            else if (Compare()(value.first, t->value->first))
            {
                pair<iterator, bool> tmp = _insert(value, t->left);
                LL(t);
                RR(t);
                return tmp;
            }
            else if (Compare()(t->value->first, value.first))
            {
                pair<iterator, bool> tmp = _insert(value, t->right);
                LL(t);
                RR(t);
                return tmp;
            }
            else
            {
                pair<iterator, bool> tmp = sjtu::pair<iterator, bool>(iterator(this, t), 0);
                return tmp;
            }
        }
        //remove辅助
        void remove(const Key& x, node*& t)
        {
            if (t == NULL)
                return;
            if (Compare()(x, t->value->first))
                remove(x, t->left);
            else if (Compare()(t->value->first, x))
                remove(x, t->right);
            else if (t->left != NULL && t->right != NULL)
            {
                node* tmp = t->right;
                node* fa = t; 
                while (tmp->left != NULL)
                {
                    fa = tmp;
                    tmp = tmp->left;
                }
                if (fa != t)
                {
                    node* l = t->left, * r = t->right;
                    int tmpLevel = t->level;
                    fa->left = t;
                    fa->left->level = tmp->level;
                    fa->left->right = tmp->right;
                    fa->left->left = NULL;
                    delete fa->left->value;
                    fa->left->value = new value_type(*(tmp->value));
                    tmp->right = r;
                    tmp->left = l;
                    tmp->level = tmpLevel;
                    t = tmp;
                    remove(fa->left->value->first, t->right);
                }
                else
                {
                    int tmpLevel = fa->level;
                    fa->level = tmp->level;
                    tmp->level = tmpLevel;
                    tmp->left = fa->left;
                    fa->right = tmp->right;
                    fa->left = NULL;
                    tmp->right = fa;
                    t = tmp;
                    delete fa->value;
                    fa->value = new value_type(*(tmp->value));
                    remove(tmp->value->first, t->right);
                }
            }
            else
            {
                node* oldNode = t;
                t = (t->left != NULL) ? t->left : t->right;
                delete oldNode;
                return;
            }
            //修改父结点层次、调整平衡
            if (t->left == NULL || t->right == NULL)
                t->level = 1;
            else
                t->level = Min(t->left->level, t->right->level) + 1;
            if (t->right != NULL && t->right->level > t->level)
                t->right->level = t->level;
            LL(t);
            if (t->right != NULL)
                LL(t->right);
            if (t->right != NULL && t->right->right != NULL)
                LL(t->right->right);
            RR(t);
            if (t->right != NULL)
                RR(t->right);
        }
        //旋转函数
        void LL(node*& t)
        {
            if (t->left != NULL && t->left->level == t->level)
            {
                node* t1 = t->left;
                t->left = t1->right;
                t1->right = t;
                t = t1;
            }
        }
        void RR(node*& t)
        {
            if (t->right != NULL && t->right->right != NULL && t->right->right->level == t->level)
            {
                node* t1 = t->right;
                t->right = t1->left;
                t1->left = t;
                t = t1;
                ++t->level;
            }
        }
    };
}

#endif