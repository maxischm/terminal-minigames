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

		struct BlockBreakerConfig
		{
			int board_dimension_x = 102;
			int board_dimension_y = 100;

			int paddle_width = 14;
			int paddle_height = 1;
			int paddle_step_size = 2;
			Vector2D::Vector2D paddle_start_position = { 47, 88 };
			float ball_speed = 5.f;

			/**
			 * Minimum angle theta that the ball will be returned at from the paddle.
			 */
			float min_theta = 20.f;
		} block_breaker_config;

		void BlockBreakerGameState::Reset(BlockBreakerGameState& game_state)
		{
			// Init rows of blocks to destroy:
			for (float y = 6; y < 11; y += 4)
			{
				for (float x = 4; x < 97; x += 8)
				{
					Block b = { { x, y }, { x + 5, y } };
					block_positions.insert(b);
				}
			}

			score = 0;
			game_state.paddle_position = block_breaker_config.paddle_start_position;
			game_state.ball_position = { paddle_position.x, paddle_position.y - block_breaker_config.paddle_height - 2 };
			game_state.ball_direction = { -block_breaker_config.ball_speed, -block_breaker_config.ball_speed };
		}

		/** **/

		/** Variables needed for execution. **/

		BlockBreakerGameState game_state;

		std::mutex paddle_mutex;
		std::mutex ball_mutex;
		std::mutex block_positions_mutex;

		BorderCollisions last_collision = BorderCollisions::None;

		/** **/

		void ExecuteBlockBreaker(QuitFunction quit_function, bool* back_to_menu)
		{
			game_state.Reset(game_state);

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
					else
					{
						// Draw ball:
						ball_mutex.lock();
						canvas.DrawPoint(game_state.ball_position.x, game_state.ball_position.y, true);
						ball_mutex.unlock();

						// Draw blocks:
						/*block_positions_mutex.lock();
						for (auto& b : game_state.block_positions)
						{
							canvas.DrawBlockLine(b.end_left.x, b.end_left.y, b.end_right.x, b.end_right.y);
						}
						block_positions_mutex.unlock();*/
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
			auto restart_button = ftxui::Button(&restart_button_label, [&] { game_state.Reset(game_state); });
			container->Add(restart_button);

			auto screen_view_renderer = ftxui::Renderer(container, [&] {
				auto score_text = std::format("Score: {}", game_state.score);
				auto ball_position_text = std::format("Ball Position: {}", game_state.ball_position.ToString());
				auto collision_text = std::format("Collision: {}", ToString(last_collision));

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
					ftxui::text(collision_text)
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
					else if (e == ftxui::Event::ArrowDown)
					{
						//game_state.last_input = InputDirection::Down;
						return true;
					}
					else if (e == ftxui::Event::ArrowUp)
					{
						//game_state.last_input = InputDirection::Up;
						return true;
					}
					return false;  
				});

			// Create update thread
			std::thread update_ball([&] {
				auto start = boost::chrono::high_resolution_clock::now();
				double delta_time = 0;

				while (!(*back_to_menu) && !game_state.lost)
				{
					using namespace std::chrono_literals;
					std::this_thread::sleep_for(1.0s / 30.f);

					// Compute delta time
					delta_time = boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - start).count() / 1000.f;
					start = boost::chrono::high_resolution_clock::now();

					// Check for collision before or after movement?
					auto game_state_cache = game_state;
					auto position_change = game_state.ball_direction * delta_time;
					game_state.ball_position += game_state.ball_direction * delta_time;
					game_state_cache = game_state;

					// Check & handle border collision
					auto collision_type = IntersectsBorder(game_state.ball_position);
					HandleBorderCollision(game_state, collision_type);

					if (IntersectsPaddle(game_state))
					{
						HandlePaddleCollision(game_state);
					}

					screen.PostEvent(ftxui::Event::Custom);
				}
			});


			screen.Loop(screen_view_event_catch_wrapper);
			update_ball.join();
		}

		BorderCollisions BlockBreaker::IntersectsBorder(Vector2D::Vector2D& pos)
		{
			if (pos.x < 2 && pos.y < block_breaker_config.board_dimension_y - 3 && pos.y >= 1)
			{
				return BorderCollisions::Left;
			}
			else if (pos.x < 2 && pos.y < 1)
			{
				return BorderCollisions::TopLeft;
			}
			else if (pos.x < 2 && pos.y > block_breaker_config.board_dimension_y - 3)
			{
				return BorderCollisions::BottomLeft;
			}
			else if (pos.x > block_breaker_config.board_dimension_x - 3 && pos.y < block_breaker_config.board_dimension_y - 3 && pos.y >= 1)
			{
				return BorderCollisions::Right;
			}
			else if (pos.x > block_breaker_config.board_dimension_x - 3 && pos.y < 1)
			{
				return BorderCollisions::TopRight;
			}
			else if (pos.x > block_breaker_config.board_dimension_x - 3 && pos.y > block_breaker_config.board_dimension_y - 3)
			{
				return BorderCollisions::BottomRight;
			}
			else if (pos.y > block_breaker_config.board_dimension_y - 3)
			{
				return BorderCollisions::Bottom;
			}
			else if (pos.y < 3)
			{
				return BorderCollisions::Top;
			}

			return BorderCollisions::None;
		}


		void BlockBreaker::HandleBorderCollision(BlockBreakerGameState& game_state, BorderCollisions collision_type)
		{
			switch (collision_type)
			{
			case BorderCollisions::Left:
				if (game_state.ball_direction.y > 0) // down movement
				{
					game_state.ball_direction = RotateBy90DegreesCounterClockwise(game_state.ball_direction);
				}
				else // up movement
				{
					game_state.ball_direction = RotateBy90DegreesClockwise(game_state.ball_direction);
				}
				break;
			case BorderCollisions::TopLeft:
			case BorderCollisions::Top:
			case BorderCollisions::TopRight:
				// FIXME: Broken
				if (game_state.ball_direction.x > 0)
				{
					game_state.ball_direction = RotateBy90DegreesClockwise(game_state.ball_direction);
				}
				else
				{
					game_state.ball_direction = RotateBy90DegreesCounterClockwise(game_state.ball_direction);
				}
				break;
			case BorderCollisions::Right:
				if (game_state.ball_direction.y > 0) // down movement
				{
					game_state.ball_direction = RotateBy90DegreesClockwise(game_state.ball_direction);
				}
				else // up movement
				{
					game_state.ball_direction = RotateBy90DegreesCounterClockwise(game_state.ball_direction);
				}
				break;
			case BorderCollisions::BottomRight:
			case BorderCollisions::Bottom:
			case BorderCollisions::BottomLeft:
				game_state.lost = true;
				break;
			case BorderCollisions::None:
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

			game_state.ball_direction = ball_direction_new;
		}

		Vector2D::Vector2D RotateBy90DegreesClockwise(Vector2D::Vector2D direction)
		{
			return { -direction.y, direction.x };
		}

		Vector2D::Vector2D RotateBy90DegreesCounterClockwise(Vector2D::Vector2D direction)
		{
			return { direction.y, -direction.x };

		}

	} // namespace BlockBreaker
} // namespace TerminalMinigames
