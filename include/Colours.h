#pragma once

#define COLOUR(fg, bg) (fg | (bg << 4))
#define BG(c) (c >> 4)

#define BACKGROUND COLOUR(0, 0)

class Colours
{
    public:
        static const unsigned short Background = BACKGROUND;
        static const unsigned short StatusBar = COLOUR(0, 7);
        static const unsigned short FunctionButton = COLOUR(0, 6);
        static const unsigned short Scrollbar = COLOUR(7, 0);
        static const unsigned short HexViewOffsetNormal = COLOUR(11, BG(BACKGROUND));
        static const unsigned short HexViewByteNormal = COLOUR(2, BG(BACKGROUND));
        static const unsigned short HexViewHighlight = COLOUR(0, 14);
        static const unsigned short HexViewCharNormal = COLOUR(10, BG(BACKGROUND));
        static const unsigned short Shadow = COLOUR(1, 0);
};
