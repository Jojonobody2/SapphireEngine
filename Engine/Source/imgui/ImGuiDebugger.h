#pragma once

#include <imgui.h>

#include <vector>
#include <string>

namespace Sapphire
{
	struct Message
	{
		std::string Text;
		ImVec4 Color;
	};

	class Debugger
	{
	public:
		Debugger();
		virtual ~Debugger();

		void AddInfo(std::string Text);
		void AddWarn(std::string Text);
		void AddError(std::string Text);
		void AddCritical(std::string Text);

		void Draw();

	private:
		std::vector<Message> m_Items;
		ImGuiTextFilter m_Filter;
		bool m_AutoScroll;
		bool m_ScrollToBottom;

		void ClearLog();
		void AddLog(const Message& Message);
	};
}