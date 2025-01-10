#pragma once

#include <cstdint>
#include <optional>
#include <type_traits>

namespace io_blair {
enum class Character : int8_t { kUnset, kIO, kBlair };

using CharacterImpl = std::underlying_type_t<Character>;

constexpr std::optional<Character> to_character(int64_t num) {
  using Character::kUnset, Character::kIO, Character::kBlair;

  switch (num) {
    case static_cast<CharacterImpl>(kUnset): return kUnset;
    case static_cast<CharacterImpl>(kIO):    return kIO;
    case static_cast<CharacterImpl>(kBlair): return kBlair;
    default:                                 return std::nullopt;
  }
}

}  // namespace io_blair
