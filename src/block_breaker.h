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

			void Draw(ftxui::Canvas& canvas);
		};

		struct BlockBreakerGameState
		{
			Vector2D::Vector2D paddle_position;
			Vector2D::Vector2D ball_position;
			Vector2D::Vector2D ball_position_prev;
			Vector2D::Vector2D ball_direction;
			float ball_speed = 10.f;

			std::unordered_set<Block, boost::hash<Block>> block_positions;

			InputDirection last_input = InputDirection::None;

			bool lost = false;
			bool won = false;

			void Reset();
		};

		void ExecuteBlockBreaker(QuitFunction quit_function, bool* back_to_menu);

		void UpdateBall(ftxui::ScreenInteractive& screen, BlockBreakerGameState& state, bool* back_flag);
		void UpdateScreen(ftxui::ScreenInteractive& screen, ftxui::Component& comp);

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

		CollisionTypes IntersectsBorder(Vector2D::Vector2D& pos, float ball_radius);
		void HandleCollision(BlockBreakerGameState& game_state, CollisionTypes collision_type, bool collided_border);

		/**
		 * Checks whether the ball collides with the paddle controlled by the player.
		 * 
		 * @param game_state Current game state describing ball & paddle positions.
		 * @returns Whether the ball intersects with the paddle.
		 */
		bool IntersectsPaddle(BlockBreakerGameState& game_state, float ball_radius);

		void HandlePaddleCollision(BlockBreakerGameState& game_state);

		CollisionTypes CheckAndHandleBlockCollision(BlockBreakerGameState& game_state, float ball_radius);

		CollisionTypes TestBlockOverlap(Block b, Vector2D::Vector2D v, float ball_radius);
	} // namespace BlockBreaker
} // namespace TerminalMinigames