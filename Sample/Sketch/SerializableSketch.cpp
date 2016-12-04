/*
参照：Siv3D/Reference-JP - スケッチ
https://github.com/Siv3D/Reference-JP/wiki/%E3%82%B9%E3%82%B1%E3%83%83%E3%83%81
*/

#include <Siv3D.hpp>

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

private:

	Image m_image;
	DynamicTexture m_texture;
};

void Main()
{
	//Sketchクラスがセーブ/ロードできるようになった。
	//動作はSketch.cppと変わらない。

	Sketch sketch;

	Serializer<BinaryWriter> writer(L"save.bin");
	writer(sketch);
	writer.getWriter().close();

	Deserializer<BinaryReader> reader(L"save.bin");
	reader(sketch);

	while (System::Update())
	{
		sketch.update();
		sketch.draw();
	}
}