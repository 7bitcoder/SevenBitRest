

#include <ostream>

namespace sd
{
    enum ConsoleColor
    {
        Default = 0,
        FontBlack = 30,
        FontRed = 31,
        FontGreen = 32,
        FontYellow = 33,
        FontBlue = 34,
        FontMagenta = 35,
        FontCyan = 36,
        FontWhite = 37,

        BackgroundBlack = 40,
        BackgroundRed = 41,
        BackgroundGreen = 42,
        BackgroundYellow = 43,
        BackgroundBlue = 44,
        BackgroundMagenta = 45,
        BackgroundCyan = 46,
        BackgroundWhite = 47,
        BackgroundBrightRed = 101
    };

    class ColorModifier
    {
        ConsoleColor code;

      public:
        ColorModifier(ConsoleColor pCode) : code(pCode) {}

        friend std::ostream &operator<<(std::ostream &os, const ColorModifier &mod)
        {
            return os << "\033[" << mod.code << "m";
        }

        friend std::ostream &operator<<(std::ostream &os, const std::pair<ColorModifier, ColorModifier> &mods)
        {
            auto &[font, back] = mods;
            return os << "\033[" << back.code << ";" << font.code << "m";
        }
    };
} // namespace sd