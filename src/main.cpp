#include <stdlib.h> // for EXIT_SUCCESS
#include "main_menu.cpp"

int main()
{
    TerminalMinigames::StartGame();

    return EXIT_SUCCESS;
}

// #include "main.h"

// #include <thread>   // for sleep_for, thread
// #include <chrono>   // for operator""s, chrono_literals
// #include <format>

// #include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
// #include "ftxui/component/component.hpp"          // for Menu
// #include "ftxui/dom/elements.hpp"                 // for vbox, xflex, size
// #include "main.h"

// bool show_main_menu = true;
// int selected_game = 0;

// std::vector<std::string> available_games = {"Snake", "Block Breaker"};
// std::vector<std::string> game_descriptions = {"Snake is a sub-genre of action video games where the player maneuvers the end of a growing line, often themed as a snake. The player must keep the snake from colliding with both other obstacles and itself, which gets harder as the snake lengthens. - Wikipedia"};

// ftxui::Element CreateMainMenu()
// {
//     // Create dropdown
//     auto game_selection_dropdown = ftxui::Dropdown(&available_games, &selected_game);

//     // Create dummy text
//     auto game_description = ftxui::paragraphAlignJustify(game_descriptions[selected_game]);

//     // Create button
//     std::string label = "Start";
//     auto start_game_button = ftxui::Button(&label, [&]
//                                            {
//         if (selected_game == 0)
//         {
//             // StartSnake(Container);
//         } });

//     return ftxui::vbox({game_selection_dropdown->Render(),
//                         game_description,
//                         start_game_button->Render()});
// }

// // ftxui::Component Start()
// // {
// // auto main_content_container = ftxui::Container::Vertical({});

// // return ftxui::Renderer(main_content_container, [&]
// //                        {
// //                         // Create dropdown
// //                         auto game_selection_dropdown = ftxui::Dropdown(&available_games, &selected_game);
// //                         main_content_container->Add(game_selection_dropdown);

// //                         // Create dummy text
// //                         auto game_description = ftxui::paragraphAlignJustify(game_descriptions[selected_game]);

// //                         // Create button
// //                         std::string label = "Start";
// //                         auto start_game_button = ftxui::Button(&label, [&]
// //                                                             {
// //                             if (selected_game == 0)
// //                             {
// //                                 // StartSnake(Container);
// //                             } });
// //                         main_content_container->Add(start_game_button);

// //                         return ftxui::vbox({ftxui::text("Terminal Minigames") | ftxui::bold | ftxui::hcenter, game_selection_dropdown->Render(),
// //                                             game_description,
// //                                             start_game_button->Render()}); });
// // }

// int main(int argc, const char *argv[])
// {
//     auto screen = ftxui::ScreenInteractive::Fullscreen();

//     auto main_content_container = ftxui::Container::Vertical({});

//     // Create dropdown
//     auto game_selection_dropdown = ftxui::Dropdown(&available_games, &selected_game);
//     main_content_container->Add(game_selection_dropdown);

//     // Create dummy text
//     auto game_description = ftxui::paragraphAlignJustify(game_descriptions[selected_game]);

//     // Create button
//     std::string label = "Start";
//     auto start_game_button = ftxui::Button(&label, [&]
//                                            {
//                                 if (selected_game == 0)
//                                 {
//                                     // StartSnake(Container);
//                                 } });
//     main_content_container->Add(start_game_button);

//     auto renderer = ftxui::Renderer(main_content_container, [&]
//                                     { return ftxui::vbox({ftxui::text("Terminal Minigames") | ftxui::bold | ftxui::hcenter,
//                                                           game_selection_dropdown->Render(),
//                                                           game_description,
//                                                           start_game_button->Render()}); });

//     // ftxui::Component renderer = Start();

//     screen.Loop(renderer);

//     // auto screen = ftxui::ScreenInteractive::Fullscreen();

//     // // --- Game Selection Dropdown --- //

//     // std::vector<std::string> games = {"Snake", "Block Breaker"};
//     // int selected_game = 0;
//     // auto game_selection_dropdown = ftxui::Dropdown(&games, &selected_game);

//     // // --- Main UI Visualization --- //
//     // bool show_main_menu = true;
//     // auto main_container = ftxui::Container::Vertical({});

//     // int counter = 0;
//     // std::string label = "Start";
//     // ftxui::Component start_button = ftxui::Button(&label, [&]
//     //                                               { counter++; screen.PostEvent(ftxui::Event::Custom); });

//     // auto counter_text = std::format("Test: {}", counter);

//     // // --- Put it all together --- //

//     // // main_container->Add(game_selection_dropdown);
//     // // main_container->Add(start_button);

//     // auto renderer = Renderer(main_container, [&]
//     //                          {
//     //                             if (show_main_menu)
//     //                             {
//     //                                 main_container->Add(game_selection_dropdown);
//     //                                 main_container->Add(start_button);
//     //                                 return ftxui::vbox({ftxui::text("Terminal Minigames") | ftxui::bold | ftxui::hcenter,
//     //                                                main_container->Render(),
//     //                                                ftxui::text(std::format("Test: {}", counter))});
//     //                             }
//     //                             else if (selected_game == 0)
//     //                             {
//     //                                 return ftxui::vbox({ftxui::text("Terminal Minigames") | ftxui::bold | ftxui::hcenter,
//     //                                                DisplaySnake(&show_main_menu) | ftxui::center | ftxui::border,
//     //                                                ftxui::text(std::format("Test: {}", counter))});
//     //                             } });

//     // screen.Loop(renderer);

//     return EXIT_SUCCESS;
// }