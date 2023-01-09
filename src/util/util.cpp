#pragma once

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
        int y_index = top_left_pos.y;
        for (const std::string text : two_line ? game_over_string_two_line : game_over_string_one_line)
        {
            canvas.DrawText(top_left_pos.x, y_index, text);
            y_index += 4;
        }
    }
}
