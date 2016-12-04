#pragma once
#include <deque>
#include <Siv3D.hpp>

template<class Application>
class ReversibleApplication
{
public:

	ReversibleApplication(const Application& application, size_t maxHistorySize = 20);

	void save(const String& actionName);

	void load(int index);

	void undo();

	void redo();

	void update()
	{
		m_application.update();

		if (Input::KeyControl.pressed && Input::KeyZ.clicked)
		{
			undo();
		}
		if (Input::KeyControl.pressed && Input::KeyY.clicked)
		{
			redo();
		}

		if (auto newAction = m_application.popAction())
		{
			save(newAction.value());
		}
	}

	void draw()
	{
		m_application.draw();
	}

private:

	struct History
	{
		String actionName;
		Serializer<MemoryWriter> serializer;

		Deserializer<ByteArray> deserializer()
		{
			auto& writer = serializer.getWriter();
			return Deserializer<ByteArray>(writer.data(), writer.size());
		}
	};

	void load()
	{
		(*m_currentState)->deserializer()(m_application);
	}

	Application m_application;

	std::deque<std::unique_ptr<History>> m_histories;
	typename std::deque<std::unique_ptr<History>>::iterator m_currentState;

	size_t m_maxHistorySize;
};

template<class Application>
class ReversibleApplicationGUI
{
public:

	ReversibleApplicationGUI(const Application& application, size_t maxHistorySize = 20, int32 guiWidth = Window::Width() / 5);

	void save(const String& actionName);

	void load(int index);

	void undo();

	void redo();

	void update()
	{
		if (!m_gui.getRect().mouseOver)
		{
			m_application.update();
		}

		if (Input::KeyControl.pressed && Input::KeyZ.clicked)
		{
			undo();
		}
		if (Input::KeyControl.pressed && Input::KeyY.clicked)
		{
			redo();
		}

		if (auto newAction = m_application.popAction())
		{
			save(newAction.value());
		}

		guiUpdate();
	}

	void draw()
	{
		m_application.draw();

		guiDraw();
	}

private:

	struct History
	{
		String actionName;
		Serializer<MemoryWriter> serializer;
		unsigned size;

		Deserializer<ByteArray> deserializer()
		{
			auto& writer = serializer.getWriter();
			return Deserializer<ByteArray>(writer.data(), writer.size());
		}

		void setSize()
		{
			auto& writer = serializer.getWriter();
			size = static_cast<unsigned>(writer.size());
		}

		String formattedSize()
		{
			const String suffixes[4] = { L"  B",L"KB",L"MB",L"GB" };
			const unsigned b1023 = 0x3ffu;
			for (int digits = 3; 0 <= digits; --digits)
			{
				const int shiftBits = digits * 10;
				if ((size & (b1023 << shiftBits)) != 0u)
				{
					return Pad((size & (b1023 << shiftBits)) >> shiftBits, { 4, L'0' }) + suffixes[digits];
				}
			}
			return L"";
		}
	};

	Rect historyRect()
	{
		return Rect(m_renderTexture.size).setPos(m_gui.getPos() + Point(16, 44));
	}

	void load()
	{
		(*m_currentState)->deserializer()(m_application);
	}

	void setNewest()
	{
		m_topHistoryIndex = newestTopIndex();
	}

	int oldestTopIndex()
	{
		return 0;
	}
	int newestTopIndex()
	{
		return Max(0, static_cast<int>(m_histories.size()) - 10);
	}

	void moveTopIndex(int delta)
	{
		m_topHistoryIndex = Clamp(m_topHistoryIndex + delta, oldestTopIndex(), newestTopIndex());
	}

	void guiUpdate();

	void guiDraw();

	GUI m_gui;
	Font m_font;
	int m_height;
	RenderTexture m_renderTexture;
	int m_topHistoryIndex = 0;

	Application m_application;

	std::deque<std::unique_ptr<History>> m_histories;
	typename std::deque<std::unique_ptr<History>>::iterator m_currentState;

	size_t m_maxHistorySize;
};


template<class Application>
ReversibleApplication<Application>::ReversibleApplication(const Application& application, size_t maxHistorySize) :
	m_application(application),
	m_maxHistorySize(maxHistorySize)
{
	m_histories.emplace_back();

	m_histories.front().reset(new History);
	m_histories.front()->actionName = L"初期状態";
	m_histories.front()->serializer(m_application);

	m_currentState = m_histories.begin();
}

template<class Application>
void ReversibleApplication<Application>::save(const String& actionName)
{
	//現在の状態が履歴の最新要素ではない場合、現在の状態よりも後ろを全て削除してから次の状態を追加する
	if (m_currentState + 1 != m_histories.end())
	{
		m_histories.erase(m_currentState + 1, m_histories.end());
	}

	m_histories.emplace_back();
	m_histories.back().reset(new History);
	m_currentState = m_histories.end() - 1;

	while (m_maxHistorySize < m_histories.size())
	{
		m_histories.pop_front();
	}

	(*m_currentState)->actionName = actionName;
	(*m_currentState)->serializer(m_application);
}

template<class Application>
void ReversibleApplication<Application>::load(int index)
{
	if (0 <= index && index < static_cast<int>(m_histories.size()))
	{
		m_currentState = m_histories.begin() + index;

		load();
	}
}

template<class Application>
void ReversibleApplication<Application>::undo()
{
	if (m_histories.begin() < m_currentState)
	{
		--m_currentState;

		load();
	}
}

