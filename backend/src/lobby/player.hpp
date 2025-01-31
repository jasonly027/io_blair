/**
 * @file player.hpp
 */
#pragma once

#include "character.hpp"
#include "session_view.hpp"


namespace io_blair {
struct Player {
  SessionView session;
  Character character;
};

}  // namespace io_blair
