#include <format>
#include <thread>
#include <mutex>
#include <deque>
#include <unordered_set>
#include <random>

#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
#include "ftxui/component/component.hpp"          // for Menu
#include "ftxui/dom/elements.hpp"                 // for vbox, xflex, size
#include "ftxui/component/event.hpp"

#include "snake_game.h"

namespace TerminalMinigames
{
    Pixel::Pixel(std::tuple<float, int> center)
    {
        SetCenter(center);
    }

    Pixel::Pixel(float center_x, int center_y)
    {
        SetCenter(std::tuple<float, int> {center_x, center_y});
    }

    void Pixel::SetCenter(std::tuple<float, int> new_center)
    {
        center = new_center;

        x1_y1 = { std::floor(std::get<0>(new_center)) - 1, std::get<1>(new_center) - 1 };
        x2_y1 = { std::floor(std::get<0>(new_center)), std::get<1>(new_center) - 1 };

        x3_y1 = { std::ceil(std::get<0>(new_center)), std::get<1>(new_center) - 1 };
        x4_y1 = { std::ceil(std::get<0>(new_center)) + 1, std::get<1>(new_center) - 1 };

        x1_y2 = { std::floor(std::get<0>(new_center)) - 1, std::get<1>(new_center) + 1 };
        x2_y2 = { std::floor(std::get<0>(new_center)), std::get<1>(new_center) + 1 };

        x3_y2 = { std::ceil(std::get<0>(new_center)), std::get<1>(new_center) + 1 };
        x4_y2 = { std::ceil(std::get<0>(new_center)) + 1, std::get<1>(new_center) + 1 };
    }

    void Pixel::SetCenter(float new_center_x, int new_center_y)
    {
        center = std::tuple<int, int>{ new_center_x, new_center_y };

        x1_y1 = { std::floor(new_center_x) - 1, new_center_y - 1 };
        x2_y1 = { std::floor(new_center_x), new_center_y - 1 };

        x3_y1 = { std::ceil(new_center_x), new_center_y - 1 };
        x4_y1 = { std::ceil(new_center_x) + 1, new_center_y - 1 };

        x1_y2 = { std::floor(new_center_x) - 1, new_center_y + 1 };
        x2_y2 = { std::floor(new_center_x), new_center_y + 1 };

        x3_y2 = { std::ceil(new_center_x), new_center_y + 1 };
        x4_y2 = { std::ceil(new_center_x) + 1, new_center_y + 1 };
    }

    void Pixel::DrawPixel(ftxui::Canvas* canvas, ftxui::Color color)
    {
        (*canvas).DrawBlock(std::get<0>(x1_y1), std::get<1>(x1_y1), true, color);
        (*canvas).DrawBlock(std::get<0>(x2_y1), std::get<1>(x2_y1), true, color);

        (*canvas).DrawBlock(std::get<0>(x3_y1), std::get<1>(x3_y1), true, color);
        (*canvas).DrawBlock(std::get<0>(x4_y1), std::get<1>(x4_y1), true, color);

        (*canvas).DrawBlock(std::get<0>(x1_y2), std::get<1>(x1_y2), true, color);
        (*canvas).DrawBlock(std::get<0>(x2_y2), std::get<1>(x2_y2), true, color);

        (*canvas).DrawBlock(std::get<0>(x3_y2), std::get<1>(x3_y2), true, color);
        (*canvas).DrawBlock(std::get<0>(x4_y2), std::get<1>(x4_y2), true, color);
    }

    void SpawnFood(SnakeGameState* current_game_state)
    {
        int new_x_factor = uniform_distribution_x(generator);
        int new_y_factor = uniform_distribution_y(generator);

        Pixel food_position(3.5f + new_x_factor * movement_offset, 5 + new_y_factor * movement_offset);

        while (std::find_if(game_state.snake_position_queue.begin(), game_state.snake_position_queue.end(), Pixel(food_position)) != game_state.snake_position_queue.end())
        {
            food_position.SetCenter(3.5f + uniform_distribution_x(generator) * movement_offset, 5 + uniform_distribution_y(generator) * movement_offset);
        }

        (*current_game_state).food_positions.insert(food_position);
    }

    void HandleInput(Pixel* new_head_pos, SnakeGameState* current_game_state, MovementDirection new_direction, int x_offset, int y_offset)
    {
        (*current_game_state).current_movement_direction = new_direction;

        (*new_head_pos).SetCenter(std::tuple<float, int> {std::get<0>((*new_head_pos).center) + x_offset, std::get<1>((*new_head_pos).center) + y_offset});
    }

