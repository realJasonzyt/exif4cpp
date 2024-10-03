#pragma once
#include "types.h"
#include <stdexcept>
#include <string>

namespace exif {

class Value {

    union {
        uint64_t     bytes_;
        std::string* string_;
    } storge_       = {0};
    ValueType type_ = ValueType::kUndefined;

public:
    void copy(const Value& other) {
        if (other.type_ == ValueType::kAscii) {
            storge_.string_ = new std::string(*other.storge_.string_);
        } else {
            storge_.bytes_ = other.storge_.bytes_;
        }
        type_ = other.type_;
    }
    void move(Value&& other) {
        storge_.string_       = other.storge_.string_;
        type_                 = other.type_;
        other.storge_.string_ = nullptr;
        other.type_           = ValueType::kUndefined;
    }

    Value() = default;
    Value(std::string str) {
        storge_.string_ = new std::string(std::move(str));
        type_           = ValueType::kAscii;
    }
    template <typename T>
        requires std::is_arithmetic_v<T>
    Value(T val) {
        static_assert(sizeof(val) <= 8);
        if constexpr (std::is_integral_v<T>) {
            if constexpr (sizeof(T) == 1) {
                type_ = std::is_signed_v<T> ? ValueType::kSByte : ValueType::kUByte;
            } else if constexpr (sizeof(T) == 2) {
                type_ = std::is_signed_v<T> ? ValueType::kSShort : ValueType::kUShort;
            } else if constexpr (sizeof(T) == 4) {
                type_ = std::is_signed_v<T> ? ValueType::kSLong : ValueType::kULong;
            } else if constexpr (sizeof(T) == 8) {
                type_ = std::is_signed_v<T> ? ValueType::kSRational : ValueType::kURational;
            }
        }
        if constexpr (std::is_floating_point_v<T>) {
            type_ = sizeof(T) == 4 ? ValueType::kFloat : ValueType::kDouble;
        }
        std::memcpy(&storge_.bytes_, &val, sizeof(T));
    }
    Value(void* ptr, size_t size, ValueType type) {
        if (type == ValueType::kAscii) {
            storge_.string_ = new std::string((char*)ptr, size);
            type_           = type;
            return;
        }
        if (type == ValueType::kUndefined) {
            return;
        }
        std::memcpy(&storge_.bytes_, ptr, size);
        type_ = type;
    }
    Value(const Value& other) { copy(other); }
    Value(Value&& other) { move(std::move(other)); }

    Value& operator=(const Value& other) {
        if (this != &other) {
            copy(other);
        }
        return *this;
    }

    Value& operator=(Value&& other) {
        if (this != &other) {
            move(std::move(other));
        }
        return *this;
    }

    ~Value() {
        if (type_ == ValueType::kAscii) {
            delete storge_.string_;
        }
    }

    ValueType getType() const { return type_; }

    template <typename T>
    T get() {
        if (type_ != ValueType::kAscii) {
            return *reinterpret_cast<T*>(&storge_.bytes_);
        }
        throw std::runtime_error("Invalid type");
    }

    template <>
    std::string get() {
        if (type_ == ValueType::kAscii) {
            return *storge_.string_;
        }
        throw std::runtime_error("Invalid type");
    }

    template <typename T>
    T& getRef() {
        if (type_ != ValueType::kAscii) {
            return *reinterpret_cast<T*>(&storge_.bytes_);
        }
        throw std::runtime_error("Invalid type");
    }

    template <>
    std::string& getRef() {
        if (type_ == ValueType::kAscii) {
            return *storge_.string_;
        }
        throw std::runtime_error("Invalid type");
    }

    template <typename T>
    void get(T& value) {
        value = get<T>();
    }

    template <>
    void get(std::string& value) {
        value = get<std::string>();
    }
};

} // namespace exif