/*
éQè∆ÅFSiv3D/Reference-JP - ìdëÏ
https://github.com/Siv3D/Reference-JP/wiki/%E9%9B%BB%E5%8D%93
*/

#include <Siv3D.hpp>

class Calculator
{
public:

	Calculator() :
		m_font(40)
	{}

	void update()
	{
		Input::GetCharsHelper(m_expression);
	}

	void draw()
	{
		m_font(m_expression).draw(10, 10, Palette::Black);

		if (const auto result = EvaluateOpt(m_expression))
		{
			m_font(result.value()).draw(10, 250, Palette::Black);
		}
	}

private:

	Font m_font;
	String m_expression;
};

void Main()
{
	Graphics::SetBackground(Color(240));

	Calculator calculator;

	while (System::Update())
	{
		calculator.update();
		calculator.draw();
	}
}