    void HandleLeftRightMovement(Pixel* new_head_pos, SnakeGameState* current_game_state, bool moves_left)
    {
        switch (game_state.last_input)
        {
        case InputDirection::None:
        case InputDirection::Left:
        case InputDirection::Right:
        {
            if (moves_left)
            {
                (*new_head_pos).SetCenter(std::tuple<float, int> {std::get<0>((*new_head_pos).center) - movement_offset, std::get<1>((*new_head_pos).center)});
            }
            else
            {
                (*new_head_pos).SetCenter(std::tuple<float, int> {std::get<0>((*new_head_pos).center) + movement_offset, std::get<1>((*new_head_pos).center)});
            }
            break;
        }
        case InputDirection::Up:
        {
            HandleInput(new_head_pos, current_game_state, MovementDirection::Up, 0, -movement_offset);
            break;
        }
        case InputDirection::Down:
        {
            HandleInput(new_head_pos, current_game_state, MovementDirection::Down, 0, movement_offset);
            break;
        }
        }
    }

    void HandleUpDownMovement(Pixel* new_head_pos, SnakeGameState* current_game_state, bool moves_up)
    {
        switch (game_state.last_input)
        {
        case InputDirection::None:
        case InputDirection::Down:
        case InputDirection::Up:
        {
            if (moves_up)
            {
                (*new_head_pos).SetCenter(std::tuple<float, int> {std::get<0>((*new_head_pos).center), std::get<1>((*new_head_pos).center) - movement_offset});
            }
            else
            {
                (*new_head_pos).SetCenter(std::tuple<float, int> {std::get<0>((*new_head_pos).center), std::get<1>((*new_head_pos).center) + movement_offset});
            }
            break;
        }
        case InputDirection::Left:
        {
            HandleInput(new_head_pos, current_game_state, MovementDirection::Left, -movement_offset, 0);
            break;
        }
        case InputDirection::Right:
        {
            HandleInput(new_head_pos, current_game_state, MovementDirection::Right, movement_offset, 0);
            break;
        }
        }
    }

    void PrintGameOverToCanvas(ftxui::Canvas* canvas)
    {
        int y_index = 28;
        for (const std::string text : game_over_string)
        {
            (*canvas).DrawText(36, y_index, text);
            y_index += 4;
        }
    }

