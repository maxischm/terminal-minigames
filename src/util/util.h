#pragma once

#include <functional>

#include "ftxui/dom/canvas.hpp"

#include "vector2d.h"

namespace TerminalMinigames
{
	using QuitFunction = std::function<void()>;

    /**
     * Enum listing all available input directions.
     */
    enum class InputDirection
    {
        Left,
        Right,
        Up,
        Down,
        None
    };

    void PrintGameOverToCanvas(ftxui::Canvas& canvas, Vector2D::Vector2D top_left_pos, bool two_line = false);
}
