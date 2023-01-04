#pragma once

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
    int selected_game = 0;

    /**
     * List of available games.
     */
    std::vector<std::string> available_games = { "Snake", "Block Breaker" };

    /**
     * List of descriptions for the list of available games.
     */
    std::vector<std::string> game_descriptions = { "Snake is a sub-genre of action video games where the player maneuvers the end of a growing line, often themed as a snake. The player must keep the snake from colliding with both other obstacles and itself, which gets harder as the snake lengthens. - Wikipedia", "DEMO TEXT FOR BLOCK BREAKER" };

    /**
     * Starts the overall game by starting the main menu.
     */
	void StartGame();

    /**
     * Displays the main menu with the game selection.
     */
    void ExecuteMainMenu();

    /**
     * Quit function to use to return to the main menu.
     */
    auto quit_game = [&] { back_to_menu = true; game_started = false; TerminalMinigames::StartGame(); };
}
