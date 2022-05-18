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

    ~SingleLinkedList() {
        Clear();
    };

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

    SingleLinkedList(std::initializer_list<Type> values) {
        if (values.size() != 0) {
            SingleLinkedList tmp;
            for (const auto &value: values) {
                tmp.PushFront(value);
            }
//            for (const auto &value: tmp) {
//                (*this).PushFront(value);
//            }
            swap(tmp);
        }
    }

    SingleLinkedList(const SingleLinkedList &other) {
        assert(size_ == 0 && head_.next_node == nullptr);
        SingleLinkedList tmp;
        for (auto value: other) {
            tmp.InsertAfter(value);
        }
        swap(tmp);
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

void Test4() {
    struct DeletionSpy {
        ~DeletionSpy() {
            if (deletion_counter_ptr) {
                ++(*deletion_counter_ptr);
            }
        }
        int* deletion_counter_ptr = nullptr;
    };

    // Проверка PopFront
    {
        SingleLinkedList<int> numbers{3, 14, 15, 92, 6};
        numbers.PopFront();
        assert((numbers == SingleLinkedList<int>{14, 15, 92, 6}));

        SingleLinkedList<DeletionSpy> list;
        list.PushFront(DeletionSpy{});
        int deletion_counter = 0;
        list.begin()->deletion_counter_ptr = &deletion_counter;
        assert(deletion_counter == 0);
        list.PopFront();
        assert(deletion_counter == 1);
    }

    // Доступ к позиции, предшествующей begin
    {
        SingleLinkedList<int> empty_list;
        const auto& const_empty_list = empty_list;
        assert(empty_list.before_begin() == empty_list.cbefore_begin());
        assert(++empty_list.before_begin() == empty_list.begin());
        assert(++empty_list.cbefore_begin() == const_empty_list.begin());

        SingleLinkedList<int> numbers{1, 2, 3, 4};
        const auto& const_numbers = numbers;
        assert(numbers.before_begin() == numbers.cbefore_begin());
        assert(++numbers.before_begin() == numbers.begin());
        assert(++numbers.cbefore_begin() == const_numbers.begin());
    }

    // Вставка элемента после указанной позиции
    {  // Вставка в пустой список
        {
            SingleLinkedList<int> lst;
            const auto inserted_item_pos = lst.InsertAfter(lst.before_begin(), 123);
            assert((lst == SingleLinkedList<int>{123}));
            assert(inserted_item_pos == lst.begin());
            assert(*inserted_item_pos == 123);
        }

        // Вставка в непустой список
        {
            SingleLinkedList<int> lst{1, 2, 3};
            auto inserted_item_pos = lst.InsertAfter(lst.before_begin(), 123);

            assert(inserted_item_pos == lst.begin());
            assert(inserted_item_pos != lst.end());
            assert(*inserted_item_pos == 123);
            assert((lst == SingleLinkedList<int>{123, 1, 2, 3}));

            inserted_item_pos = lst.InsertAfter(lst.begin(), 555);
            assert(++SingleLinkedList<int>::Iterator(lst.begin()) == inserted_item_pos);
            assert(*inserted_item_pos == 555);
            assert((lst == SingleLinkedList<int>{123, 555, 1, 2, 3}));
        };
    }

    // Вспомогательный класс, бросающий исключение после создания N-копии
    struct ThrowOnCopy {
        ThrowOnCopy() = default;
        explicit ThrowOnCopy(int& copy_counter) noexcept
                : countdown_ptr(&copy_counter) {
        }
        ThrowOnCopy(const ThrowOnCopy& other)
                : countdown_ptr(other.countdown_ptr)  //
        {
            if (countdown_ptr) {
                if (*countdown_ptr == 0) {
                    throw std::bad_alloc();
                } else {
                    --(*countdown_ptr);
                }
            }
        }
        // Присваивание элементов этого типа не требуется
        ThrowOnCopy& operator=(const ThrowOnCopy& rhs) = delete;
        // Адрес счётчика обратного отсчёта. Если не равен nullptr, то уменьшается при каждом копировании.
        // Как только обнулится, конструктор копирования выбросит исключение
        int* countdown_ptr = nullptr;
    };

    // Проверка обеспечения строгой гарантии безопасности исключений
    {
        bool exception_was_thrown = false;
        for (int max_copy_counter = 10; max_copy_counter >= 0; --max_copy_counter) {
            SingleLinkedList<ThrowOnCopy> list{ThrowOnCopy{}, ThrowOnCopy{}, ThrowOnCopy{}};
            try {
                int copy_counter = max_copy_counter;
                list.InsertAfter(list.cbegin(), ThrowOnCopy(copy_counter));
                assert(list.GetSize() == 4u);
            } catch (const std::bad_alloc&) {
                exception_was_thrown = true;
                assert(list.GetSize() == 3u);
                break;
            }
        }
        assert(exception_was_thrown);
    }

    // Удаление элементов после указанной позиции
    {
        {
            SingleLinkedList<int> lst{1, 2, 3, 4};
            const auto& const_lst = lst;
            const auto item_after_erased = lst.EraseAfter(const_lst.cbefore_begin());
            assert((lst == SingleLinkedList<int>{2, 3, 4}));
            assert(item_after_erased == lst.begin());
        }
        {
            SingleLinkedList<int> lst{1, 2, 3, 4};
            const auto item_after_erased = lst.EraseAfter(lst.cbegin());
            assert((lst == SingleLinkedList<int>{1, 3, 4}));
            assert(item_after_erased == (++lst.begin()));
        }
        {
            SingleLinkedList<int> lst{1, 2, 3, 4};
            const auto item_after_erased = lst.EraseAfter(++(++lst.cbegin()));
            assert((lst == SingleLinkedList<int>{1, 2, 3}));
            assert(item_after_erased == lst.end());
        }
        {
            SingleLinkedList<DeletionSpy> list{DeletionSpy{}, DeletionSpy{}, DeletionSpy{}};
            auto after_begin = ++list.begin();
            int deletion_counter = 0;
            after_begin->deletion_counter_ptr = &deletion_counter;
            assert(deletion_counter == 0u);
            list.EraseAfter(list.cbegin());
            assert(deletion_counter == 1u);
        }
    }
}

int main() {
    Test4();
}