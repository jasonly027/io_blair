#pragma once

#include <cstdint>
#include <optional>
#include <type_traits>

namespace io_blair {
enum class Character : uint8_t { kUnset, kIO, kBlair };

using CharacterImpl = std::underlying_type_t<Character>;

constexpr std::optional<Character> to_character(int64_t num) {
    using Character::kUnset, Character::kIO, Character::kBlair;

    constexpr auto kF = [](Character c) {
        return static_cast<CharacterImpl>(c);
    };

    switch (num) {
        case kF(kUnset):
            return kUnset;
        case kF(kIO):
            return kIO;
        case kF(kBlair):
            return kBlair;
        default:
            return std::nullopt;
    }
}
}  // namespace io_blair
