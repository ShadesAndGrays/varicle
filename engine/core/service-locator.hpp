#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <memory>
#include <typeindex>
#include <vector>

namespace varicle {

class ServiceLocator {
  private:
    template <typename T> struct ServiceSlot {
        static inline std::unique_ptr<T> instance;         // for ownership
        static inline T*                 access = nullptr; // for quicker access
    };

    struct ServiceEraser {
        std::type_index       type_idx;
        std::function<void()> callback;
    };

    // Track registration order to ensure safe LIFO destruction
    static inline std::vector<ServiceEraser> s_erasers;

  public:
    // pass ownership of a unique_ptr to the locator
    template <typename T> static void provide(std::unique_ptr<T> service) {
        if (has<T>()) {
            remove<T>();
        }

        ServiceSlot<T>::instance = std::move(service);
        ServiceSlot<T>::access   = ServiceSlot<T>::instance.get();
        s_erasers.emplace_back(std::type_index(typeid(T)), []() {
            ServiceSlot<T>::access = nullptr;
            ServiceSlot<T>::instance.reset();
        });
    }

    template <typename T> static bool has() {
        return ServiceSlot<T>::access != nullptr;
    }

    template <typename T> static T& get() {
        assert(has<T>() && "Attempted to access unregistered service!");
        return *ServiceSlot<T>::access;
    }

    template <typename T> static T* get_ptr() { return ServiceSlot<T>::access; }

    // Explicitly destroy a specific service
    template <typename T> static void remove() {
        ServiceSlot<T>::access = nullptr;
        ServiceSlot<T>::instance.reset();
        std::erase_if(s_erasers, [](const ServiceEraser& e) -> bool {
            return std::type_index(typeid(T)) == e.type_idx;
        });
    }

    static inline void shutdown() {
        for (auto it = s_erasers.rbegin(); it != s_erasers.rend(); ++it) {
            it->callback();
        }
        s_erasers.clear();
    }
};
}; // namespace varicle
