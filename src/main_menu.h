#pragma once

namespace TerminalMinigames
{
    bool quit = false;
    bool back_to_menu = false;
    bool game_started = false;

    int selected_game = 0;

    std::vector<std::string> available_games = { "Snake", "Block Breaker" };
    std::vector<std::string> game_descriptions = { "Snake is a sub-genre of action video games where the player maneuvers the end of a growing line, often themed as a snake. The player must keep the snake from colliding with both other obstacles and itself, which gets harder as the snake lengthens. - Wikipedia", "DEMO TEXT FOR BLOCK BREAKER" };

	void StartGame();
    void ExecuteMainMenu();
    auto quit_game = [&] { back_to_menu = true; game_started = false; TerminalMinigames::StartGame(); };
}
