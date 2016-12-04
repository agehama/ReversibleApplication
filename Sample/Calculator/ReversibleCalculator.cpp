/*
éQè∆ÅFSiv3D/Reference-JP - ìdëÏ
https://github.com/Siv3D/Reference-JP/wiki/%E9%9B%BB%E5%8D%93
*/

#include <Siv3D.hpp>
#include "../../ReversibleApplication.hpp"

class Calculator
{
public:

	Calculator() :
		m_font(40)
	{}

	void update()
	{
		Input::GetCharsHelper(m_expression);

		if (const auto result = EvaluateOpt(m_expression))
		{
			if (Input::MouseL.clicked)
			{
				m_newAction = Format(m_expression, L" = ", result.value());
			}
		}
	}

	void draw()
	{
		m_font(m_expression).draw(10, 10, Palette::Black);

		if (const auto result = EvaluateOpt(m_expression))
		{
			m_font(result.value()).draw(10, 250, Palette::Black);
		}
	}

	template <class Archive>
	void serialize(Archive& archive)
	{
		archive(m_expression);
	}

	Optional<String> popAction()
	{
		const Optional<String> result = m_newAction;
		m_newAction = none;
		return result;
	}

private:

	Font m_font;
	String m_expression;
	Optional<String> m_newAction;
};

void Main()
{
	Graphics::SetBackground(Color(240));

	ReversibleApplicationGUI<Calculator> calculator(Calculator{}, 20u, 200);

	while (System::Update())
	{
		calculator.update();
		calculator.draw();
	}
}