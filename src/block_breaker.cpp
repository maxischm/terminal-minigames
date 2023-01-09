#include "block_breaker.h"
#include "block_breaker.h"
#include <thread>
#include <format>
#include <mutex>
#include <unordered_set>

#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
#include "ftxui/component/component.hpp"          // for Menu
#include "ftxui/dom/elements.hpp"                 // for vbox, xflex, size
#include "ftxui/component/event.hpp"

#include "boost/chrono/chrono.hpp"
#include "boost/chrono/time_point.hpp"

#include "block_breaker.h"
#include "util/util.h"

namespace TerminalMinigames
{
	namespace BlockBreaker
	{
		/** Definitions of member functions. **/
		
		void Block::Draw(ftxui::Canvas& canvas)
		{
			canvas.DrawBlockLine(end_left.x, end_left.y, end_right.x, end_left.y);
			canvas.DrawBlockLine(end_left.x, end_right.y, end_right.x, end_right.y);
		}

		struct BlockBreakerConfig
		{
			int board_dimension_x = 102;
			int board_dimension_y = 100;

			int paddle_width = 14;
			int paddle_height = 1;
			int paddle_step_size = 2;
			Vector2D::Vector2D paddle_start_position = { 47, 88 };
			float ball_speed = 10.f;

			/**
			 * Minimum angle theta that the ball will be returned at from the paddle.
			 */
			float min_theta = 20.f;
		} block_breaker_config;

		void BlockBreakerGameState::Reset()
		{
			// Init rows of blocks to destroy:
			for (double y = 6.f; y < 13.f; y += 6.f)
			{
				for (double x = 4.f; x < 97.f; x += 8.f)
				{
					Block b = { { x, y }, { x + 5.f, y + 2.f } };
					block_positions.insert(b);
				}
			}

			score = 0;
			paddle_position = block_breaker_config.paddle_start_position;
			ball_position = { paddle_position.x, paddle_position.y - block_breaker_config.paddle_height - 2 };
			//ball_position = { 5.f, 7.8f };
			//ball_position_prev = { 5.f, 8.1f };
			//ball_direction = { -block_breaker_config.ball_speed, -block_breaker_config.ball_speed };
			ball_direction = { 0, -block_breaker_config.ball_speed };

			lost = false;
		}

		/** **/

		/** Variables needed for execution. **/
		BlockBreakerGameState game_state;
		std::mutex paddle_mutex;
		std::mutex ball_mutex;
		std::mutex block_positions_mutex;

		CollisionTypes last_collision = CollisionTypes::None;

		/** **/

