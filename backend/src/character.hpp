/**
 * @file character.hpp
 */
#pragma once

namespace io_blair {
/**
 * @brief Possible game characters.
 */
enum class Character { // NOLINTBEGIN(readability-identifier-naming)
  unknown, /** Unspecified character */
  Io, /** Io */
  Blair, /** Blair */
}; // NOLINTEND(readability-identifier-naming)
} // namespace io_blair
