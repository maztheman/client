#pragma once
#include "utility.hpp"

#include <imgui.h>
#include <imgui-SFML.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>

namespace kms {

#ifdef _WIN32
	class console_t {
		int							m_xCoord;
		int							m_yCoord;
		HANDLE						m_hPrintF;
		CONSOLE_SCREEN_BUFFER_INFO	m_info;
		CONSOLE_FONT_INFOEX			m_font_info;
		std::vector<COORD>			m_arSaved;
		int							m_textAttribute;
		bool						m_bLocalEcho;

	public:
		console_t()
			: m_xCoord(0)
			, m_yCoord(0)
			, m_hPrintF(GetStdHandle(STD_OUTPUT_HANDLE))
			, m_textAttribute(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
		{
			GetConsoleScreenBufferInfo(m_hPrintF, &m_info);
			GetCurrentConsoleFontEx(m_hPrintF, FALSE, &m_font_info);
		}

		void setLocalEcho(bool bValue) {
			m_bLocalEcho = bValue;
		}

		void setCoord(int x, int y) {
			COORD inp = {x, y};
			m_xCoord = x;
			m_yCoord = y;
			SetConsoleCursorPosition(m_hPrintF, inp);
		}

		void clearScreen(terminal_attrib::terminal_attrib attrib)
		{
			int buff = 0;
			if (attrib & terminal_attrib::bg_bold) {
				buff |= BACKGROUND_INTENSITY;
			}
			if (attrib & terminal_attrib::bg_red) {
				buff |= BACKGROUND_RED;
			}
			if (attrib & terminal_attrib::bg_green) {
				buff |= BACKGROUND_GREEN;
			}
			if (attrib & terminal_attrib::bg_blue) {
				buff |= BACKGROUND_BLUE;
			}
			COORD outp = {0, 0};
			DWORD attrWritten = 0;
			for(int y = 0;y < m_info.dwSize.Y; y++) {
				outp.Y = y;
				::FillConsoleOutputAttribute(m_hPrintF, buff, m_info.dwSize.X, outp, &attrWritten);
			}
		}

		void clearFromCurrent()
		{
			//shitty way to clear a line
			COORD outp = {0, 0};
			DWORD charWritten = 0;
			for(int y = m_yCoord;y < m_info.dwSize.Y; y++) {
				outp.Y = y;
				::FillConsoleOutputCharacter(m_hPrintF, ' ', m_info.dwSize.X, outp, &charWritten);
			}
		}

		void clearToCurrent() 
		{
			//shitty way to clear a line
			CCharVector xBuff(m_info.dwSize.X, ' ');
			COORD outp = {0, 0};
			DWORD charWritten = 0;
			for(int y = 0;y < m_yCoord; y++) {
				outp.Y = y;
				::FillConsoleOutputCharacter(m_hPrintF, ' ', m_info.dwSize.X, outp, &charWritten);
			}
		}

		void writeText(const std::string& sText)
		{
			printf("%s", sText.c_str());
		}

		void moveCursorUp(int count = 1)
		{
			m_yCoord -= count;
			setCoord(m_xCoord, m_yCoord);
		}

		void moveCursorDown(int count = 1)
		{
			m_yCoord += count;
			setCoord(m_xCoord, m_yCoord);
		}

		void moveCursorForward(int count = 1)
		{
			m_xCoord += count;
			setCoord(m_xCoord, m_yCoord);
		}

		void moveCursorBackward(int count = 1)
		{
			m_xCoord -= count;
			setCoord(m_xCoord, m_yCoord);
		}

		void saveCursor()
		{
			COORD inp = {m_xCoord, m_yCoord};
			m_arSaved.push_back(inp);
		}

		void unSaveCursor()
		{
			COORD inp = m_arSaved.back();
			m_xCoord = inp.X;
			m_yCoord = inp.Y;
			m_arSaved.pop_back();
		}

		void eraseLineFromCursor()
		{
			eraseLineFromX(m_xCoord);
		}

		void eraseLineFromX(int x) 
		{
			//shitty way to clear a line
			CCharVector xBuff(m_info.dwSize.X - x, ' ');
			COORD outp = {x, m_yCoord};
			DWORD charWritten = 0;
			WriteConsoleOutputCharacter(m_hPrintF, &xBuff[0], xBuff.size(), outp, &charWritten);
		}

		void eraseLineToCursor()
		{
			eraseLineToX(m_xCoord);
		}

		void eraseLineToX(int x)
		{//could be off by 1...
			//shitty way to clear a line
			CCharVector xBuff(x, ' ');
			COORD outp = {0, m_yCoord};
			DWORD charWritten = 0;
			WriteConsoleOutputCharacter(m_hPrintF, &xBuff[0], xBuff.size(), outp, &charWritten);
		}

		void setTextColor(terminal_attrib::terminal_attrib attrib)
		{
			int buff = 0;
			if (attrib & terminal_attrib::fg_bold) {
				buff |= FOREGROUND_INTENSITY;
			}
			if (attrib & terminal_attrib::fg_red) {
				buff |= FOREGROUND_RED;
			}
			if (attrib & terminal_attrib::fg_green) {
				buff |= FOREGROUND_GREEN;
			}
			if (attrib & terminal_attrib::fg_blue) {
				buff |= FOREGROUND_BLUE;
			}
			if (attrib & terminal_attrib::bg_bold) {
				buff |= BACKGROUND_INTENSITY;
			}
			if (attrib & terminal_attrib::bg_red) {
				buff |= BACKGROUND_RED;
			}
			if (attrib & terminal_attrib::bg_green) {
				buff |= BACKGROUND_GREEN;
			}
			if (attrib & terminal_attrib::bg_blue) {
				buff |= BACKGROUND_BLUE;
			}

			m_textAttribute = buff;
			SetConsoleTextAttribute(m_hPrintF, buff);
		}

		void setBold(bool bEnabled)
		{
			m_font_info.FontWeight = bEnabled ? 700 : 400;
			SetCurrentConsoleFontEx(m_hPrintF, FALSE, &m_font_info);
		}

		void setItalic(bool bEnabled)
		{
			SetCurrentConsoleFontEx(m_hPrintF, FALSE, &m_font_info);
		}

		void setUnderline(bool bEnabled)
		{
			SetCurrentConsoleFontEx(m_hPrintF, FALSE, &m_font_info);
		}

	constexpr static bool isTTY() {
		return false;
	}

	};
#else



class console_t
{
	static constexpr tcflag_t ECHO_FLAG = ECHO;

