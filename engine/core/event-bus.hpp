#pragma once

#include "engine/util/slot-map.hpp"
#include <functional>
#include <memory>
#include <typeindex>
#include <utility>
#include <vector>

namespace varicle::event {

using ConnectionID = structures::SlotID;

// TODO: Look into EASTL for optimization fixed_function callback
template <typename T> using EventCallback = std::function<void(const T&)>;

struct IListener {
    virtual ~IListener() = default;
};

template <typename Event> struct Listener : IListener {
    EventCallback<Event> callback;
    explicit Listener(EventCallback<Event> cb) : callback(std::move(cb)) {}
};

class EventBus {

  private:
    std::vector<std::function<void()>> event_queue;
    std::unordered_map<
        std::type_index,
        structures::SlotMap<std::unique_ptr<IListener>>>
        m_listeners;

  public:
    template <typename Event> void publish(const Event& event) {
        auto type_idx = std::type_index(typeid(Event)); // get index
        auto it       = m_listeners.find(type_idx);

        if (it != m_listeners.end()) { // has been subscribed two at least once
            for (const auto& listener_base : it->second.get_data()) {
                if (auto listener =
                        static_cast<Listener<Event>*>(listener_base.get())) {
                    listener->callback(event); // call with event
                }
            }
        }
    }

    template <typename Event>
    ConnectionID subscribe(EventCallback<Event> event_callback) {
        auto type_idx = std::type_index(typeid(Event)); // get index
        return m_listeners[type_idx].emplace(
            std::make_unique<Listener<Event>>(std::move(event_callback))
        );
    }

    template <typename Event> bool unsubscribe(ConnectionID connection_id) {
        auto type_idx = std::type_index(typeid(Event)); // get index
        auto it       = m_listeners.find(type_idx);
        if (it != m_listeners.end()) { // has been subscribed two at least once
            return it->second.remove(connection_id);
        }
        return false;
    }

    template <typename Event> void defer(Event event) {
        event_queue.emplace_back([this, event = std::move(event)]() {
            publish<Event>(event);
        });
    }

    // run all defefferd queue_events
    void flush() {
        std::vector<std::function<void()>> processing_queue;
        processing_queue.swap(event_queue);
        for (auto& callback : processing_queue) {
            callback();
        }
    }
};

} // namespace varicle::event
