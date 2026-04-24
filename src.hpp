#ifndef SRC_HPP
#define SRC_HPP

#include <stdexcept>
#include <typeinfo>
#include <typeindex>
#include <memory>
#include <unordered_map>

namespace sjtu {

class any_ptr {
private:
    struct base_holder {
        virtual ~base_holder() = default;
        virtual std::type_index type() const = 0;
        virtual void* data() = 0;
    };

    template<typename T>
    struct holder : base_holder {
        T value;
        
        holder(const T& v) : value(v) {}
        holder(T&& v) : value(std::move(v)) {}
        
        template<typename... Args>
        holder(Args&&... args) : value(std::forward<Args>(args)...) {}
        
        std::type_index type() const override {
            return std::type_index(typeid(T));
        }
        
        void* data() override {
            return &value;
        }
    };

    std::shared_ptr<base_holder> ptr;

public:
    /**
     * @brief 默认构造函数，行为应与创建空指针类似
     * 
     */
    any_ptr() : ptr(nullptr) {}

    /**
     * @brief 拷贝构造函数，要求拷贝的层次为浅拷贝，即该对象与被拷贝对象的内容指向同一块内存
     * @note 若将指针作为参数传入，则指针的生命周期由该对象管理
     * @example
     *  any_ptr a = make_any_ptr(1);
     *  any_ptr b = a;
     *  a.unwrap<int> = 2;
     *  std::cout << b << std::endl; // 2
     * @param other
     */
    any_ptr(const any_ptr &other) : ptr(other.ptr) {}
    
    template <class T>
    any_ptr(T *ptr) : ptr(std::make_shared<holder<T>>(*ptr)) {}

    /**
     * @brief 析构函数，注意若一块内存被多个对象共享，那么只有最后一个析构的对象需要释放内存
     * @example
     *  any_ptr a = make_any_ptr(1);
     *  {
     *    any_ptr b = a;
     *  }
     *  std::cout << a << std::endl; // valid
     * @example
     *  int *p = new int(1);
     *  {
     *    any_ptr a = p;
     *  }
     *  std::cout << *p << std::endl; // invalid
     * 
     */
    ~any_ptr() = default;

    /**
     * @brief 拷贝赋值运算符，要求拷贝的层次为浅拷贝，即该对象与被拷贝对象的内容指向同一块内存
     * @note 应与指针拷贝赋值运算符的语义相近
     * @param other
     * @return any_ptr&
     */
    any_ptr &operator=(const any_ptr &other) {
        if (this != &other) {
            ptr = other.ptr;
        }
        return *this;
    }
    
    template <class T>
    any_ptr &operator=(T *ptr) {
        this->ptr = std::make_shared<holder<T>>(*ptr);
        return *this;
    }

    /**
     * @brief 获取该对象指向的值的引用
     * @note 若该对象指向的值不是 T 类型，则抛出异常 std::bad_cast
     * @example
     *  any_ptr a = make_any_ptr(1);
     *  std::cout << a.unwrap<int>() << std::endl; // 1
     * @tparam T
     * @return T&
     */
    template <class T>
    T &unwrap() {
        if (!ptr) {
            throw std::bad_cast();
        }
        if (ptr->type() != std::type_index(typeid(T))) {
            throw std::bad_cast();
        }
        return *static_cast<T*>(ptr->data());
    }
    
    template <class T>
    const T &unwrap() const {
        if (!ptr) {
            throw std::bad_cast();
        }
        if (ptr->type() != std::type_index(typeid(T))) {
            throw std::bad_cast();
        }
        return *static_cast<const T*>(ptr->data());
    }
};

/**
 * @brief 由指定类型的值构造一个 any_ptr 对象
 * @example
 *  any_ptr a = make_any_ptr(1);
 *  any_ptr v = make_any_ptr<std::vector<int>>(1, 2, 3);
 *  any_ptr m = make_any_ptr<std::map<int, int>>({{1, 2}, {3, 4}});
 * @tparam T
 * @param t
 * @return any_ptr
 */
template <class T>
any_ptr make_any_ptr(const T &t) {
    return any_ptr(std::make_shared<any_ptr::holder<T>>(t));
}

template <class T, class... Args>
any_ptr make_any_ptr(Args&&... args) {
    return any_ptr(std::make_shared<any_ptr::holder<T>>(std::forward<Args>(args)...));
}

template <class T>
any_ptr make_any_ptr(std::initializer_list<typename T::value_type> il) {
    return any_ptr(std::make_shared<any_ptr::holder<T>>(il));
}

}  // namespace sjtu

#endif
