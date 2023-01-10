#include "vector2d.h"

namespace Vector2D
{
	Vector2D RotateBy90DegreesClockwise(Vector2D direction)
	{
		return { -direction.y, direction.x };
	}

	Vector2D RotateBy90DegreesCounterClockwise(Vector2D direction)
	{
		return { direction.y, -direction.x };

	}
}
