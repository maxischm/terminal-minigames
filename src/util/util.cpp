#pragma once

#include <algorithm>

#include "util.h"

#include "ftxui/dom/canvas.hpp"

namespace TerminalMinigames
{
    /**
     * String vector containing the different lines to print "Game Over" to the screen.
     */
    const std::vector<std::string> game_over_string_one_line = {
        "  _____                        ____",
        " / ____|                      / __ \\",
        "| |  __  __ _ _ __ ___   ___ | |  | |_   _____ _ __",
        "| | |_ |/ _` | '_ ` _ \\ / _ \\| |  | \\ \\ / / _ \\ '__|",
        "| |__| | (_| | | | | | |  __/| |__| |\\ V /  __/ |",
        " \\_____|\\__,_|_| |_| |_|\\___| \\____/  \\_/ \\___|_|" };

    const std::vector<std::string> game_over_string_two_line = {
        "  _____                     ",
        " / ____|                    ",
        "| |  __  __ _ _ __ ___   ___",
        "| | |_ |/ _` | '_ ` _ \\ / _ \\",
        "| |__| | (_| | | | | | |  __/",
        " \\_____|\\__,_|_| |_| |_|\\___|",
        "",
        "  ____",
        " / __ \\",
        "| |  | |_   _____ _ __",
        "| |  | \\ \\ / / _ \\ '__|",
        "| |__| |\\ V /  __/ |",
        " \\____/  \\_/ \\___|_|"};

    void PrintGameOverToCanvas(ftxui::Canvas& canvas, Vector2D::Vector2D top_left_pos, bool two_line)
    {
        PrintTextToCanvas(canvas, top_left_pos, two_line ? game_over_string_two_line : game_over_string_one_line);
    }

    const std::vector<std::string> win_string_one_line = {
        "__     __          __          __         _ ",
        "\\ \\   / /          \\ \\        / /        | |",
         " \\ \\_/ /__  _   _   \\ \\  /\\  / /__  _ __ | |",
         "  \\   / _ \\| | | |   \\ \\/  \\/ / _ \\| '_ \\| |",
         "   | | (_) | |_| |    \\  /\\  / (_) | | | |_|",
         "   |_|\\___/ \\__,_|     \\/  \\/ \\___/|_| |_(_)"
    };

    void PrintWonMessageToCanvas(ftxui::Canvas& canvas, Vector2D::Vector2D top_left_pos)
    {
        PrintTextToCanvas(canvas, top_left_pos, win_string_one_line);
    }

    void PrintTextToCanvas(ftxui::Canvas& canvas, Vector2D::Vector2D top_left_pos, std::vector<std::string> message)
    {
        int y_index = top_left_pos.y;
        for (const std::string line : message)
        {
            canvas.DrawText(top_left_pos.x, y_index, line);
            y_index += 4;
        }
    }

    bool IsPointOnLineSegment(Vector2D::Vector2D p, Vector2D::Vector2D q, Vector2D::Vector2D r)
    {
        return q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
            q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y);
    }

    PointOrientation ThreePointOrientation(Vector2D::Vector2D p, Vector2D::Vector2D q, Vector2D::Vector2D r)
    {
        // See https://www.geeksforgeeks.org/orientation-3-ordered-points/
        // for details of below formula.
        double val = (q.y - p.y) * (r.x - q.x) -
            (q.x - p.x) * (r.y - q.y);

        if (val == 0) return PointOrientation::Collinear;

        return (val > 0) ? PointOrientation::Clockwise : PointOrientation::CounterClockwise;
    }

    bool LineSegmentsIntersect(Vector2D::Vector2D p1, Vector2D::Vector2D q1, Vector2D::Vector2D p2, Vector2D::Vector2D q2)
    {
        // Find the four orientations needed for general and
        // special cases
        PointOrientation o1 = ThreePointOrientation(p1, q1, p2);
        PointOrientation o2 = ThreePointOrientation(p1, q1, q2);
        PointOrientation o3 = ThreePointOrientation(p2, q2, p1);
        PointOrientation o4 = ThreePointOrientation(p2, q2, q1);

        // General case
        if (o1 != o2 && o3 != o4)
        {
            return true;
        }

        // Special Cases
        // p1, q1 and p2 are collinear and p2 lies on segment p1q1
        if (o1 == PointOrientation::Collinear && IsPointOnLineSegment(p1, p2, q1)) 
        {
            return true;
        }

        // p1, q1 and q2 are collinear and q2 lies on segment p1q1
        if (o2 == PointOrientation::Collinear && IsPointOnLineSegment(p1, q2, q1)) 
        {
            return true;
        }

        // p2, q2 and p1 are collinear and p1 lies on segment p2q2
        if (o3 == PointOrientation::Collinear && IsPointOnLineSegment(p2, p1, q2)) 
        {
            return true;
        }

        // p2, q2 and q1 are collinear and q1 lies on segment p2q2
        if (o4 == PointOrientation::Collinear && IsPointOnLineSegment(p2, q1, q2)) 
        {
            return true;
        }

        return false; // Doesn't fall in any of the above cases
    }
}
