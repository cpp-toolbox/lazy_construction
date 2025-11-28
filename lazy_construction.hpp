#ifndef LAZY_CONSTRUCTION_HPP
#define LAZY_CONSTRUCTION_HPP

#include <memory>
#include <tuple>
#include <utility>

/**
 * @brief Wrapper for a type which only constructs the object upon first access.
 *
 * This template allows defining objects that are constructed
 * only on first use. This can be used to avoid static initialization order issues.
 *
 * @tparam T The type of the object to create.
 * @tparam Args... The types of constructor arguments for T.
 *
 * One use case of this class can be when you want to have multiple global variables going on. The problem is if one
 * depends on the other existing and additionally they are defined in different translation units then it's impossible
 * to guarantee the initialization order of the variables which can lead to segfaults because you tried to use a
 * different global which was not yet initialized. This is all known as the "static initialization order fiasco".
 *
 * This class provides a solution to the fiasco by making sure that it gets constructed when its first accessed,
 * regardless of other globals.
 *
 * @note This template uses a parameter pack to store constructor arguments internally, and only initializes the real
 * object upon first usage.
 *
 * Example:
 * @code
 * LazyConstruction<MyData, std::string, int> global_data("Name", 42);
 * global_data->print(); // constructs MyData("Name", 42) on first access
 * @endcode
 */
template <typename T, typename... Args> struct LazyConstruction {
    std::unique_ptr<T> ptr;   ///< Pointer to the lazily-constructed object
    std::tuple<Args...> args; ///< Stored constructor arguments

    LazyConstruction(Args... constructor_args) : args(std::make_tuple(constructor_args...)) {}

    T *operator->() {
        if (!ptr) {
            ptr = std::apply([](auto &&...a) { return std::make_unique<T>(std::forward<decltype(a)>(a)...); }, args);
        }
        return ptr.get();
    }

    T &operator*() {
        if (!ptr) {
            ptr = std::apply([](auto &&...a) { return std::make_unique<T>(std::forward<decltype(a)>(a)...); }, args);
        }
        return *ptr;
    }
};

#endif // LAZY_CONSTRUCTION_HPP
