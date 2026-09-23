#pragma once

#include <cassert>
#include <cstdint>
#include <format>
#include <iostream>
#include <utility>
#include <vector>

namespace varicle::structures {

struct SlotID {
    uint32_t index;
    uint32_t generation;

    bool operator==(const SlotID& other) {
        return (index == other.index) && (generation == other.generation);
    }

    bool operator==(const SlotID& other) const {
        return (index == other.index) && (generation == other.generation);
    }

    friend std::ostream& operator<<(std::ostream& os, const SlotID& id) {
        return os << std::format("(idx: {} gen: {}) ", id.index, id.generation);
    }

    void print() const {
        std::cout << std::format("(idx: {} gen: {}) ", index, generation)
                  << "\n";
    }

    std::string to_string() const {
        return std::format("idx: {} gen: {} ", index, generation);
    }

    bool is_valid() { return index != std::numeric_limits<uint32_t>().max(); }
};

template <typename T> class SlotMap {

    struct Slot {
        std::uint32_t dense_index;
        std::uint32_t generation;
    };

    std::vector<Slot>
        slots; // holds a mapping to our data and tracks generation
    std::vector<std::uint32_t>
        free_list; // Stores list of available entries in the index table
    std::vector<T>             data;  // Holds are actual data
    std::vector<std::uint32_t> erase; // maps data to index

  public:
    const std::vector<T>& get_data() const { return data; }
    std::vector<T>&       get_data() { return data; }

    T* get(SlotID id) {
        if (id.index >= slots.size() || !id.is_valid()) {
            return nullptr;
        }

        const Slot& slot = slots[id.index];

        if (slot.generation != id.generation) {
            return nullptr;
        }
        return &data[slot.dense_index];
    }

    const T* get(SlotID id) const {
        if (id.index >= slots.size() || !id.is_valid()) {
            return nullptr;
        }

        const Slot& slot = slots[id.index];

        if (slot.generation != id.generation) {
            return nullptr;
        }
        return &data[slot.dense_index];
    }

    T& operator[](SlotID id) {
        T* ptr = get(id);
        assert(ptr != nullptr && "Invalid ID Passed to SlotMap::operator[]");
        return *ptr;
    }

    const T& operator[](SlotID id) const {
        const T* ptr = get(id);
        assert(ptr != nullptr && "Invalid ID Passed to SlotMap::operator[]");
        return *ptr;
    }

    template <typename... Args> SlotID emplace(Args&&... args) {
        SlotID   new_id{};
        uint32_t slot_index = 0;
        if (free_list.empty()) {
            slot_index        = static_cast<uint32_t>(slots.size());
            new_id.index      = slot_index;
            new_id.generation = 0;

            slots.emplace_back(
                Slot{ .dense_index = static_cast<uint32_t>(data.size()),
                      .generation  = 0 }
            );

        } else {
            slot_index = free_list.back();
            free_list.pop_back();
            Slot& slot        = slots[slot_index];
            new_id.index      = slot_index;
            new_id.generation = slot.generation;
            slot.dense_index  = data.size();
        }

        data.emplace_back(std::forward<Args>(args)...);
        erase.emplace_back(slot_index);

        return new_id;
    }

    SlotID push(T val) { return emplace(std::move(val)); }

    bool remove(const SlotID& id) {
        if (get(id)) {
            Slot&      slot = slots[id.index]; // Get the target slot
            const auto slot_to_update =
                erase.back(); // Retrieve slot that points to the data block we
                              // are about to swap with

            if (id.index != slot_to_update) {
                std::swap(data[slot.dense_index], data.back()); // swap data
                std::swap(
                    erase[slot.dense_index],
                    erase.back()
                ); // swap erase to match changes in data
                slots[slot_to_update].dense_index =
                    slot.dense_index; // tell the slot that it's data block just
                // moved to the swapped index
            }

            data.pop_back();                  // remove data
            erase.pop_back();                 // remove erase to match changes
            free_list.emplace_back(id.index); // free real estate
            slot.generation++; // to avoid collisions we update now
            slot.dense_index =
                std::numeric_limits<uint32_t>()
                    .max(); // and we can do this just readability. The data has
                            // been removed
            return true;
        }
        return false;
    }

    void print() const {
        std::cout << "Slots \n";
        for (const auto& slot : slots) {
            std::cout << std::format(
                "(idx: {} gen: {}) ", slot.dense_index, slot.generation
            );
        }
        std::cout << "\nData \n";
        for (const auto& val : data) {

            std::cout << std::format("(val: {} ) ", val);
        }
        std::cout << "\nErase \n";

        for (const auto& i : erase) {
            std::cout << std::format("(idx: {} ) ", i);
        }

        std::cout << "\nFree Slots\n";

        for (const auto& i : free_list) {
            std::cout << std::format("(idx: {} ) ", i);
        }
        std::cout << "\n";
    }
};

} // namespace varicle::structures
