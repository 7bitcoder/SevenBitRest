#pragma once

#include <iostream>
#include <mutex>
#include <string_view>

#include "Common/ConsoleColor.hpp"
#include "Log/LogLevel.hpp"
#include "Log/LogProviderBase.hpp"

namespace sd
{
    class ConsoleLogProvider final : public LogProviderBase
    {
      private:
        std::mutex _m;

      public:
        ConsoleLogProvider(IConfiguration *config) : LogProviderBase(config, "Console") {}

        void logImpl(LogLevel level, std::string_view message) final
        {
            if (level > LogLevel::DebugLevel)
            {
                std::lock_guard<std::mutex> guard(_m);
                std::cout << getColorModifiers(level);

                std::cout << message.substr(0, 4);

                std::cout << ColorModifier{ConsoleColor::Default};

                std::cout << message.substr(4);
            }
            else
            {
                std::cout << message;
            }
        }

      private:
        std::pair<ColorModifier, ColorModifier> getColorModifiers(LogLevel level)
        {
            switch (level)
            {
            case LogLevel::InformationLevel:
                return {{ConsoleColor::FontGreen}, {ConsoleColor::Default}};
            case LogLevel::WarningLevel:
                return {{ConsoleColor::FontYellow}, {ConsoleColor::Default}};
            case LogLevel::ErrorLevel:
                return {{ConsoleColor::FontBlack}, {ConsoleColor::BackgroundRed}};
            case LogLevel::CriticalLevel:
            default:
                return {{ConsoleColor::FontWhite}, {ConsoleColor::BackgroundBrightRed}};
            }
        }
    };
} // namespace sd