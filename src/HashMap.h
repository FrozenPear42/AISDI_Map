#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <functional>
#include <iostream>

namespace aisdi {

    template<typename KeyType, typename ValueType>
    class HashMap {
    public:
        using key_type = KeyType;
        using mapped_type = ValueType;
        using value_type = std::pair<const key_type, mapped_type>;
        using size_type = std::size_t;
        using reference = value_type&;
        using const_reference = const value_type&;

        class ConstIterator;

        class Iterator;

        class BucketNode;

        using iterator = Iterator;
        using const_iterator = ConstIterator;

        HashMap() : mBucketCount(200), mCount(0) {
            mBuckets = new BucketNode* [mBucketCount];
            for (size_type i = 0; i < mBucketCount; i++)
                mBuckets[i] = nullptr;
            mHasher = [](const key_type& pKey) {
                return std::hash<key_type>{}(pKey);
            };
        }

        HashMap(std::initializer_list<value_type> list) : HashMap() {
            for (auto&& item : list)
                insert(item.first, item.second);
        }

        HashMap(const HashMap& other) : HashMap() {
            mHasher = other.mHasher;

            for (auto it = other.cbegin(); it != other.cend(); ++it)
                insert((*it).first, (*it).second);
        }

        HashMap(HashMap&& other) : HashMap() {
            std::swap(mCount, other.mCount);
            std::swap(mBuckets, other.mBuckets);
            std::swap(mBucketCount, other.mBucketCount);
            std::swap(mHasher, other.mHasher);
        }

        HashMap& operator=(const HashMap& other) {
            if (*this == other)
                return *this;
            clear();
            for (auto&& item : other) {
                insert(item.first, item.second);
            }
            return *this;
        }


        HashMap& operator=(HashMap&& other) {
            std::swap(mCount, other.mCount);
            std::swap(mBuckets, other.mBuckets);
            std::swap(mBucketCount, other.mBucketCount);
            std::swap(mHasher, other.mHasher);
            other.mCount = 0;
            return *this;
        }

        bool isEmpty() const {
            return mCount == 0;
        }

        mapped_type& operator[](const key_type& key) {
            size_type bucket = bucketHash(key);
            BucketNode* node = mBuckets[bucket];
            while (node != nullptr && node->mPair.first != key)
                node = node->mNextNode;
            if (node == nullptr)
                return (*insert(key)).second;
            else
                return node->mPair.second;
        }

        const mapped_type& valueOf(const key_type& key) const {
            auto it = find(key);
            if (it != end())
                return (*it).second;
            throw std::out_of_range("Not found");
        }

        mapped_type& valueOf(const key_type& key) {
            return const_cast<mapped_type&>(static_cast<const HashMap<KeyType, ValueType>*>(this)->valueOf(key));
        }

        const_iterator find(const key_type& key) const {
            size_type bucket = bucketHash(key);
            BucketNode* node = mBuckets[bucket];
            while (node != nullptr && node->mPair.first != key)
                node = node->mNextNode;
            if (node == nullptr)
                return end();
            return ConstIterator(*this, bucket, node);
        }

        iterator find(const key_type& key) {
            return static_cast<const HashMap<KeyType, ValueType>*>(this)->find(key);
        }

        void remove(const key_type& key) {
            size_type bucket = bucketHash(key);
            BucketNode* prev = mBuckets[bucket];

            if (prev == nullptr) throw std::out_of_range("Key not found");

            if (prev->mPair.first == key) {
                mBuckets[bucket] = prev->mNextNode;
                delete prev;
                mCount--;
                return;
            }

            while (prev != nullptr && prev->mNextNode->mPair.first != key) prev = prev->mNextNode;
            if (prev == nullptr) throw std::out_of_range("Key not found");
            prev->mNextNode = prev->mNextNode->mNextNode;
            mCount--;
            delete prev;
        }

        void remove(const const_iterator& it) {
            if (it == end())
                throw std::out_of_range("Erasing begin");

            BucketNode* prev;
            prev = mBuckets[it.mBucket];

            if (prev == it.mNode) {
                mBuckets[it.mBucket] = prev->mNextNode;
                delete prev;
                mCount--;
                return;
            }

            while (prev->mNextNode != it.mNode) prev = prev->mNextNode;
            prev->mNextNode = prev->mNextNode->mNextNode;
            mCount--;
            delete prev;
        }

        size_type getSize() const {
            return mCount;
        }

        bool operator==(const HashMap& other) const {
            if (mCount != other.mCount || mBucketCount != other.mBucketCount)
                return false;

            for (size_type i = 0; i < mBucketCount; ++i) {
                if ((mBuckets[i] == nullptr && other.mBuckets[i] != nullptr) ||
                    (mBuckets[i] != nullptr && other.mBuckets[i] == nullptr))
                    return false;

                /* Buckets full, compare */
                BucketNode* node = mBuckets[i];
                while (node != nullptr) {
                    if (other.find(node->mPair.first)->second != node->mPair.second)
                        return false;
                    node = node->mNextNode;
                }
            }

            return true;
        }

        bool operator!=(const HashMap& other) const {
            return !(*this == other);
        }

        iterator begin() {
            return Iterator(*this, 0, mBuckets[0]);
        }

        iterator end() {
            return Iterator(*this, mBucketCount, nullptr);
        }

