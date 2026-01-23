/**==============================================
 *                 Entity.hpp
 *  entity ecs component
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once
#include <cstdint>

namespace ecs {

using Entity = std::uint32_t;
constexpr Entity INVALID_ENTITY = 0;

} // namespace ecs