		void ExecuteBlockBreaker(QuitFunction quit_function, bool* back_to_menu)
		{
			game_state.Reset();

			ftxui::Component event_catcher;

			auto screen = ftxui::ScreenInteractive::Fullscreen();
			auto container = ftxui::Container::Vertical({});

			auto game_view_renderer = ftxui::Renderer([&]
				{
					auto canvas = ftxui::Canvas(block_breaker_config.board_dimension_x, block_breaker_config.board_dimension_y);

					// Draw custom border around canvas:
					canvas.DrawBlockLine(0, 2, canvas.width(), 2); // top border
					canvas.DrawBlockLine(0, 2, 0, canvas.height() - 3); // left border (part 1)
					canvas.DrawBlockLine(1, 2, 1, canvas.height() - 3); // left border (part 2)
					canvas.DrawBlockLine(canvas.width() - 1, 2, canvas.width() - 1, canvas.height() - 3); // right border (part 1)
					canvas.DrawBlockLine(canvas.width() - 2, 2, canvas.width() - 2, canvas.height() - 3); // right border (part 1)
					canvas.DrawBlockLine(0, canvas.height() - 3, canvas.width() - 1, canvas.height() - 3); // bottom border

					paddle_mutex.lock();
					// Draw paddle:
					canvas.DrawBlockLine(
						game_state.paddle_position.x - block_breaker_config.paddle_width / 2,
						game_state.paddle_position.y,
						game_state.paddle_position.x + block_breaker_config.paddle_width / 2 - 1,
						game_state.paddle_position.y);
					paddle_mutex.unlock();

					if (game_state.lost)
					{
						PrintGameOverToCanvas(canvas, Vector2D::Vector2D(12, 20), true);
					}
					else if (game_state.won)
					{
						PrintWonMessageToCanvas(canvas, Vector2D::Vector2D(6, 20));
					}
					else
					{
						// Draw ball:
						ball_mutex.lock();
						canvas.DrawPoint(game_state.ball_position.x, game_state.ball_position.y, true);
						ball_mutex.unlock();

						// Draw blocks:
						block_positions_mutex.lock();
						for (auto b : game_state.block_positions)
						{
							b.Draw(canvas);
						}
						block_positions_mutex.unlock();
					}

					return ftxui::canvas(std::move(canvas));
				});

			container->Add(game_view_renderer);

			// Quit button
			std::string quit_button_label = "Back to Menu";
			auto quit_button = ftxui::Button(&quit_button_label, [&] { quit_function(); });
			container->Add(quit_button);

			// Restart button
			std::string restart_button_label = "Restart";
			auto restart_button = ftxui::Button(&restart_button_label, [&] { game_state.Reset(); });
			container->Add(restart_button);

			auto screen_view_renderer = ftxui::Renderer(container, [&] {
				auto score_text = std::format("Score: {}", game_state.score);
				auto ball_position_text = std::format("Ball Position: {}", game_state.ball_position.ToString());
				auto collision_text = std::format("Collision: {}", ToString(last_collision));
				auto speed_text = std::format("Speed: {}", Vector2D::Magnitude(game_state.ball_direction));

				return ftxui::vbox({
					ftxui::text("Terminal Minigames") | ftxui::bold | ftxui::center,
					ftxui::text(score_text),
					ftxui::hbox({
						game_view_renderer->Render(),
						ftxui::vbox({
							quit_button->Render(),
							restart_button->Render(),
							ftxui::filler()
						})
					}),
					ftxui::text(ball_position_text),
					ftxui::text(collision_text),
					ftxui::text(speed_text)
					});
			});

			auto screen_view_event_catch_wrapper = ftxui::CatchEvent(screen_view_renderer, [&](ftxui::Event e) 
				{ 
					if (e == ftxui::Event::ArrowLeft)
					{

						game_state.last_input = InputDirection::Left;
						if (game_state.paddle_position.x - block_breaker_config.paddle_step_size >= 1 + block_breaker_config.paddle_width / 2)
						{
							game_state.paddle_position.x -= block_breaker_config.paddle_step_size;
						}

						return true;
					}
					else if (e == ftxui::Event::ArrowRight)
					{
						game_state.last_input = InputDirection::Right;
						if (game_state.paddle_position.x + block_breaker_config.paddle_step_size <= block_breaker_config.board_dimension_x - 2 - block_breaker_config.paddle_width / 2)
						{
							game_state.paddle_position.x += block_breaker_config.paddle_step_size;
						}

						return true;
					}
					else if (e == ftxui::Event::ArrowDown || e == ftxui::Event::ArrowUp)
					{
						// Catch input but no actions to be done
						return true;
					}
					return false;  
				});

			// Create update thread
			std::thread update_ball([&] {
				auto start = boost::chrono::high_resolution_clock::now();
				double delta_time = 0;

				while (!(*back_to_menu) && !game_state.lost && !game_state.won)
				{
					using namespace std::chrono_literals;
					std::this_thread::sleep_for(1.0s / 30.f);

					// Compute delta time
					delta_time = boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - start).count() / 1000.f;
					start = boost::chrono::high_resolution_clock::now();

					ball_mutex.lock();

					// Move ball
					game_state.ball_position_prev = game_state.ball_position;
					auto game_state_cache = game_state;
					auto position_change = game_state.ball_direction * delta_time;
					game_state.ball_position += game_state.ball_direction * delta_time;
					game_state_cache = game_state;

					// Check & handle border collision
					auto collision_type = IntersectsBorder(game_state.ball_position);
					HandleCollision(game_state, collision_type, true);

					// Check & handle paddle collision
					if (IntersectsPaddle(game_state))
					{
						HandlePaddleCollision(game_state);
					}

					// Check & handle block collision
					CheckAndHandleBlockCollision(game_state);

					ball_mutex.unlock();

					screen.PostEvent(ftxui::Event::Custom);
				}
			});


