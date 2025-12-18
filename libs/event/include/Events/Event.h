#pragma once
#include <functional>
#include <vector>

template <typename R, typename... Args> class Event {
  public:
    using Callback = std::function<R(Args...)>;

    // Subscribe a member function with an object pointer
    template <typename T> void Subscribe(T *obj, R (T::*method)(Args...)) {
        callbacks.emplace_back([obj, method](Args... args) -> R {
            return (obj->*method)(std::forward<Args>(args)...);
        });
    }

    // Subscribe a const member function with an object pointer
    template <typename T>
    void Subscribe(const T *obj, R (T::*method)(Args...) const) {
        callbacks.emplace_back([obj, method](Args... args) -> R {
            return (obj->*method)(std::forward<Args>(args)...);
        });
    }

    // Subscribe a free/static function pointer
    void Subscribe(R (*func)(Args...)) {
        callbacks.emplace_back(func);
    }

    // Emit to all subscribers
    void Emit(Args... args) {
        for (auto &cb : callbacks) {
            cb(std::forward<Args>(args)...);
        }
    }
    protected :
    private :
        std::vector<Callback>                            callbacks;
};
