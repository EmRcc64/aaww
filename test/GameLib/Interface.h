#pragma once

class IBackground : public CSingleton<IBackground>
{
	public:
		IBackground() {}
		virtual ~IBackground() {}

		virtual bool IsBlock(int x, int y) = 0;
};
//martysama0134's 747bda46b83d0f642ccb846d9a8c1cbe
