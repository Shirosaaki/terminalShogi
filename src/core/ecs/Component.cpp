/**==============================================
 *                 Component.cpp
 *  Author: Shirosaaki
 *  Date: 2026-01-23
 *=============================================**/

#include "../../../includes/core/ecs/Component.hpp"

namespace ecs {

ComponentTypeId nextComponentTypeId() {
    static ComponentTypeId last = 0;
    return last++;
}

} // namespace ecs
