#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
#include "ftxui/component/component.hpp"          // for Menu
#include "ftxui/dom/elements.hpp"                 // for vbox, xflex, size

#include "main_menu.h"
#include "snake_game.h"
#include "block_breaker.h"
#include "util/util.h"

namespace TerminalMinigames
{
    /**
     * Flag whether the player quit the application.
     */
    bool quit = false;

    /**
     * Flag whether the player wants to return to the main menu.
     */
    bool back_to_menu = false;

    /**
     * Flag whether the player has started a game from the main menu.
     */
    bool game_started = false;

    /**
     * Index of the game selected from the list of available games.
     */
    int selected_game = 1;

    /**
     * List of available games.
     */
    std::vector<std::string> available_games = { "Snake", "Block Breaker" };

    /**
     * List of descriptions for the list of available games.
     */
    std::vector<std::string> game_descriptions = { "Snake is a sub-genre of action video games where the player maneuvers the end of a growing line, often themed as a snake. The player must keep the snake from colliding with both other obstacles and itself, which gets harder as the snake lengthens. - Wikipedia", "DEMO TEXT FOR BLOCK BREAKER" };

    /**
     * Quit function to use to return to the main menu.
     */
    auto quit_game = [&] { back_to_menu = true; game_started = false; TerminalMinigames::StartGame(); };


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
                    Snake::ExecuteSnake(quit_game, &back_to_menu);
                    break;
                case 1:
                    BlockBreaker::ExecuteBlockBreaker(quit_game, &back_to_menu);
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