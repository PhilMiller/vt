
#if ! defined __RUNTIME_TRANSPORT_REGISTRY__
#define __RUNTIME_TRANSPORT_REGISTRY__

#include <vector>
#include <unordered_map>
#include <cassert>

#include "common.h"
#include "function.h"
#include "handler.h"

namespace runtime {

struct Registry {
  using handler_manager_t = HandlerManager;
  using handler_bits_t = HandlerBits;
  using tagged_handler_t = std::tuple<tag_t, handler_t>;
  using container_t = std::unordered_map<handler_t, active_function_t>;
  using tag_container_t = std::unordered_map<tag_t, active_function_t>;
  using han_tag_container_t = std::unordered_map<handler_t, tag_container_t>;

  Registry() = default;

  handler_t
  register_new_handler(
    active_function_t fn, tag_t const& tag = no_tag,
    bool const& is_collective = false
  );

  void
  unregister_handler_fn(handler_t const& han, tag_t const& tag = no_tag);

  void
  swap_handler(
    handler_t const& han, active_function_t fn, tag_t const& tag = no_tag
  );

  handler_t
  register_active_handler(active_function_t fn, tag_t const& tag = no_tag);

  active_function_t
  get_handler(handler_t const& han, tag_t const& tag = no_tag);

  active_function_t
  get_handler_no_tag(handler_t const& han);

  active_function_t
  get_trigger(handler_t const& han);

  void
  save_trigger(handler_t const& han, active_function_t fn);

private:
  container_t triggers;

  container_t registered;

  han_tag_container_t tagged_registered;

  handler_identifier_t cur_ident_collective = first_handle_identifier;

  handler_identifier_t cur_ident = first_handle_identifier;
};

extern std::unique_ptr<Registry> the_registry;

} //end namespace runtime

#endif /*__RUNTIME_TRANSPORT_REGISTRY__*/
