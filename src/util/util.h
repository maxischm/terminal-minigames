#pragma once

#define _USE_MATH_DEFINES

#include <functional>
#include <numbers>

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

    enum class PointOrientation
    {
        Collinear,
        Clockwise,
        CounterClockwise
    };

    void PrintGameOverToCanvas(ftxui::Canvas& canvas, Vector2D::Vector2D top_left_pos, bool two_line = false);
    void PrintWonMessageToCanvas(ftxui::Canvas& canvas, Vector2D::Vector2D top_left_pos);
    void PrintTextToCanvas(ftxui::Canvas& canvas, Vector2D::Vector2D top_left_pos, std::vector<std::string> message);

    /**
     * Checks whether the vector given by p lies on the line segment from v1 to v2.
     * From https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
     */
    bool IsPointOnLineSegment(Vector2D::Vector2D p, Vector2D::Vector2D q, Vector2D::Vector2D r);

    /**
     * Returns the orientation of the three points p, q, and r.
     * From https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
     */
    PointOrientation ThreePointOrientation(Vector2D::Vector2D p, Vector2D::Vector2D q, Vector2D::Vector2D r);

    /**
     * Checks whether the two line segments intersect.
     * From https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
     */
    bool LineSegmentsIntersect(Vector2D::Vector2D p1, Vector2D::Vector2D q1, Vector2D::Vector2D p2, Vector2D::Vector2D q2);

    /**
     * Convert degrees to radians.
     * From https://stackoverflow.com/a/31525208
     */
    static double DegreesToRadians(double d) {
        return (d / 180.0) * (std::numbers::pi);
    }
}
