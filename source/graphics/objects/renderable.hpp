#pragma once

namespace Khan
{
	class RenderContext;

	class Renderable
	{
	public:
		virtual void Draw(RenderContext& context) = 0;
	};
}