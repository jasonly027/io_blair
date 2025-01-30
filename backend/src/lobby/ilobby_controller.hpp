#pragma once

#include "character.hpp"
#include "isession.hpp"


namespace io_blair {
class ILobbyController {
 public:
  virtual void set_character(ISession& self, ISession& other, Character character) = 0;

 private:
};

}  // namespace io_blair
