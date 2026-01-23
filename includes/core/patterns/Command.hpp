/**==============================================
 *                 Command.hpp
 *  command
 *  Author: shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#pragma once
#include "../ecs/Registry.hpp"

namespace core {

class Command {
public:
    virtual ~Command() = default;
    virtual void execute(ecs::Registry& registry) = 0;
};

} // namespace core
