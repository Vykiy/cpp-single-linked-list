#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <iostream>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
                : value(val)
                , next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    SingleLinkedList()=default;
    ~SingleLinkedList(){
        Clear();
    };

    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;
        explicit BasicIterator(Node* node): node_(node) {
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other)noexcept {
            node_=other.node_;
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_==nullptr ? true:rhs.node_==this->node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return rhs.node_!=this->node_;
        }

        bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_==nullptr? true:rhs.node_==this->node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return rhs.node_!=this->node_;
        }

        BasicIterator& operator++() noexcept {
            if (node_!=nullptr && node_->next_node!=nullptr ){
                node_=node_->next_node;
                return *this;
            }
            node_=nullptr;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            if (node_!=nullptr){
                auto old(*this);
                node_=node_->next_node->next_node;
                return old;
            }
            return *this;
        }

        [[nodiscard]] reference operator*() const noexcept {
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            return &node_->value;
        }

    private:
        Node* node_ = nullptr;
    };

    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    [[nodiscard]] Iterator begin() noexcept {
        if (size_==0) {
            return Iterator{nullptr};
        }
        return Iterator{head_.next_node};
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator{nullptr};
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        if (size_==0) {
            return Iterator{nullptr};
        }
        return ConstIterator{head_.next_node};
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator{nullptr};
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return begin();
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return end();
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator(&head_);
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        auto link = const_cast<Node*>(&head_);
        return ConstIterator(link);
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return cbefore_begin();
    }

    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_==0 ? true: false;
    }

    SingleLinkedList(std::initializer_list<Type> values) {
        if(values.size()){
            SingleLinkedList tmp;
            for (const auto& value : values){
                tmp.PushFront(value);
            }
            for (const auto& value : tmp){
                (*this).PushFront(value);
            }
        }
    }

    SingleLinkedList(const SingleLinkedList& other) {
        assert(size_ == 0 && head_.next_node == nullptr);
        SingleLinkedList tmp_r;
        SingleLinkedList tmp;
        for (auto value : other){
            tmp_r.PushFront(value);
        }
        for (auto value : tmp_r){
            tmp.PushFront(value);
        }
        swap(tmp);
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        auto tmp(rhs);
        swap(tmp);
        return *this;
    }

    void swap(SingleLinkedList& other) noexcept {
        const auto current= head_.next_node;
        const auto current_s=size_;
        head_.next_node=other.head_.next_node;
        other.head_.next_node=current;
        size_=other.size_;
        other.size_=current_s;
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        if (pos==before_begin()){
            head_.next_node = new Node(value,head_.next_node);
            ++size_;
            return Iterator{head_.next_node};
        }
        pos.node_->next_node=new Node(value,pos.node_->next_node);
        ++size_;
        return Iterator{pos.node_->next_node};
    }

    void PushFront(const Type& val) {
        InsertAfter(before_begin(),val);
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        if(head_.next_node==nullptr){
            return end();
        }
        if(pos==before_begin()){
            auto to_delete=head_.next_node;
            head_.next_node=head_.next_node->next_node;
            delete to_delete;
            --size_;
            return Iterator{head_.next_node};
        }
        auto to_delete=pos.node_->next_node;
        auto shifted=pos.node_->next_node->next_node;
        pos.node_->next_node=shifted;
        delete to_delete;
        --size_;
        return Iterator{shifted};
    }

    void PopFront() noexcept {
        EraseAfter(before_begin());
    }

    void Clear() noexcept{
        while(head_.next_node!=nullptr){
            PopFront();
        }
    }

private:
    Node head_=Node();
    size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return lhs.GetSize()==rhs.GetSize() && std::equal(lhs.begin(),lhs.end(),rhs.begin(),lhs.end()) ;
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs==rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(),lhs.end(),rhs.begin(),rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    // Заглушка. Реализуйте сравнение самостоятельно
    return lhs<rhs || lhs==rhs;
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs<rhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return lhs>rhs || lhs==rhs;
}