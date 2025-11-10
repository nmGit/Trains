#pragma once
#include <functional>
#include <vector>


template <typename R, typename... Args> class Event {
  public:
    using Callback = std::function<R(Args...)>;

    void Subscribe(Callback callback) {
        callbacks.push_back(callback);
    }

    void Unsubscribe(Callback callback) {
        callbacks.erase(
            std::remove(callbacks.begin(), callbacks.end(), callback),
            callbacks.end());
    }

    void Emit(Args... args) {
        for (auto &callback : callbacks) {
            callback(args...);
        }
    }

  protected:
  private:
    std::vector<Callback> callbacks;
};
