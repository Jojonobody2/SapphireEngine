#pragma once

#pragma warning(disable : 4996)

#include "ImGuiDebugger.h"

#include <spdlog/sinks/base_sink.h>

#include <mutex>

namespace spdlog
{
	namespace sinks
	{
		template <typename Mutex>
		class DebuggerSink final : public base_sink<Mutex>
		{
		public:
			explicit DebuggerSink(Sapphire::Debugger* Debugger)
			{
				m_Debugger = Debugger;
			}

		protected:
			void sink_it_(const details::log_msg& msg) override
			{
				//format log message to specified format
				memory_buf_t formatted;
				base_sink<Mutex>::formatter_->format(msg, formatted);
				formatted.push_back('\0');

				switch (msg.level)
				{
					case spdlog::level::info:
					{
						m_Debugger->AddInfo(formatted.data());
						break;
					}

					case spdlog::level::warn:
					{
						m_Debugger->AddWarn(formatted.data());
						break;
					}

					case spdlog::level::err:
					{
						m_Debugger->AddError(formatted.data());
						break;
					}

					case spdlog::level::critical:
					{
						m_Debugger->AddCritical(formatted.data());
						break;
					}

					default: break;
				}
			}

			void flush_() override {}

		private:
			Sapphire::Debugger* m_Debugger;
		};

		using debugger_sink_mt = DebuggerSink<std::mutex>;
	}
}