			screen.Loop(screen_view_event_catch_wrapper);
			update_ball.join();
		}

		CollisionTypes BlockBreaker::IntersectsBorder(Vector2D::Vector2D& pos)
		{
			if (pos.x < 2 && pos.y < block_breaker_config.board_dimension_y - 3 && pos.y >= 1)
			{
				return CollisionTypes::Left;
			}
			else if (pos.x < 2 && pos.y < 1)
			{
				return CollisionTypes::TopLeft;
			}
			else if (pos.x < 2 && pos.y > block_breaker_config.board_dimension_y - 3)
			{
				return CollisionTypes::BottomLeft;
			}
			else if (pos.x > block_breaker_config.board_dimension_x - 3 && pos.y < block_breaker_config.board_dimension_y - 3 && pos.y >= 1)
			{
				return CollisionTypes::Right;
			}
			else if (pos.x > block_breaker_config.board_dimension_x - 3 && pos.y < 1)
			{
				return CollisionTypes::TopRight;
			}
			else if (pos.x > block_breaker_config.board_dimension_x - 3 && pos.y > block_breaker_config.board_dimension_y - 3)
			{
				return CollisionTypes::BottomRight;
			}
			else if (pos.y > block_breaker_config.board_dimension_y - 3)
			{
				return CollisionTypes::Bottom;
			}
			else if (pos.y < 3)
			{
				return CollisionTypes::Top;
			}

			return CollisionTypes::None;
		}


		void BlockBreaker::HandleCollision(BlockBreakerGameState& game_state, CollisionTypes collision_type, bool collided_border)
		{
			switch (collision_type)
			{
			case CollisionTypes::Left:
					game_state.ball_direction = Vector2D::Vector2D(-game_state.ball_direction.x, game_state.ball_direction.y);
				break;
			case CollisionTypes::TopLeft:
			case CollisionTypes::Top:
			case CollisionTypes::TopRight:
				game_state.ball_direction = Vector2D::Vector2D(game_state.ball_direction.x, -game_state.ball_direction.y);
				break;
			case CollisionTypes::Right:
				game_state.ball_direction = Vector2D::Vector2D(-game_state.ball_direction.x, game_state.ball_direction.y);
				break;
			case CollisionTypes::BottomRight:
			case CollisionTypes::Bottom:
			case CollisionTypes::BottomLeft:
				if (collided_border)
				{
					game_state.lost = true;
				}
				else
				{
					game_state.ball_direction = Vector2D::Vector2D(game_state.ball_direction.x, -game_state.ball_direction.y);
				}
				break;
			case CollisionTypes::None:
				break;
			default:
				break;
			}
		
		}
		
		bool IntersectsPaddle(BlockBreakerGameState& game_state)
		{
			if (game_state.ball_direction.y < 0)
			{
				return false;
			}
			if (game_state.ball_position.y >= game_state.paddle_position.y - block_breaker_config.paddle_height) // y-coordinate matches
			{
				return game_state.ball_position.x > game_state.paddle_position.x - block_breaker_config.paddle_width / 2
					&& game_state.ball_position.x < game_state.paddle_position.x + block_breaker_config.paddle_width / 2;
			}

			return false;
		}