    template<typename Functor>
    void ExecuteSnake(Functor quit_function, bool* back_to_menu)
    {
        game_state.Reset();

        auto screen = ftxui::ScreenInteractive::Fullscreen();
        auto container = ftxui::Container::Vertical({});

        ftxui::Component event_catcher;

        auto board_renderer = ftxui::Renderer([&]
            {
                auto canvas = ftxui::Canvas(board_dimension_x, board_dimension_y);

                // Draw custom border around canvas:
                canvas.DrawBlockLine(0, 2, canvas.width(), 2); // top border

                canvas.DrawBlockLine(0, 2, 0, canvas.height() - 3); // left border (part 1)
                canvas.DrawBlockLine(1, 2, 1, canvas.height() - 3); // left border (part 2)

                canvas.DrawBlockLine(canvas.width() - 1, 2, canvas.width() - 1, canvas.height() - 3); // right border (part 1)
                canvas.DrawBlockLine(canvas.width() - 2,  2, canvas.width() - 2, canvas.height() - 3); // right border (part 1)

                canvas.DrawBlockLine(0, canvas.height() - 3, canvas.width() - 1, canvas.height() - 3); // bottom border

                // Fetch all necessary locks
                snake_positions_mutex.lock();
                food_positions_mutex.lock();

                // Draw food on canvas:
                for (auto food : game_state.food_positions)
                {
                    food.DrawPixel(&canvas, ftxui::Color::Red);
                }

                // Draw Snake on canvas:
                for (int index = 0; index < game_state.snake_position_queue.size(); ++index)
                {
                    if (index == 0)
                    {
                        game_state.snake_position_queue[index].DrawPixel(&canvas, ftxui::Color::LightGreen);
                    }
                    else
                    {
                        game_state.snake_position_queue[index].DrawPixel(&canvas, ftxui::Color::Green);
                    }
                }

                // Unlock all mutexes
                snake_positions_mutex.unlock();
                food_positions_mutex.unlock();

                if (game_state.isDead)
                {
                    PrintGameOverToCanvas(&canvas);
                }
                    
                return ftxui::canvas(std::move(canvas));
            });

        container->Add(board_renderer);

        // Quit button
        std::string quit_button_label = "Back to Menu";
        auto quit_button = ftxui::Button(&quit_button_label, [&] { quit_function(); });
        container->Add(quit_button);

        // Restart button
        std::string restart_button_label = "Restart";
        auto restart_button = ftxui::Button(&restart_button_label, [&] { game_state.Reset(); food_positions_mutex.lock(); SpawnFood(&game_state); food_positions_mutex.unlock(); });
        container->Add(restart_button);

        auto game_view_renderer = ftxui::Renderer(container, [&]
                                        { 
                                            auto length_text = std::format("Length: {}", game_state.snake_position_queue.size());

                                            return ftxui::vbox({ 
                                                ftxui::text("Terminal Minigames") | ftxui::bold | ftxui::center, 
                                                ftxui::text(length_text), 
                                                ftxui::hbox({
                                                    board_renderer->Render(),
                                                    ftxui::vbox({
                                                        quit_button->Render(),
                                                        restart_button->Render(),
                                                        ftxui::filler()
                                                    })
                                                })
                                            }); 
                                        });
        
        auto game_view_event_catch_wrapper = ftxui::CatchEvent(game_view_renderer, [&](ftxui::Event e) {
            if (e == ftxui::Event::ArrowLeft)
            {
                game_state.last_input = InputDirection::Left;
                return true;
            }
            else if (e == ftxui::Event::ArrowRight)
            {
                game_state.last_input = InputDirection::Right;
                return true;
            }
            else if (e == ftxui::Event::ArrowDown)
            {
                game_state.last_input = InputDirection::Down;
                return true;
            }
            else if (e == ftxui::Event::ArrowUp)
            {
                game_state.last_input = InputDirection::Up;
                return true;
            }

            return false;
            });

        std::thread move_snake([&] {
            int seconds_passed_since_last_food_spawn = 0;

            food_positions_mutex.lock();
            SpawnFood(&game_state);
            food_positions_mutex.unlock();

            while (!(*back_to_menu) && !game_state.isDead)
            {
                // wait certain amount of time before updating position:
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(0.5s);

                // Fetch required locks
                snake_positions_mutex.lock();
                food_positions_mutex.lock();

                // handle input (if present) and move the snake
                auto new_head_pos = game_state.snake_position_queue.front();

                switch (game_state.current_movement_direction)
                {
                case MovementDirection::Left:
                case MovementDirection::Right:
                {
                    HandleLeftRightMovement(&new_head_pos, &game_state, game_state.current_movement_direction == MovementDirection::Left);
                    break;
                }
                case MovementDirection::Up:
                case MovementDirection::Down:
                {
                    HandleUpDownMovement(&new_head_pos, &game_state, game_state.current_movement_direction == MovementDirection::Up);
                    break;
                }
                }

                // Check if new_head_pos is already contained in snake_position_queue or is out of bounds:
                if (std::find_if(game_state.snake_position_queue.begin(), game_state.snake_position_queue.end(), Pixel(new_head_pos)) != game_state.snake_position_queue.end()
                    || std::get<0>(new_head_pos.center) > max_x_dimension
                    || std::get<0>(new_head_pos.center) < min_x_dimension
                    || std::get<1>(new_head_pos.center) > max_y_dimension
                    || std::get<1>(new_head_pos.center) < min_y_dimension)
                {
                    game_state.isDead = true;
                    screen.PostEvent(ftxui::Event::Custom);
                    snake_positions_mutex.unlock();
                    food_positions_mutex.unlock();
                    break;
                }

                game_state.snake_position_queue.push_front(new_head_pos);
                
                // Check if new_head_pos is contained in food_positions => snake is eating
                bool is_eating = game_state.food_positions.contains(new_head_pos);
                if (is_eating)
                {
                    game_state.food_positions.erase(new_head_pos);
                    SpawnFood(&game_state);
                }
                else 
                {
                    game_state.snake_position_queue.pop_back();
                }

                if (seconds_passed_since_last_food_spawn > 20)
                {
                    SpawnFood(&game_state);
                    seconds_passed_since_last_food_spawn = 0;
                }
                else
                {
                    seconds_passed_since_last_food_spawn++;
                }

                game_state.last_input = InputDirection::None;

                food_positions_mutex.unlock();
                snake_positions_mutex.unlock();

                screen.PostEvent(ftxui::Event::Custom);
            }
            });
        
        screen.Loop(game_view_event_catch_wrapper);
        move_snake.join();
    }
} // namespace TerminalMinigames
