#ifndef TEXT_GENERATOR_H
#define TEXT_GENERATOR_H

#include <iostream>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;

class text_generator
{
public:
    text_generator(string fontFile);
    bool checkThaiGlyphExist();
    Mat genText(wstring genText,int size,int color);
    virtual ~text_generator();
protected:
private:
    struct Font
    {
        FT_Face face;
        hb_font_t* hb_font;
    };
    FT_Library library;
    Font font;

    void LoadFont(const FT_Library& library,const string& path,const int& size, Font& font);
    void CalcSize(hb_glyph_info_t *glyph_infos,hb_glyph_position_t *glyph_positions,const unsigned int& glyph_count,const FT_Face& face,Rect& rect);
};

#endif // TEXT_GENERATOR_H
