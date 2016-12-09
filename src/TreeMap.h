#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi {

    template<typename KeyType, typename ValueType>
    class TreeMap {
    public:
        using key_type = KeyType;
        using mapped_type = ValueType;
        using value_type = std::pair<const key_type, mapped_type>;
        using size_type = std::size_t;
        using reference = value_type&;
        using const_reference = const value_type&;

        class ConstIterator;

        class Iterator;

        class TreeNode;

        using iterator = Iterator;
        using const_iterator = ConstIterator;

        TreeMap() : mRoot(nullptr), mCount(0) {}

        TreeMap(std::initializer_list<value_type> list) : TreeMap() {
            for (auto&& item : list)
                insert(item);
        }

        TreeMap(const TreeMap& other) : TreeMap() {
            for (auto&& item : other)
                insert(item);
        }

        TreeMap(TreeMap&& other) : TreeMap() {
            std::swap(mRoot, other.mRoot);
            std::swap(mCount, other.mCount);
        }

        TreeMap& operator=(const TreeMap& other) {
            if (*this == other)
                return *this;
            clear(mRoot);
            for (auto&& item : other)
                insert(item);
            return *this;
        }

        TreeMap& operator=(TreeMap&& other) {
            if (*this == other)
                return *this;
            clear(mRoot);
            std::swap(mRoot, other.mRoot);
            std::swap(mCount, other.mCount);
            return *this;
        }

        bool isEmpty() const {
            return mCount == 0;
        }

        mapped_type& operator[](const key_type& key) {
            TreeNode* node = findNode(key);
            if (node != nullptr)
                return node->mPair.second;
            node = allocate(key);
            return node->mPair.second;
        }

        const mapped_type& valueOf(const key_type& key) const {
            TreeNode* node = findNode(key);
            if (node == nullptr)
                throw std::out_of_range("Key does not exists");
            else
                return node->mPair.second;
        }

        mapped_type& valueOf(const key_type& key) {
            return const_cast<mapped_type&>(static_cast<const TreeMap<KeyType, ValueType>*>(this)->valueOf(key));
        }

        const_iterator find(const key_type& key) const {
            TreeNode* node = findNode(key);
            if (node == nullptr)
                return end();
            return Iterator(*this, node);
        }

        iterator find(const key_type& key) {
            return static_cast<const TreeMap<KeyType, ValueType>*>(this)->find(key);
        }

        void remove(const key_type& key) {
            removeNode(key);
        }

        void remove(const const_iterator& it) {
            if (it == end())
                throw std::out_of_range("Removing end iterator");
            removeNode(it.mNode->mPair.first);
        }

        size_type getSize() const {
            return mCount;
        }

        bool operator==(const TreeMap& other) const {
            if (mCount != other.mCount)
                return false;

            for (auto&& item : other) {
                TreeNode* node = findNode(item.first);
                if (node == nullptr || node->mPair.second != item.second)
                    return false;
            }

            return true;
        }

        bool operator!=(const TreeMap& other) const {
            return !(*this == other);
        }

        iterator begin() {
            return Iterator(*this, mostLeft());
        }

        iterator end() {
            return Iterator(*this, nullptr);
        }

        const_iterator cbegin() const {
            return ConstIterator(*this, mostLeft());
        }

        const_iterator cend() const {
            return ConstIterator(*this, nullptr);
        }

        const_iterator begin() const {
            return cbegin();
        }

        const_iterator end() const {
            return cend();
        }

    private:
        TreeNode* mRoot;
        size_type mCount;

        iterator insert(value_type pValue) {
            TreeNode* node = allocate(pValue.first);
            node->mPair.second = pValue.second;
            return Iterator(*this, node);
        };

        TreeNode* allocate(key_type pKey) {
            TreeNode* new_node = new TreeNode(std::make_pair(pKey, ValueType()));
            TreeNode* node = mRoot;
            TreeNode* parent;
            ++mCount;

            if (mRoot == nullptr) {
                mRoot = new_node;
                return new_node;
            }

            while (true) {
                parent = node;

                if (pKey < node->mPair.first) {
                    node = node->mLeft;
                    if (node == nullptr) {
                        new_node->mParent = parent;
                        parent->mLeft = new_node;
                        new_node->mHeight = parent->mHeight + 1;
                        rebalance(parent);
                        return new_node;
                    }
                } else {
                    node = node->mRight;
                    if (node == nullptr) {
                        new_node->mParent = parent;
                        parent->mRight = new_node;
                        new_node->mHeight = parent->mHeight + 1;
                        rebalance(parent);
                        return new_node;
                    }
                }
            }
        };

        void removeNode(KeyType pKey) {
            if (mRoot == nullptr)
                throw std::out_of_range("Removing node that does not exist");

            TreeNode* n = mRoot;
            TreeNode* parent = mRoot;
            TreeNode* child = mRoot;
            TreeNode* delNode = nullptr;

            while (child != nullptr) {
                parent = n;
                n = child;
                if (pKey >= n->mPair.first)
                    child = n->mRight;
                else
                    child = n->mLeft;
                if (pKey == n->mPair.first)
                    delNode = n;
            }

            if (!delNode)
                throw std::out_of_range("Removing nonexisting element");

            //delNode->mPair = std::move(n->mPair);

            if (n->mLeft)
                child = n->mLeft;
            else
                child = n->mRight;

            if (mRoot->mPair.first == pKey) {
                mRoot = child;
            } else {
                if (parent->mLeft == n)
                    parent->mLeft = child;
                else
                    parent->mRight = child;

                rebalance(parent);
            }
            --mCount;
        }


        TreeNode* findNode(const key_type& pKey) const {
            TreeNode* root = mRoot;
            while (root && root->mPair.first != pKey)
                if (root->mPair.first < pKey)
                    root = root->mRight;
                else
                    root = root->mLeft;
            return root;
        }

        void clear(TreeNode* pRoot) {

            if (pRoot == nullptr)
                return;

            clear(pRoot->mLeft);
            clear(pRoot->mRight);

            if (pRoot->mLeft != nullptr) {
                --mCount;
                delete pRoot->mLeft;
                pRoot->mLeft = nullptr;
            }
            if (pRoot->mRight != nullptr) {
                --mCount;
                delete pRoot->mRight;
                pRoot->mRight = nullptr;
            }
            if (pRoot == mRoot) {
                delete mRoot;
                mRoot = nullptr;
                --mCount;
            }
        }

        TreeNode* mostLeft() const {
            TreeNode* node = mRoot;
            if (node == nullptr)
                return nullptr;
            while (node->mLeft != nullptr)
                node = node->mLeft;
            return node;
        }

        TreeNode* mostRight() const {
            TreeNode* node = mRoot;
            if (!node)
                return nullptr;

            while (node->mRight)
                node = node->mRight;
            return node;
        }

        void rebalance(TreeNode* pRoot) {
            int balance = getHeight(pRoot->mLeft) - getHeight(pRoot->mRight);

            if (balance == 2) {
                if (getHeight(pRoot->mLeft->mLeft) >= getHeight(pRoot->mLeft->mRight))
                    pRoot = rotateRight(pRoot);
                else
                    pRoot = rotateLeftRight(pRoot);

            } else if (balance == -2) {
                if (getHeight(pRoot->mRight->mRight) >= getHeight(pRoot->mRight->mLeft))
                    pRoot = rotateLeft(pRoot);
                else
                    pRoot = rotateRightLeft(pRoot);
            }
            if (pRoot->mParent != nullptr)
                rebalance(pRoot);
            else
                mRoot = pRoot;
        }

        TreeNode* rotateLeft(TreeNode* pRoot) {
            TreeNode* x = pRoot->mRight;
            x->mParent = pRoot->mParent;
            pRoot->mRight = x->mLeft;

            if (pRoot->mRight != nullptr)
                pRoot->mRight->mParent = pRoot;

            x->mLeft = pRoot;
            pRoot->mParent = x;

            if (x->mParent != nullptr) {
                if (x->mParent->mRight == pRoot)
                    x->mParent->mRight = x;
                else
                    x->mParent->mLeft = x;
            }
            pRoot->mHeight = std::max(getHeight(pRoot->mLeft), getHeight(pRoot->mRight)) + 1;
            x->mHeight = std::max(getHeight(x->mLeft), getHeight(x->mRight)) + 1;

            return x;
        }


        TreeNode* rotateRight(TreeNode* pRoot) {
            TreeNode* x = pRoot->mLeft;
            x->mParent = pRoot->mParent;
            pRoot->mLeft = x->mRight;

            if (pRoot->mLeft != nullptr)
                pRoot->mLeft->mParent = pRoot;

            x->mRight = pRoot;
            pRoot->mParent = x;

            if (x->mParent != nullptr) {
                if (x->mParent->mRight == pRoot)
                    x->mParent->mRight = x;
                else
                    x->mParent->mLeft = x;
            }
            pRoot->mHeight = std::max(getHeight(pRoot->mLeft), getHeight(pRoot->mRight)) + 1;
            x->mHeight = std::max(getHeight(x->mLeft), getHeight(x->mRight)) + 1;

            return x;
        }

        TreeNode* rotateLeftRight(TreeNode* pRoot) {
            pRoot->mLeft = rotateLeft(pRoot->mLeft);
            return rotateRight(pRoot);
        }

        TreeNode* rotateRightLeft(TreeNode* pRoot) {
            pRoot->mRight = rotateRight(pRoot->mRight);
            return rotateLeft(pRoot);
        }

        inline int getHeight(const TreeNode* pRoot) const {
            if (!pRoot)
                return 0;
            return pRoot->mHeight;
        }
    };

    template<typename KeyType, typename ValueType>
    struct TreeMap<KeyType, ValueType>::TreeNode {
        value_type mPair;
        TreeNode* mParent;
        TreeNode* mLeft;
        TreeNode* mRight;
        int mHeight;

        TreeNode() : mPair(std::make_pair(KeyType(), ValueType())), mParent(nullptr), mLeft(nullptr), mRight(nullptr),
                     mHeight(1) {}

        TreeNode(value_type pPair) : mPair(pPair), mParent(nullptr), mLeft(nullptr), mRight(nullptr), mHeight(1) {}
    };

    template<typename KeyType, typename ValueType>
    class TreeMap<KeyType, ValueType>::ConstIterator {
    public:
        using reference = typename TreeMap::const_reference;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename TreeMap::value_type;
        using pointer = const typename TreeMap::value_type*;

        friend class TreeMap;

        explicit ConstIterator(const TreeMap& pMap, TreeNode* pNode) : mMap(pMap),
                                                                       mNode(pNode) {}

        ConstIterator(const ConstIterator& other) : ConstIterator(other.mMap, other.mNode) {}

        ConstIterator& operator++() {
            if (mNode == nullptr)
                throw std::out_of_range("Eh, what are you doing, incrementing end iterator?");

            if (mNode->mRight) {
                mNode = mNode->mRight;
                while (mNode->mLeft) mNode = mNode->mLeft;
            } else {
                while (mNode->mParent && mNode->mParent->mRight == mNode) mNode = mNode->mParent;
                mNode = mNode->mParent;
            }
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator old(*this);
            operator++();
            return old;
        }

        ConstIterator& operator--() {
            if (mNode == nullptr)
                throw std::out_of_range("Eh, what are you doing, decrementing begin iterator?");

            if (mNode->mLeft) {
                mNode = mNode->mRight;
                while (mNode->mRight) mNode = mNode->mRight;
            } else {
                while (mNode->mParent && mNode->mParent->mLeft == mNode) mNode = mNode->mParent;
                mNode = mNode->mParent;
            }
            return *this;
        }

        ConstIterator operator--(int) {
            ConstIterator old(*this);
            operator--();
            return old;
        }

        reference operator*() const {
            if (mNode == nullptr)
                throw std::out_of_range("Dereferencing used iterator");
            return mNode->mPair;
        }

        pointer operator->() const {
            return &this->operator*();
        }

        bool operator==(const ConstIterator& other) const {
            return mNode == other.mNode;
        }

        bool operator!=(const ConstIterator& other) const {
            return !(*this == other);
        }

    private:
        const TreeMap& mMap;
        TreeNode* mNode;
    };

    template<typename KeyType, typename ValueType>
    class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator {
    public:
        using reference = typename TreeMap::reference;
        using pointer = typename TreeMap::value_type*;

        explicit Iterator(const TreeMap& pMap, TreeNode* pNode) : ConstIterator(pMap, pNode) {}

        Iterator(const ConstIterator& other)
                : ConstIterator(other) {}

        Iterator& operator++() {
            ConstIterator::operator++();
            return *this;
        }

        Iterator operator++(int) {
            auto result = *this;
            ConstIterator::operator++();
            return result;
        }

        Iterator& operator--() {
            ConstIterator::operator--();
            return *this;
        }

        Iterator operator--(int) {
            auto result = *this;
            ConstIterator::operator--();
            return result;
        }

        pointer operator->() const {
            return &this->operator*();
        }

        reference operator*() const {
            // ugly cast, yet reduces code duplication.
            return const_cast<reference>(ConstIterator::operator*());
        }
    };

}

#endif /* AISDI_MAPS_MAP_H */
