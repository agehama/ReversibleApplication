/*
参照：Siv3D/Reference-JP - スケッチ
https://github.com/Siv3D/Reference-JP/wiki/%E3%82%B9%E3%82%B1%E3%83%83%E3%83%81
*/

#include <Siv3D.hpp>
#include "../../ReversibleApplication.hpp"

class Sketch
{
public:

	Sketch(const Size& size = Window::Size(), const Color& color = Palette::White) :
		m_image(size, color),
		m_texture(m_image)
	{}
	
	void update()
	{
		if (Input::MouseL.pressed)
		{
			Line(Mouse::PreviousPos(), Mouse::Pos()).overwrite(m_image, 8, Palette::Orange);

			m_texture.fill(m_image);
		}

		if (Input::MouseL.released)
		{
			m_newAction = String(L"線を描く");
		}
	}

	void draw()const
	{
		m_texture.draw();
	}

	template <class Archive>
	void save(Archive& archive)const
	{
		archive(m_image);
	}

	template <class Archive>
	void load(Archive& archive)
	{
		archive(m_image);
		m_texture.fill(m_image);
	}

	Optional<String> popAction()
	{
		const Optional<String> result = m_newAction;
		m_newAction = none;
		return result;
	}

private:

	Image m_image;
	DynamicTexture m_texture;
	Optional<String> m_newAction;
};

void Main()
{
	ReversibleApplicationGUI<Sketch> app(Sketch{});

	while (System::Update())
	{
		app.update();
		app.draw();
	}
}