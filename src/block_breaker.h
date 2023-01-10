#pragma once

#include <unordered_set>

#include "boost/container_hash/hash.hpp"
#include "ftxui/component/screen_interactive.hpp"

#include "util/util.h"
#include "util/vector2d.h"

namespace TerminalMinigames
{
	namespace BlockBreaker
	{
		/**
		 * Block structure representing the blocks to destroy during the game.
		 */
		struct Block
		{
			/**
			 * Vector pointing to the top left corner of the block.
			 */
			Vector2D::Vector2D end_left;

			/**
			 * Vector pointing to the bottom right corner of the block.
			 */
			Vector2D::Vector2D end_right;

			Block() = default;
			Block(Vector2D::Vector2D left_endpoint, Vector2D::Vector2D right_endpoint) : end_left(left_endpoint), end_right(right_endpoint) {};

			bool operator ==(const Block& other) const
			{
				return end_left == other.end_left && end_right == other.end_right;
			}

			friend size_t hash_value(const Block& b)
			{
				size_t seed = 0;

				boost::hash<Vector2D::Vector2D> vector_hasher;

				boost::hash_combine(seed, vector_hasher(b.end_left));
				boost::hash_combine(seed, vector_hasher(b.end_right));

				return seed;
			}

			/**
			 * Draw function to draw the block on the given canvas.
			 */
			void Draw(ftxui::Canvas& canvas);
		};

		/**
		 * Game state describing structure.
		 */
		struct BlockBreakerGameState
		{
			/**
			 * Position of the paddle.
			 */
			Vector2D::Vector2D paddle_position;
			/**
			 * Position of the ball.
			 */
			Vector2D::Vector2D ball_position;
			/**
			 * Previous position of the ball.
			 * Required for the collision detection.
			 */
			Vector2D::Vector2D ball_position_prev;
			/**
			 * Direction vector for the ball trajectory.
			 */
			Vector2D::Vector2D ball_direction;
			/**
			 * Speed value equaling the magnitude of the ball_direction vector.
			 */
			float ball_speed = 10.f;

			/**
			 * Set of blocks to destroy.
			 */
			std::unordered_set<Block, boost::hash<Block>> block_positions;

			InputDirection last_input = InputDirection::None;

			bool lost = false;
			bool won = false;

			/**
			 * Resets the game state for a new start of the game.
			 */
			void Reset();
		};

		/**
		 * Main function for the Block Breaker game.
		 * 
		 * @param quit_function Function to execute on the Quit/Back to menu button.
		 * @param back_to_menu Pointer to back flag in the main menu to display main menu again.
		 */
		void ExecuteBlockBreaker(QuitFunction quit_function, bool* back_to_menu);

		/**
		 * Update function to update the ball in an individual thread.
		 * 
		 * @param screen Screen reference to post events to to signal ball position's updates.
		 * @param state Game state reference.
		 * @param back_flag Boolean flag to signal returning back to the menu.
		 */
		void UpdateBall(ftxui::ScreenInteractive& screen, BlockBreakerGameState& state, bool* back_flag);

		/**
		 * Update function to refresh the screen.
		 * 
		 * @param screen Screen reference to trigger looping.
		 * @param comp UI component to display on the screen.
		 */
		void UpdateScreen(ftxui::ScreenInteractive& screen, ftxui::Component& comp);

		/**
		 * Enum listing the collision types.
		 */
		enum class CollisionTypes
		{
			Left,
			TopLeft,
			Top,
			TopRight,
			Right,
			BottomRight,
			Bottom,
			BottomLeft,
			None
		};

		/**
		 * Transforms the given collision type into a printable string.
		 * 
		 * @param c Collision type to transform into string.
		 * @returns String for the collision type.
		 */
		inline const std::string ToString(CollisionTypes c)
		{
			switch (c)
			{
			case CollisionTypes::Left:			return "Left";
			case CollisionTypes::TopLeft:		return "Top Left";
			case CollisionTypes::TopRight:		return "Top Right";
			case CollisionTypes::Right:			return "Right";
			case CollisionTypes::BottomLeft:	return "Bottom Left";
			case CollisionTypes::BottomRight:	return "Bottom Right";
			case CollisionTypes::Top:			return "Top";
			case CollisionTypes::Bottom:		return "Bottom";
			case CollisionTypes::None:			return "None";
			}
		}

		/**
		 * Checks whether the ball at the given position with the given radius intersects with the game canvas's borders.
		 * 
		 * @param pos Position vector of the ball.
		 * @param ball_radius Radius of the ball.
		 * @returns The type of collision that has occurred.
		 */
		CollisionTypes IntersectsBorder(Vector2D::Vector2D& pos, float ball_radius);

		/**
		 * Handles the ball's collision with an object, i.e. applies direction changes.
		 * 
		 * @param game_state Current state of the game.
		 * @param collision_type Type of collision that has occurred.
		 * @param collided_border Whether the ball collided with a border of the canvas.
		 */
		void HandleCollision(BlockBreakerGameState& game_state, CollisionTypes collision_type, bool collided_border);

		/**
		 * Checks whether the ball collides with the paddle controlled by the player.
		 * 
		 * @param game_state Current game state describing ball & paddle positions.
		 * @param ball_radius Ball's radius.
		 * @returns Whether the ball intersects with the paddle.
		 */
		bool IntersectsPaddle(BlockBreakerGameState& game_state, float ball_radius);

		/**
		 * Handles the collision of the ball with the paddle.
		 * 
		 * @param game_state Current game state.
		 */
		void HandlePaddleCollision(BlockBreakerGameState& game_state);

		/**
		 * Checks and handles collisions between ball and block.
		 * 
		 * @param game_state Current game state.
		 * @param ball_radius Radius of the ball.
		 * @returns Type of collision that occurred with the hit block.
		 */
		CollisionTypes CheckAndHandleBlockCollision(BlockBreakerGameState& game_state, float ball_radius);

		/**
		 * Checks whether the ball at the given position with the given radius overlaps with the given block.
		 * 
		 * @param b Block object to check overlap for.
		 * @param v Ball position.
		 * @param ball_radius Radius of the ball.
		 * @returns Type of collision between ball and block.
		 */
		CollisionTypes TestBlockOverlap(Block b, Vector2D::Vector2D v, float ball_radius);
	} // namespace BlockBreaker
} // namespace TerminalMinigames