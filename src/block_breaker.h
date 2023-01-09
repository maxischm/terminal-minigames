#pragma once

#include <unordered_set>

#include "boost/container_hash/hash.hpp"

#include "util/util.h"
#include "util/vector2d.h"

namespace TerminalMinigames
{
	namespace BlockBreaker
	{
		

		struct Block
		{
			Vector2D::Vector2D end_left;
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
		};

		struct BlockBreakerGameState
		{
			Vector2D::Vector2D paddle_position;
			Vector2D::Vector2D ball_position;
			Vector2D::Vector2D ball_direction;

			int score = 0;

			std::unordered_set<Block, boost::hash<Block>> block_positions;

			InputDirection last_input = InputDirection::None;

			bool lost = false;

			void Reset(BlockBreakerGameState& game_state);
		};

		void ExecuteBlockBreaker(QuitFunction quit_function, bool* back_to_menu);

		enum class BorderCollisions
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

		inline const std::string ToString(BorderCollisions c)
		{
			switch (c)
			{
			case BorderCollisions::Left:		return "Left";
			case BorderCollisions::TopLeft:		return "Top Left";
			case BorderCollisions::TopRight:	return "Top Right";
			case BorderCollisions::Right:		return "Right";
			case BorderCollisions::BottomLeft:	return "Bottom Left";
			case BorderCollisions::BottomRight: return "Bottom Right";
			case BorderCollisions::Top:			return "Top";
			case BorderCollisions::Bottom:		return "Bottom";
			case BorderCollisions::None:		return "None";
			}
		}

		BorderCollisions IntersectsBorder(Vector2D::Vector2D& pos);
		void HandleBorderCollision(BlockBreakerGameState& game_state, BorderCollisions collision_type);

		/**
		 * Checks whether the ball collides with the paddle controlled by the player.
		 * 
		 * @param game_state Current game state describing ball & paddle positions.
		 * @returns Whether the ball intersects with the paddle.
		 */
		bool IntersectsPaddle(BlockBreakerGameState& game_state);

		void HandlePaddleCollision(BlockBreakerGameState& game_state);

		/**
		 * Rotates the given direction vector clockwise by 90 degrees as seen on the canvas.
		 * Due to the canvas's (0, 0) being in the top left, this function rotates the given
		 * direction as it is seen by the player.
		 * 
		 * @param direction The direction vector to rotate.
		 * @returns Rotated direction vector.
		 */
		Vector2D::Vector2D RotateBy90DegreesClockwise(Vector2D::Vector2D direction);

		/**
		 * Rotates the given direction vector counter-clockwise by 90 degrees as seen on the canvas.
		 * Due to the canvas's (0, 0) being in the top left, this function rotates the given
		 * direction as it is seen by the player.
		 * 
		 * @param direction The direction vector to rotate.
		 * @returns Rotated direction vector.
		 */
		Vector2D::Vector2D RotateBy90DegreesCounterClockwise(Vector2D::Vector2D direction);
	} // namespace BlockBreaker
} // namespace TerminalMinigames