	termios m_termAttributes;
	//sf::RenderWindow window;

public:
	console_t()
	//: window(sf::VideoMode(640, 480), "mudclient")
	{
		/*window.setFramerateLimit(60);
  		ImGui::SFML::Init(window);
		sf::Clock deltaClock;
		if (window.isOpen()) {

			sf::Event event;
			while (window.pollEvent(event)) {
				ImGui::SFML::ProcessEvent(event);

				if (event.type == sf::Event::Closed) {
					window.close();
				}
			}

			ImGui::SFML::Update(window, deltaClock.restart());

			ImGui::Begin("Hello, world!");
			ImGui::End();

			window.clear();
			ImGui::SFML::Render(window);
			window.display();
		}
		*/

		tcgetattr(STDERR_FILENO, &m_termAttributes);
		setLocalEcho(true);
	}

	~console_t()
	{
		 ImGui::SFML::Shutdown();
	}

	void setLocalEcho(bool bValue) {
		if (bValue) {
			m_termAttributes.c_lflag |= ECHO_FLAG;
		} else {
			m_termAttributes.c_lflag &= ~ECHO_FLAG;
		}

		tcsetattr(STDERR_FILENO, TCSANOW, &m_termAttributes);
	}

	void writeText(const std::string& sText)
	{
		fmt::print(stderr, "{}", sText);
	}

	constexpr static bool isTTY() {
		return true;
	}
};


#endif
} // end namespace kms