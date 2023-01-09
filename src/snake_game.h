#pragma once

#include <unordered_set>
#include <random>

#include "util/util.h"

namespace TerminalMinigames
{
    namespace Snake
    {
        /**
         * Enum listing all the avilable movement directions for the snake.
         */
        enum class MovementDirection
        {
            Left,
            Right,
            Up,
            Down
        };

        /**
         * Creates a bigger representation of a coordinate of the snake by wrapping it in a 2x1 container.
         * Required to prevent visualization by a block with questionmark. Looks nicer this way.
         */
        struct Pixel
        {
            std::tuple<int, int> x1_y1;
            std::tuple<int, int> x2_y1;

            std::tuple<int, int> x3_y1;
            std::tuple<int, int> x4_y1;

            std::tuple<int, int> x1_y2;
            std::tuple<int, int> x2_y2;

            std::tuple<int, int> x3_y2;
            std::tuple<int, int> x4_y2;

            std::tuple<float, int> center; // min: (2,2); max: (97,47)

            Pixel(std::tuple<float, int> center);
            Pixel(float center_x, int center_y);

            void SetCenter(std::tuple<float, int> new_center);
            void SetCenter(float new_center_x, int new_center_y);

            /**
             * Prints the pixel to the given canvas in the given color by printing the individual points as blocks.
             */
            void DrawPixel(ftxui::Canvas* canvas, ftxui::Color color);

            /**
             * Allows comparison with other pixels whether they point to the same coordinates (center).
             * @param other_pixel Pixel to compare to.
             */
            bool operator()(const Pixel& other_pixel) const
            {
                return std::floor(std::get<0>(center)) == std::floor(std::get<0>(other_pixel.center)) && std::get<1>(center) == std::get<1>(other_pixel.center);
            }

            bool operator==(const Pixel& other_pixel) const
            {
                return std::floor(std::get<0>(center)) == std::floor(std::get<0>(other_pixel.center)) && std::get<1>(center) == std::get<1>(other_pixel.center);
            }

            struct HashFunction
            {
                size_t operator()(const Pixel& pixel) const
                {
                    size_t x_hash = std::hash<int>()(std::floor(std::get<0>(pixel.center)));
                    size_t y_hash = std::hash<int>()(std::get<1>(pixel.center));
                    return x_hash ^ y_hash;
                }
            };
        };

        /**
         * Struct containing all the information required for maintaining the current state of the game.
         */
        struct SnakeGameState
        {
            /**
             * Flag whether the player died or not.
             */
            bool isDead = false;
            /**
             * Length of the snake.
             */
            int snake_length = 4;
            /**
             * Double-ended queue containing the snake's positions in form of TerminalMinigames::Pixel instances.
             */
            std::deque<Pixel> snake_position_queue = {
                    Pixel(47.5f, 25),
                    Pixel(51.5f, 25),
                    Pixel(55.5f, 25),
                    Pixel(59.5f, 25) };
            /**
             * Last input caught.
             */
            std::atomic<InputDirection> last_input = InputDirection::None ;
            /**
             * Current movement direction of the snake.
             */
            std::atomic<MovementDirection> current_movement_direction = MovementDirection::Left;
            /**
             * Set containing the positions of the food for the snake.
             */
            std::unordered_set<Pixel, Pixel::HashFunction> food_positions = {};
        
            /**
             * Resets the game state to start a fresh game.
             */
            void Reset()
            {
                isDead = false;
                snake_length = 4;
                snake_position_queue = {
                    Pixel(47.5f, 25),
                    Pixel(51.5f, 25),
                    Pixel(55.5f, 25),
                    Pixel(59.5f, 25) };
                last_input = InputDirection::None;
                current_movement_direction = MovementDirection::Left;
                food_positions.clear();
            }
        };

        struct SnakeConfig
        {
            /**
             * Size of the step to move a TerminalMinigames::Pixel's center point to move it on the canvas.
             */
            int movement_offset = 4;

            /**
             * Width of the canvas.
             */
            int board_dimension_x = 200;
            /**
             * Height of the canvas.
             */
            int board_dimension_y = 100;
            /**
             * Min position on the x-axis to put a pixel's center.
             */
            const int min_x_dimension = 3;
            /**
             * Min position on the y-axis to put a pixel's center.
             */
            const int min_y_dimension = 3;
            /**
             * Max position on the x-axis to put a pixel's center.
             */
            const int max_x_dimension = 196;
            /**
             * Max position on the y-axis to put a pixel's center.
             */
            const int max_y_dimension = 96;

            /**
             * Max factor to multiply the movement_offset by to position the food on the x-axis of the canvas.
             */
            const int food_x_offset_max_factor = 48;
            /**
             * Min factor to multiply the movement_offset by to position the food on the x-axis of the canvas.
             */
            const int food_x_offset_min_factor = 1;
            /**
             * Max factor to multiply the movement_offset by to position the food on the y-axis of the canvas.
             */
            const int food_y_offset_max_factor = 22;
            /**
             * Min factor to multiply the movement_offset by to position the food on the y-axis of the canvas.
             */
            const int food_y_offset_min_factor = 1;
        };

        /**
         * Spawns food by putting it in the passed game state's food position set.
         * The newly added food position is then drawn on the next draw call of the canvas.
         */
        void SpawnFood(SnakeGameState* current_game_state);

        /**
         * Handles movement when an input was received.
         */
        void HandleInput(Pixel* new_head_pos, SnakeGameState* current_game_state, MovementDirection new_direction, int x_offset, int y_offset);
        /**
         * Handles snake movement when the movement direction is either left or right.
         */
        void HandleLeftRightMovement(Pixel* new_head_pos, SnakeGameState* current_game_state, bool moves_left);
        /**
         * Handles snake movement when the movement direction is either up or down.
         */
        void HandleUpDownMovement(Pixel* new_head_pos, SnakeGameState* current_game_state, bool moves_up);
    
        /**
         * Main function for the snake game.
         * @param quit_function Function executed when the player presses the back to menu button.
         * @param back_to_menu Flag whether the player wants to go back to the menu. Required by the main menu.
         */        
        void ExecuteSnake(QuitFunction quit_function, bool* back_to_menu);
    } // namespace Snake
} // namespace TerminalMinigames
