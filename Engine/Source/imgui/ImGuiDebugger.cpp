#include "ImGuiDebugger.h"

namespace Sapphire
{
	Debugger::Debugger()
	{
		ClearLog();

		m_AutoScroll = true;
		m_ScrollToBottom = false;
	}

	Debugger::~Debugger()
	{
		ClearLog();
	}

	void Debugger::AddInfo(std::string Text)
	{
		AddLog({ Text, {1.0f, 1.0, 1.0f, 1.0f} });
	}

	void Debugger::AddWarn(std::string Text)
	{
		AddLog({ Text, { 0.95f, 0.80f, 0.1f, 1.0f} });
	}

	void Debugger::AddError(std::string Text)
	{
		AddLog({ Text, { 1.0f, 0.1f, 0.1f, 1.0f} });
	}

	void Debugger::AddCritical(std::string Text)
	{
		AddLog({ Text, { 1.0f, 0.0f, 0.0f, 1.0f} });
	}

	void Debugger::ClearLog()
	{
		m_Items.clear();
	}

	void Debugger::AddLog(const Message& Message)
	{
		m_Items.push_back(Message);
	}

	void Debugger::Draw()
	{
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Debugger", nullptr))
		{
			ImGui::End();
			return;
		}

		if (ImGui::SmallButton("Clear")) { ClearLog(); }
		ImGui::SameLine();
		bool CopyToClipboard = ImGui::SmallButton("Copy");

		ImGui::Separator();

		if (ImGui::BeginPopup("Options"))
		{
			ImGui::Checkbox("Auto-Scroll", &m_AutoScroll);
			ImGui::EndPopup();
		}

		if (ImGui::Button("Options"))
		{
			ImGui::OpenPopup("Options");
		}

		ImGui::SameLine();
		m_Filter.Draw("Filter", 180);
		ImGui::Separator();

		if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
		{
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::Selectable("Clear"))
				{
					ClearLog();
				}


				ImGui::EndPopup();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

			if (CopyToClipboard)
			{
				ImGui::LogToClipboard();
			}
			
			for (int i = 0; i < m_Items.size(); i++)
			{
				const char* item = m_Items[i].Text.c_str();

				if (!m_Filter.PassFilter(item))
				{
					continue;
				}
				
				ImGui::PushStyleColor(ImGuiCol_Text, m_Items[i].Color);
				ImGui::TextUnformatted(item);
				ImGui::PopStyleColor();
			}

			if (CopyToClipboard)
			{
				ImGui::LogFinish();
			}

			if (m_ScrollToBottom || (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			{
				ImGui::SetScrollHereY(1.0f);
			}

			m_ScrollToBottom = false;

			ImGui::PopStyleVar();
		}

		ImGui::EndChild();

		ImGui::End();
	}
}