		void HandlePaddleCollision(BlockBreakerGameState& game_state)
		{
			auto distance_from_paddle_middle = abs(game_state.paddle_position.x - game_state.ball_position.x);

			auto normalized_distance = (distance_from_paddle_middle / (block_breaker_config.paddle_width / 2));
			auto theta_new = normalized_distance * (90 - block_breaker_config.min_theta) + block_breaker_config.min_theta;

			if (distance_from_paddle_middle < 0) // ball is to the right side of the paddle center => negate theta to rotate counter clockwise
			{
				theta_new = -theta_new;
			}
			auto ball_direction_new = Vector2D::Vector2D(cos(theta_new), sin(theta_new)) * game_state.ball_direction.x
				+ Vector2D::Vector2D(sin(theta_new), cos(theta_new)) * game_state.ball_direction.y;

			game_state.ball_direction = Vector2D::Normalize(ball_direction_new) * block_breaker_config.ball_speed;
		}

		CollisionTypes CheckAndHandleBlockCollision(BlockBreakerGameState& game_state)
		{
			CollisionTypes collision_type = CollisionTypes::None;
			std::unordered_set<Block, boost::hash<Block>> block_positions_new = game_state.block_positions;

			for (auto& b : game_state.block_positions)
			{
				collision_type = TestBlockOverlap(b, game_state.ball_position);
				if (collision_type != CollisionTypes::None)
				{
					last_collision = collision_type;
					block_positions_new.erase(b);
					break;
				}
			}

			if (collision_type != CollisionTypes::None)
			{
				game_state.block_positions = block_positions_new;

				if (game_state.block_positions.size() == 0)
				{
					game_state.won = true;
				}

				HandleCollision(game_state, collision_type, false);
			}

			return collision_type;
		}

		CollisionTypes TestBlockOverlap(Block b, Vector2D::Vector2D v)
		{
			double d1x = b.end_left.x - v.x;
			double d1y = b.end_left.y - v.y;
			double d2x = v.x - b.end_right.x;
			double d2y = v.y - b.end_right.y;

			if (d1x > 0.0f || d1y > 0.0f) // compare to top left corner of block
			{
				return CollisionTypes::None;
			}

			if (d2x > 0.0f || d2y > 0.0f) // compare to bottom right corner of block
			{
				return CollisionTypes::None;
			}

			// Check bottom border:
			auto p1 = Vector2D::Vector2D(b.end_left.x, b.end_right.y);
			auto p2 = Vector2D::Vector2D(b.end_right.x, b.end_right.y);
			if (LineSegmentsIntersect(p1, p2, game_state.ball_position_prev, game_state.ball_position))
			{
				return CollisionTypes::Top; // from ball's view collision is top (like with top border)
			}

			// Check top border:
			p1 = Vector2D::Vector2D(b.end_left.x, b.end_left.y);
			p2 = Vector2D::Vector2D(b.end_right.x, b.end_left.y);
			if (LineSegmentsIntersect(p1, p2, game_state.ball_position_prev, game_state.ball_position))
			{
				return CollisionTypes::Bottom;
			}

			// Check left border:
			p1 = Vector2D::Vector2D(b.end_left.x, b.end_left.y);
			p2 = Vector2D::Vector2D(b.end_left.x, b.end_right.y);
			if (LineSegmentsIntersect(p1, p2, game_state.ball_position_prev, game_state.ball_position))
			{
				return CollisionTypes::Right;
			}

			// Check right border:
			p1 = Vector2D::Vector2D(b.end_right.x, b.end_left.y);
			p2 = Vector2D::Vector2D(b.end_right.x, b.end_right.y);
			if (LineSegmentsIntersect(p1, p2, game_state.ball_position_prev, game_state.ball_position))
			{
				return CollisionTypes::Left;
			}
		}

	} // namespace BlockBreaker
} // namespace TerminalMinigames
