/*
** EPITECH PROJECT, 2024
** my
** File description:
** my
*/

#ifndef MY_H_
    #define MY_H_
    #include <cstdio>
    #include <cstring>
    #include <csignal>
    #include <cstdlib>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <ncurses.h>
    #include <string>
    #include <iostream>
    #include <vector>
    #include <map>
    #include <stdexcept>
    // include all other headers here
    #include "core/utils/SignalHandler.hpp"
    #include "core/ecs/Registry.hpp"

    #include "ui/InputSystem.hpp"
    #include "ui/NcursesRenderer.hpp"

    #include "game/shogi/ShogiFactory.hpp"
    #include "game/systems/InputSystem.hpp"
    #include "game/systems/RenderSystem.hpp"
    #include "game/systems/MovementSystem.hpp"
    #include "game/systems/RulesSystem.hpp"
    #include "game/systems/CleanupSystem.hpp"

#endif /* !MY_H_ */
