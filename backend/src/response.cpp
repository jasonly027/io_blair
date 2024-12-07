#include "response.hpp"

#include <rfl/json.hpp>
#include <utility>

using std::string, std::string_view, std::optional;
namespace json = rfl::json;

namespace io_blair {

namespace response {

struct Join {
    bool success;
    optional<string> code;
};

string join(bool success, optional<string> code) {
    assert(success == code.has_value()
        && "Either success alongside a code or fail by itself");
    return json::write(Join{.success = success, .code = std::move(code)});
}

}  // namespace response
}  // namespace io_blair