        const_iterator cbegin() const {
            return ConstIterator(*this, 0, mBuckets[0]);
        }

        const_iterator cend() const {
            return ConstIterator(*this, mBucketCount, nullptr);
        }

        const_iterator begin() const {
            return cbegin();
        }

        const_iterator end() const {
            return cend();
        }

    private:
        size_type mBucketCount;
        size_type mCount;
        BucketNode** mBuckets;
        std::function<size_type(const key_type&)> mHasher;

        size_type bucketHash(const key_type& pKey) const {
            return mHasher(pKey) % mBucketCount;
        }

        size_type hash(const key_type& pKey) const {
            return mHasher(pKey);
        }

        iterator insert(const key_type& pKey, mapped_type pValue) {
            size_type bucket = bucketHash(pKey);
            BucketNode* newValue = new BucketNode(pKey, pValue);
            if (mBuckets[bucket] != nullptr)
                newValue->mNextNode = mBuckets[bucket];
            mBuckets[bucket] = newValue;
            mCount++;
            return Iterator(*this, bucket, newValue);
        };

        iterator insert(const key_type& pKey) {
            size_type bucket = bucketHash(pKey);
            BucketNode* newValue = new BucketNode(pKey);
            if (mBuckets[bucket] != nullptr)
                newValue->mNextNode = mBuckets[bucket];
            mBuckets[bucket] = newValue;
            mCount++;
            return Iterator(*this, bucket, newValue);
        };

        void clear() {
            BucketNode* node;
            BucketNode* tmp_node;

            for (size_type i = 0; i < mBucketCount; ++i) {
                node = mBuckets[i];
                while (node != nullptr) {
                    tmp_node = node;
                    node = node->mNextNode;
                    delete tmp_node;
                    mCount--;
                }
            }
        };

    };

    template<typename KeyType, typename ValueType>
    struct HashMap<KeyType, ValueType>::BucketNode {
        value_type mPair;
        BucketNode* mNextNode;

        BucketNode(const key_type& pKey) : mPair(std::make_pair(pKey, ValueType{})), mNextNode(nullptr) { }

        BucketNode(const key_type& pKey, mapped_type pData) : mPair(std::make_pair(pKey, pData)), mNextNode(nullptr) { }

        BucketNode(value_type pPair) : mPair(pPair), mNextNode(nullptr) { }
    };


    template<typename KeyType, typename ValueType>
    class HashMap<KeyType, ValueType>::ConstIterator {
    public:
        using reference = typename HashMap::const_reference;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename HashMap::value_type;
        using pointer = const typename HashMap::value_type*;

        friend class HashMap;

        explicit ConstIterator(const HashMap<KeyType, ValueType>& pMap, size_type pBucket, BucketNode* pNode) : mMap(
                pMap), mBucket(pBucket), mNode(pNode) {
            while (mNode == nullptr && mBucket != mMap.mBucketCount) {
                mBucket++;
                mNode = mMap.mBuckets[mBucket];
            }
        }

        ConstIterator(const ConstIterator& other) : mMap(other.mMap), mBucket(other.mBucket), mNode(other.mNode) { }

        ConstIterator& operator++() {
            if (mNode == nullptr)
                throw std::out_of_range("Incrementing end iterator");
            mNode = mNode->mNextNode;
            while (mNode == nullptr && mBucket != mMap.mBucketCount) {
                mBucket++;
                mNode = mMap.mBuckets[mBucket];
            }
            return *this;
        }

        ConstIterator operator++(int) {
            ConstIterator ret(*this);
            operator++();
            return ret;
        }

        ConstIterator& operator--() {

            if (mMap.mBuckets[mBucket] == mNode) {
                while (mMap.mBuckets[mBucket] == nullptr && mBucket > 0) mBucket--;
                mNode = mMap.mBuckets[mBucket];
                if (mBucket < 0 || mNode == nullptr)
                    throw std::out_of_range("Decrementing begin iterator");
                while (mNode->mNextNode != nullptr) mNode = mNode->mNextNode;

            } else {
                BucketNode* node = mMap.mBuckets[mBucket];
                while (node->mNextNode != mNode) node = node->mNextNode;
                mNode = node;
            }
            return *this;
        }

        ConstIterator operator--(int) {
            ConstIterator ret(*this);
            operator--();
            return ret;
        }

        reference operator*() const {
            if (mNode == nullptr)
                throw std::out_of_range("Dereferencing end iterator");
            return mNode->mPair;
        }

        pointer operator->() const {
            return &this->operator*();
        }

        bool operator==(const ConstIterator& other) const {
            return (mNode == other.mNode && mBucket == other.mBucket);
        }

        bool operator!=(const ConstIterator& other) const {
            return !(*this == other);

        }

    private:
        const HashMap& mMap;
        size_type mBucket;
        BucketNode* mNode;
    };

    template<typename KeyType, typename ValueType>
    class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator {
    public:
        using reference = typename HashMap::reference;
        using pointer = typename HashMap::value_type*;


        explicit Iterator(const HashMap<KeyType, ValueType>& pMap, size_type pBucket, BucketNode* pNode)
                : ConstIterator(pMap, pBucket, pNode) { }

        Iterator(const ConstIterator& other) : ConstIterator(other) { }

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

#endif /* AISDI_MAPS_HASHMAP_H */
