#pragma once

namespace TerminalMinigames
{
    enum class InputDirection
    {
        Left,
        Right,
        Up,
        Down,
        None
    };

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

    struct SnakeGameState
    {
        bool isDead = false;
        int snake_length = 4;
        std::deque<Pixel> snake_position_queue = {
                Pixel(47.5f, 25),
                Pixel(51.5f, 25),
                Pixel(55.5f, 25),
                Pixel(59.5f, 25) };

        std::atomic<InputDirection> last_input = InputDirection::None ;
        std::atomic<MovementDirection> current_movement_direction = MovementDirection::Left;

        std::unordered_set<Pixel, Pixel::HashFunction> food_positions = {};
        
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

    SnakeGameState game_state;
    std::mutex snake_positions_mutex;
    std::mutex food_positions_mutex;

    int movement_offset = 4;

    int board_dimension_x = 200;
    int board_dimension_y = 100;
    const int min_x_dimension = 3;
    const int min_y_dimension = 3;
    const int max_x_dimension = 196;
    const int max_y_dimension = 96;

    const int food_x_offset_max_factor = 48;
    const int food_x_offset_min_factor = 1;
    const int food_y_offset_max_factor = 22;
    const int food_y_offset_min_factor = 1;

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> uniform_distribution_x(food_x_offset_min_factor, food_x_offset_max_factor);
    std::uniform_int_distribution<> uniform_distribution_y(food_y_offset_min_factor, food_y_offset_max_factor);

    void SpawnFood(SnakeGameState* current_game_state);
    void HandleRightInput(Pixel* new_head_pos, SnakeGameState* current_game_state);
    void HandleLeftInput(Pixel* new_head_pos, SnakeGameState* current_game_state);
    void HandleUpInput(Pixel* new_head_pos, SnakeGameState* current_game_state);
    void HandleDownInput(Pixel* new_head_pos, SnakeGameState* current_game_state);

    void HandleLeftMovement(Pixel* new_head_pos, SnakeGameState* current_game_state);
    void HandleRightMovement(Pixel* new_head_pos, SnakeGameState* current_game_state);
    void HandleDownMovement(Pixel* new_head_pos, SnakeGameState* current_game_state);
    void HandleUpMovement(Pixel* new_head_pos, SnakeGameState* current_game_state);

    template<typename Functor>
    void ExecuteSnake(Functor quit_function, bool* back_to_menu);
}
