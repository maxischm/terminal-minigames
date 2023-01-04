#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
#include "ftxui/component/component.hpp"          // for Menu
#include "ftxui/dom/elements.hpp"                 // for vbox, xflex, size

#include "main_menu.h"
#include "snake_game.cpp"

namespace TerminalMinigames
{
    void ExecuteMainMenu()
    {
        auto screen = ftxui::ScreenInteractive::Fullscreen();
        auto component = ftxui::Container::Vertical({});

        // Create dropdown
        auto game_selection_dropdown = ftxui::Dropdown(&available_games, &selected_game);
        component->Add(game_selection_dropdown);

        std::string label = "Start";
        auto start_game_button = ftxui::Button(&label, [&]
                                               {
                                                   game_started = true;
                                                   back_to_menu = false;
                                                   quit = false;
                                                   screen.ExitLoopClosure()(); });
        component->Add(start_game_button);

        auto renderer = ftxui::Renderer(component, [&]
            { return ftxui::vbox({ ftxui::text("Terminal Minigames") | ftxui::bold | ftxui::center,
                                    game_selection_dropdown->Render(),
                                    ftxui::vbox({
                                        ftxui::paragraph(game_descriptions[selected_game]),
                                        ftxui::filler(),
                                        start_game_button->Render()
                                    }) | ftxui::center}); });
        screen.Loop(renderer);
    }

    void StartGame()
    {
        while (!quit)
        {
            if (!game_started)
            {
                ExecuteMainMenu();
            }
            else
            {
                switch (selected_game)
                {
                case 0:
                    ExecuteSnake(quit_game, &back_to_menu);
                    break;

                default:
                {
                    game_started = false;
                    ExecuteMainMenu();
                    break;
                }
                }
            }
        }
    }

} // namespace TerminalMinigames