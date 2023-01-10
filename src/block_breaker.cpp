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

			float ball_speed_initial = 10.f;
			float ball_radius = 0.5f;

			float speed_increase_factor = 1.01f;

			/**
			 * Minimum angle theta that the ball will be returned at from the paddle.
			 */
			float min_theta = 20.f;
		} block_breaker_config;

		void BlockBreakerGameState::Reset()
		{
			// Init rows of blocks to destroy:
			for (double y = 6.f; y < 19.f; y += 6.f)
			{
				for (double x = 4.f; x < 97.f; x += 8.f)
				{
					Block b = { { x, y }, { x + 5.f, y + 2.f } };
					block_positions.insert(b);
				}
			}

			paddle_position = block_breaker_config.paddle_start_position;
			ball_position = { paddle_position.x, paddle_position.y - block_breaker_config.paddle_height - 2 };
			ball_direction = { 0, -block_breaker_config.ball_speed_initial };
			ball_speed = block_breaker_config.ball_speed_initial;

			lost = false;
		}

		/** **/

		/** Variables needed for execution. **/
		BlockBreakerGameState game_state;
		std::mutex paddle_mutex;
		std::mutex ball_mutex;
		std::mutex block_positions_mutex;

		CollisionTypes last_collision = CollisionTypes::None;

		bool restart_flag;

		/** **/

		void UpdateBall(ftxui::ScreenInteractive& screen, BlockBreakerGameState& state, bool* back_flag)
		{
			auto start = boost::chrono::high_resolution_clock::now();
			double delta_time = 0;

			while (!(*back_flag) && !state.lost && !state.won)
			{
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(1.0s / 30.f);

				// Compute delta time
				delta_time = boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::high_resolution_clock::now() - start).count() / 1000.f;
				start = boost::chrono::high_resolution_clock::now();

				ball_mutex.lock();

				// Move ball
				state.ball_position_prev = state.ball_position;
				auto state_cache = state;
				auto position_change = state.ball_direction * delta_time;
				state.ball_position += state.ball_direction * delta_time;
				state_cache = state;

				// Check & handle border collision
				auto collision_type = IntersectsBorder(state.ball_position, block_breaker_config.ball_radius);
				HandleCollision(state, collision_type, true);

				// Check & handle paddle collision
				if (IntersectsPaddle(state, block_breaker_config.ball_radius))
				{
					HandlePaddleCollision(state);
				}

				// Check & handle block collision
				CheckAndHandleBlockCollision(state, block_breaker_config.ball_radius);

				ball_mutex.unlock();

				screen.PostEvent(ftxui::Event::Custom);
			}
		}

		void UpdateScreen(ftxui::ScreenInteractive& screen, ftxui::Component& comp)
		{
			screen.Loop(comp);
		}

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
			auto restart_button = ftxui::Button(&restart_button_label, [&] { game_state.Reset(); restart_flag = true; });
			container->Add(restart_button);

			auto screen_view_renderer = ftxui::Renderer(container, [&] {
				auto ball_position_text = std::format("Ball Position: {}", game_state.ball_position.ToString());
				auto collision_text = std::format("Collision: {}", ToString(last_collision));
				auto speed_text = std::format("Speed: {}", Vector2D::Magnitude(game_state.ball_direction));

				return ftxui::vbox({
					ftxui::text("Terminal Minigames") | ftxui::bold | ftxui::center,
					ftxui::text(""),
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
			std::thread update_screen(UpdateScreen, std::ref(screen), std::ref(screen_view_event_catch_wrapper));

			std::thread update_ball;
			restart_flag = true;
			while (!(*back_to_menu))
			{
				if (restart_flag) // check if should restart the update ball thread
				{
					update_ball = std::thread(UpdateBall, std::ref(screen), std::ref(game_state), back_to_menu);
					restart_flag = false;
				}
				if (update_ball.joinable())
				{
					update_ball.join();
				}
			}

			update_screen.join();
		}

		CollisionTypes BlockBreaker::IntersectsBorder(Vector2D::Vector2D& pos, float ball_radius)
		{
			if (pos.x - ball_radius < 2 && pos.y - ball_radius < block_breaker_config.board_dimension_y - 3 && pos.y + ball_radius >= 1)
			{
				return CollisionTypes::Left;
			}
			else if (pos.x - ball_radius < 2 && pos.y - ball_radius < 1)
			{
				return CollisionTypes::TopLeft;
			}
			else if (pos.x - ball_radius < 2 && pos.y + ball_radius > block_breaker_config.board_dimension_y - 3)
			{
				return CollisionTypes::BottomLeft;
			}
			else if (pos.x + ball_radius > block_breaker_config.board_dimension_x - 3 && pos.y - ball_radius < block_breaker_config.board_dimension_y - 3 && pos.y + ball_radius >= 1)
			{
				return CollisionTypes::Right;
			}
			else if (pos.x + ball_radius > block_breaker_config.board_dimension_x - 3 && pos.y - ball_radius < 1)
			{
				return CollisionTypes::TopRight;
			}
			else if (pos.x + ball_radius > block_breaker_config.board_dimension_x - 3 && pos.y + ball_radius > block_breaker_config.board_dimension_y - 3)
			{
				return CollisionTypes::BottomRight;
			}
			else if (pos.y + ball_radius > block_breaker_config.board_dimension_y - 3)
			{
				return CollisionTypes::Bottom;
			}
			else if (pos.y - ball_radius < 3)
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
		
		bool IntersectsPaddle(BlockBreakerGameState& game_state, float ball_radius)
		{
			if (game_state.ball_direction.y < 0)
			{
				return false;
			}
			if (game_state.ball_position.y + ball_radius >= game_state.paddle_position.y - block_breaker_config.paddle_height) // y-coordinate matches
			{
				return game_state.ball_position.x + ball_radius > game_state.paddle_position.x - block_breaker_config.paddle_width / 2
					&& game_state.ball_position.x - ball_radius < game_state.paddle_position.x + block_breaker_config.paddle_width / 2;
			}

			return false;
		}

		void HandlePaddleCollision(BlockBreakerGameState& game_state)
		{
			auto distance_from_paddle_middle = abs(game_state.paddle_position.x - game_state.ball_position.x);

			auto normalized_distance = 1 - (distance_from_paddle_middle / (block_breaker_config.paddle_width / 2));
			auto theta_new = normalized_distance * (90 - block_breaker_config.min_theta) + block_breaker_config.min_theta;


			auto x_new = cos(DegreesToRadians(theta_new));
			auto y_new = sin(DegreesToRadians(theta_new));

			if (game_state.paddle_position.x - game_state.ball_position.x > 0) // ball is to the left side of the paddle center
			{
				x_new = -x_new;
			}

			auto ball_direction_new = Vector2D::Vector2D(x_new, -y_new) * game_state.ball_speed;

			game_state.ball_speed *= block_breaker_config.speed_increase_factor;

			game_state.ball_direction = Vector2D::Normalize(ball_direction_new) * game_state.ball_speed;
		}

		CollisionTypes CheckAndHandleBlockCollision(BlockBreakerGameState& game_state, float ball_radius)
		{
			CollisionTypes collision_type = CollisionTypes::None;
			std::unordered_set<Block, boost::hash<Block>> block_positions_new = game_state.block_positions;

			for (auto& b : game_state.block_positions)
			{
				collision_type = TestBlockOverlap(b, game_state.ball_position, ball_radius);
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

		CollisionTypes TestBlockOverlap(Block b, Vector2D::Vector2D v, float ball_radius)
		{
			double d1x = b.end_left.x - v.x + ball_radius;
			double d1y = b.end_left.y - v.y + ball_radius;
			double d2x = (v.x - ball_radius) - b.end_right.x;
			double d2y = (v.y - ball_radius) - b.end_right.y;

			if (d1x > 0.0f || d1y > 0.0f) // compare to top left corner of block
			{
				return CollisionTypes::None;
			}

			if (d2x > 0.0f || d2y > 0.0f) // compare to bottom right corner of block
			{
				return CollisionTypes::None;
			}

			// Check collision with a block's bottom border:
			auto p1 = Vector2D::Vector2D(b.end_left.x, b.end_right.y);
			auto p2 = Vector2D::Vector2D(b.end_right.x, b.end_right.y);
			if (LineSegmentsIntersect(p1, p2, game_state.ball_position_prev, game_state.ball_position + Vector2D::Vector2D(0, -ball_radius)))
			{
				return CollisionTypes::Top; // from ball's view collision is top (like with top border)
			}

			// Check top border:
			p1 = Vector2D::Vector2D(b.end_left.x, b.end_left.y);
			p2 = Vector2D::Vector2D(b.end_right.x, b.end_left.y);
			if (LineSegmentsIntersect(p1, p2, game_state.ball_position_prev, game_state.ball_position + Vector2D::Vector2D(0, ball_radius)))
			{
				return CollisionTypes::Bottom;
			}

			// Check left border:
			p1 = Vector2D::Vector2D(b.end_left.x, b.end_left.y);
			p2 = Vector2D::Vector2D(b.end_left.x, b.end_right.y);
			if (LineSegmentsIntersect(p1, p2, game_state.ball_position_prev, game_state.ball_position + Vector2D::Vector2D(ball_radius, 0)))
			{
				return CollisionTypes::Right;
			}

			// Check right border:
			p1 = Vector2D::Vector2D(b.end_right.x, b.end_left.y);
			p2 = Vector2D::Vector2D(b.end_right.x, b.end_right.y);
			if (LineSegmentsIntersect(p1, p2, game_state.ball_position_prev, game_state.ball_position + Vector2D::Vector2D(-ball_radius, 0)))
			{
				return CollisionTypes::Left;
			}

			return CollisionTypes::None;
		}

	} // namespace BlockBreaker
} // namespace TerminalMinigames