template<class Application>
void ReversibleApplication<Application>::redo()
{
	if (m_currentState + 1 < m_histories.end())
	{
		++m_currentState;

		load();
	}
}

template<class Application>
ReversibleApplicationGUI<Application>::ReversibleApplicationGUI(const Application& application, size_t maxHistorySize, int32 guiWidth) :
	m_font(8),
	m_height(m_font(L"テスト").region().h),
	m_renderTexture(guiWidth, static_cast<int32>(m_height*10.5), ColorF(0.0, 0.0)),
	m_gui(GUIStyle::Default),
	m_application(application),
	m_maxHistorySize(maxHistorySize)
{
	m_histories.emplace_back();

	m_histories.front().reset(new History);
	m_histories.front()->actionName = L"初期状態";
	m_histories.front()->serializer(m_application);
	m_histories.front()->setSize();

	m_currentState = m_histories.begin();

	//m_gui.setPos(Window::Width() * 4 / 5 - 20, 0);
	m_gui.setPos(Window::Width() - guiWidth - 20, 0);
	m_gui.setTitle(L"履歴");
	m_gui.add(L"Tex", GUITexture::Create(m_renderTexture));
}

template<class Application>
void ReversibleApplicationGUI<Application>::save(const String& actionName)
{
	//現在の状態が履歴の最新要素ではない場合、現在の状態よりも後ろを全て削除してから次の状態を追加する
	if (m_currentState + 1 != m_histories.end())
	{
		m_histories.erase(m_currentState + 1, m_histories.end());
	}

	m_histories.emplace_back();
	m_histories.back().reset(new History);
	m_currentState = m_histories.end() - 1;

	while (m_maxHistorySize < m_histories.size())
	{
		m_histories.pop_front();
	}

	(*m_currentState)->actionName = actionName;
	(*m_currentState)->serializer(m_application);
	(*m_currentState)->setSize();

	setNewest();
}

template<class Application>
void ReversibleApplicationGUI<Application>::load(int index)
{
	if (0 <= index && index < static_cast<int>(m_histories.size()))
	{
		m_currentState = m_histories.begin() + index;

		load();
	}
}

template<class Application>
void ReversibleApplicationGUI<Application>::undo()
{
	if (m_histories.begin() < m_currentState)
	{
		--m_currentState;

		load();
	}
}

template<class Application>
void ReversibleApplicationGUI<Application>::redo()
{
	if (m_currentState + 1 < m_histories.end())
	{
		++m_currentState;

		load();
	}
}

template<class Application>
void ReversibleApplicationGUI<Application>::guiUpdate()
{
	const auto rect = historyRect();
	if (rect.mouseOver)
	{
		const int wheel = Mouse::Wheel();
		moveTopIndex(wheel);
	}

	if (rect.leftClicked)
	{
		for (int i = m_topHistoryIndex, n = 0; i < static_cast<int>(m_histories.size()) && n < 11; ++i, ++n)
		{
			const Vec2 pos(0, n*m_height);
			//const RectF currentRect = RectF(Window::Size().x*0.2, m_height).setPos(pos).moveBy(rect.pos);
			
			const RectF currentRect = RectF(m_renderTexture.width, m_height).setPos(pos).moveBy(rect.pos);

			if (currentRect.mouseOver)
			{
				load(i);
				break;
			}
		}
	}
}

template<class Application>
void ReversibleApplicationGUI<Application>::guiDraw()
{
	const int height = m_font(L"テスト").region().h;

	const int domainHeight = m_renderTexture.height;

	const auto currentIndex = std::distance(m_histories.begin(), m_currentState);

	m_renderTexture.clear(Palette::Darkgray);

	Graphics2D::SetRenderTarget(m_renderTexture);
	Graphics2D::SetBlendState(BlendState::Opaque);
	for (int i = m_topHistoryIndex, n = 0; i < static_cast<int>(m_histories.size()) && n < 11; ++i, ++n)
	{
		const Vec2 pos(0, n*height);
		//const RectF rect = RectF(Window::Size().x*0.2, height).setPos(pos);
		const RectF rect = RectF(m_renderTexture.width, height).setPos(pos);

		rect.draw(Palette::Blue);
		if (i == currentIndex)
		{
			rect.drawFrame(1.0, 0.0, Palette::White);
		}
	}

	Graphics2D::SetBlendState(BlendState::Default);
	for (int i = m_topHistoryIndex, n = 0; i < static_cast<int>(m_histories.size()) && n < 11; ++i, ++n)
	{
		const Vec2 pos0(0, n*height);
		const Vec2 pos1(40, n*height);
		m_font.draw(Format(m_histories[i]->formattedSize()), pos0);
		m_font(L" | ", m_histories[i]->actionName).draw(pos1);
	}

	{
		const RectF scrollBar = Rect(m_renderTexture.size).stretched(0, 0, 0, height - m_renderTexture.width);
		scrollBar.draw(Palette::Lightgrey);

		const int drawnRects = Min(static_cast<int>(m_histories.size()) - m_topHistoryIndex, 10);
		const double verticalScale = 1.0*drawnRects / m_histories.size();
		const double verticalPos = 1.0*m_topHistoryIndex / m_histories.size();
		scrollBar.scaled(1.0, verticalScale).setPos(scrollBar.x, scrollBar.h*verticalPos).draw(Palette::Black);
	}

	Graphics2D::SetRenderTarget(Graphics::GetSwapChainTexture());
	Graphics2D::SetBlendState(BlendState::Default);
}
