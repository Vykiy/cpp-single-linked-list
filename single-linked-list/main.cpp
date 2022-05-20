#include <cassert>
#include <cstddef>
#include <string>
#include <utility>
#include <iostream>
#include <memory>

template<typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;

        Node(const Type &val, Node *next)
                : value(val), next_node(next) {
        }

        Type value;
        Node *next_node = nullptr;
    };

    template<typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;

        explicit BasicIterator(Node *node) : node_(node) {
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type> &other) noexcept {
            node_ = other.node_;
        }

        BasicIterator &operator=(const BasicIterator &rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type> &rhs) const noexcept {
            return rhs.node_ == this->node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type> &rhs) const noexcept {
            return rhs.node_ != this->node_;
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type> &rhs) const noexcept {
            return rhs.node_ == this->node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type> &rhs) const noexcept {
            return rhs.node_ != this->node_;
        }

        BasicIterator &operator++() {
            assert(node_ == nullptr || node_->next_node == nullptr);
//            if (node_ != nullptr && node_->next_node != nullptr) {
//                node_ = node_->next_node;
//                return *this;
//            }
//            node_ = nullptr;
//            return *this;

            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            if (node_ != nullptr) {
                auto old(*this);
                node_ = node_->next_node->next_node;
                return old;
            }
            return *this;
        }

        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ == nullptr);
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            assert(&node_ == nullptr);
            return &node_->value;
        }

    private:
        Node *node_ = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type &;
    using const_reference = const value_type &;

    SingleLinkedList() = default;

    SingleLinkedList(std::initializer_list<Type> values) {
        Assign(values.begin(), values.end());  // Может бросить исключение
    }

    ~SingleLinkedList() {
        Clear();
    }

    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{head_.next_node};
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator{nullptr};
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
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
        auto link = const_cast<Node *>(&head_);
        return ConstIterator(link);
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return cbefore_begin();
    }

    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    SingleLinkedList &operator=(const SingleLinkedList &rhs) {
        auto tmp(rhs);
        swap(tmp);
        return *this;
    }

    void swap(SingleLinkedList &other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

    Iterator InsertAfter(ConstIterator pos, const Type &value) {
        assert(pos.node_ == nullptr);
        pos.node_->next_node = new Node(value, pos.node_->next_node);
        ++size_;
        return Iterator{pos.node_->next_node};
    }

    void PushFront(const Type &val) {
        InsertAfter(before_begin(), val);
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert(pos.node_ == nullptr || pos.node_->next_node == nullptr);
        auto to_delete = pos.node_->next_node;
        auto shifted = pos.node_->next_node->next_node;
        pos.node_->next_node = shifted;
        delete to_delete;
        --size_;
        return Iterator{shifted};
    }

    void PopFront() noexcept {
        EraseAfter(before_begin());
    }

    void Clear() noexcept {
        while (head_.next_node != nullptr) {
            PopFront();
        }
    }

private:
    Node head_ = Node();
    size_t size_ = 0;

    template <typename InputIterator>
    void Assign(InputIterator from, InputIterator to) {
        // Создаём временный список, в который будем добавлять элементы из диапазона [from, to)
        // Если в процессе добавления будет выброшено исключение,
        // его деструктор подчистит всю память
        SingleLinkedList<Type> tmp;

        // Элементы будут записываться начиная с указателя на первый узел
        Node** node_ptr = &tmp.head_.next_node;
        while (from != to) {
            // Ожидается, что текущий указатель - нулевой
            assert(*node_ptr == nullptr);

            // Создаём новый узел и записываем его адрес в указатель текущего узла
            *node_ptr = new Node(*from, nullptr);
            ++tmp.size_;

            // Теперь node_ptr хранит адрес указателя на следующий узел
            node_ptr = &((*node_ptr)->next_node);

            // Переходим к следующему элементу диапазона
            ++from;
        }

        // Теперь, когда tmp содержит копию элементов диапазона [from, to),
        // можно совершить обмен данными текущего объекта и tmp
        swap(tmp);
        // Теперь текущий список содержит копию элементов диапазона [from, to),
        // а tmp - прежнее значение текущего списка
    }
};

template<typename Type>
void swap(SingleLinkedList<Type> &lhs, SingleLinkedList<Type> &rhs) noexcept {
    lhs.swap(rhs);
}

template<typename Type>
bool operator==(const SingleLinkedList<Type> &lhs, const SingleLinkedList<Type> &rhs) {
    return lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), lhs.end());
}

template<typename Type>
bool operator!=(const SingleLinkedList<Type> &lhs, const SingleLinkedList<Type> &rhs) {
    return !(lhs == rhs);
}

template<typename Type>
bool operator<(const SingleLinkedList<Type> &lhs, const SingleLinkedList<Type> &rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename Type>
bool operator<=(const SingleLinkedList<Type> &lhs, const SingleLinkedList<Type> &rhs) {
    // Заглушка. Реализуйте сравнение самостоятельно
    return lhs < rhs || lhs == rhs;
}

template<typename Type>
bool operator>(const SingleLinkedList<Type> &lhs, const SingleLinkedList<Type> &rhs) {
    return !(lhs < rhs);
}

template<typename Type>
bool operator>=(const SingleLinkedList<Type> &lhs, const SingleLinkedList<Type> &rhs) {
    return lhs > rhs || lhs == rhs;
}