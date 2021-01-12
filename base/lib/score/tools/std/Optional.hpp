#pragma once
#include <ossia/detail/optional.hpp>

template <typename... Args>
using optional = std::optional<Args...>;
using none_t = decltype(std::nullopt);
