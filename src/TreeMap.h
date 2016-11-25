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

        TreeMap() : mRoot(nullptr), mCount(0) { }

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
            clear();
            for (auto&& item : other)
                insert(item);
            return *this;
        }

        TreeMap& operator=(TreeMap&& other) {
            clear();
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
            removeNode(findNode(key));
        }

        void remove(const const_iterator& it) {
            removeNode(it.mNode);
        }

        size_type getSize() const {
            return mCount;
        }

        bool operator==(const TreeMap& other) const {
            (void) other;
            throw std::runtime_error("TODO");
        }

        bool operator!=(const TreeMap& other) const {
            return !(*this == other);
        }

        iterator begin() {
            return Iterator(*this, mRoot);
        }

        iterator end() {
            return Iterator(*this, nullptr);
        }

        const_iterator cbegin() const {
            return ConstIterator(*this, mRoot);
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
            (void) pKey;
            ++mCount;
            return new TreeNode;
        };

        void removeNode(TreeNode* pNode) {
            (void)pNode;
        }

        TreeNode* rotateRight(TreeNode* pRoot) {
            return pRoot;
        }

        TreeNode* rotateLeft(TreeNode* pRoot) {
            return pRoot;
        }

        void clear() {
        }

        TreeNode* findNode(const key_type& pKey) const {
            TreeNode* root = mRoot;
            while (root != nullptr && root->mPair.first != pKey)
                if (root->mPair.first < pKey)
                    root = root->mRight;
                else
                    root = root->mLeft;
            return root;
        }

    };

    template<typename KeyType, typename ValueType>
    struct TreeMap<KeyType, ValueType>::TreeNode {
        enum class TreeColor {
            BLACK, RED
        };
        value_type mPair;
        TreeColor mColor;
        TreeNode* mParent;
        TreeNode* mLeft;
        TreeNode* mRight;
    };


    template<typename KeyType, typename ValueType>
    class TreeMap<KeyType, ValueType>::ConstIterator {
    public:
        using reference = typename TreeMap::const_reference;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename TreeMap::value_type;
        using pointer = const typename TreeMap::value_type*;

        friend class TreeMap;

        explicit ConstIterator(const TreeMap& pMap, TreeNode* pNode) : mMap(pMap), mNode(pNode) { }

        ConstIterator(const ConstIterator& other) : mMap(other.mMap), mNode(other.mNode) { }

        ConstIterator& operator++() {
            throw std::runtime_error("TODO");
        }

        ConstIterator operator++(int) {
            ConstIterator old(*this);
            operator++();
            return old;
        }

        ConstIterator& operator--() {
            throw std::runtime_error("TODO");
        }

        ConstIterator operator--(int) {
            ConstIterator old(*this);
            operator--();
            return old;
        }

        reference operator*() const {
            if(mNode == nullptr)
                throw std::out_of_range("Derefferencing end iterator");
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
                : ConstIterator(other) { }

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
