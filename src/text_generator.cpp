#include "text_generator.h"

text_generator::text_generator(string fontFile)
{
    //Freetype Initilization

    FT_Init_FreeType(&library);
    LoadFont(library,fontFile,32, font);
}

text_generator::~text_generator()
{

 //  FT_Done_Face(font.face);
 //  hb_font_destroy(font.hb_font);
}
bool text_generator::checkThaiGlyphExist()
{
    //Check kor-kai exist
    if(FT_Get_Char_Index(font.face,0x0E01)!=0)
    {
        return true;
    }

    return false;
}
void text_generator::LoadFont(const FT_Library& library,const string& path,const int& size, Font& font)
{
    FT_New_Face(library, path.c_str(), 0, &font.face);
    FT_Set_Char_Size(font.face, 0, size * 64,96,96);
    font.hb_font = hb_ft_font_create(font.face, 0);
}
void text_generator::CalcSize(hb_glyph_info_t *glyph_infos,hb_glyph_position_t *glyph_positions,const unsigned int& glyph_count,const FT_Face& face,Rect& rect)
{
    int width = 0;
    int above_base_line = 0;
    int below_base_line = 0;

    for (unsigned int i = 0; i < glyph_count; i++)
    {
        FT_Load_Glyph(face, glyph_infos[i].codepoint, FT_LOAD_DEFAULT);
        width += (glyph_positions[i].x_advance >> 6);
        int bearing = (face->glyph->metrics.horiBearingY >> 6) + (glyph_positions[i].y_offset >> 6);

        if (bearing > above_base_line)
            above_base_line = bearing;

        int height_minus_bearing = (face->glyph->metrics.height >> 6) - bearing;
        if (height_minus_bearing > below_base_line)
            below_base_line = height_minus_bearing;
    }
    rect.x = 0;
    rect.y = -above_base_line;
    rect.width = width;
    rect.height = above_base_line + below_base_line;
}

Mat text_generator::genText(wstring genText,int size,int color)
{

    FT_Set_Pixel_Sizes(font.face, 0, size);

    hb_buffer_t *buffer = hb_buffer_create();
    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
    hb_buffer_set_script(buffer, HB_SCRIPT_THAI);
    hb_buffer_add_utf32(buffer,(uint32_t*)(genText.c_str()),genText.length(),0,genText.length());
    hb_shape(font.hb_font, buffer, NULL, 0);


    unsigned int glyph_count = hb_buffer_get_length(buffer);
    hb_glyph_info_t *glyph_infos = hb_buffer_get_glyph_infos(buffer, NULL);
    hb_glyph_position_t *glyph_positions = hb_buffer_get_glyph_positions(buffer, NULL);

    Rect boundingRect;
    CalcSize(glyph_infos,glyph_positions,glyph_count,font.face,boundingRect);


    cv::Mat gray(boundingRect.height+200,boundingRect.width+200, CV_8UC1,cv::Scalar::all(0));

    int x = 0;
    int baseline = -boundingRect.y;
    int centerBegin,centerWidth;
    for (unsigned int i = 0; i < glyph_count; i++)
    {
        FT_Load_Glyph( font.face,glyph_infos[i].codepoint,FT_LOAD_RENDER | FT_LOAD_NO_HINTING);
        FT_Bitmap *bitmap = &(font.face->glyph->bitmap);
        cv::Mat glyph_img(bitmap->rows, bitmap->width, CV_8UC1, bitmap->buffer, bitmap->pitch);
        int top =  baseline - (font.face->glyph->metrics.horiBearingY >> 6) - (glyph_positions[i].y_offset >> 6)+2;
        if(top<0)
            top=0;
        int startXpos = x + font.face->glyph->bitmap_left;
        if(startXpos<0){
            startXpos = 0;

        }
        cv::Mat gray_part(gray, cv::Rect(startXpos, top, bitmap->width, bitmap->rows));

        glyph_img.copyTo(gray_part);

        if(i==1)
        {
            centerBegin = startXpos;
            centerWidth = bitmap->width;
        }

        x += (glyph_positions[i].x_advance >> 6);
    }
    int bg = 255 - color;
    // Merge. The grayscale image act as an alpha channel.
    //Mat b(gray.size(), CV_8UC1, cv::Scalar::all(bg));
   // Mat g(gray.size(), CV_8UC1, cv::Scalar::all(bg));
   // Mat r(gray.size(), CV_8UC1, cv::Scalar::all(bg));

    Mat alpha;
    Mat temp;
    gray.convertTo(temp,CV_32F);
    alpha = temp/255.0;
    //cout<<"Test"<<endl;
    Mat mask = alpha ==0;
    temp = Mat(gray.size(), CV_32F, cv::Scalar::all(bg));
    //cout<<"Test"<<endl;
    temp =temp.mul(alpha);
    //cout<<"Test"<<endl;
    temp.setTo(color,mask);
    temp.convertTo(gray,CV_8U);
    /*
    for (int i = 0; i < b.rows; i++)
    {
        for (int j = 0; j < b.cols; j++)
        {
            double alpha = (double)gray.at<uint8_t>(i, j) / 255;
            b.at<uint8_t>(i, j) *= alpha;
            g.at<uint8_t>(i, j) *= alpha;
            r.at<uint8_t>(i, j) *= alpha;

            if(alpha==0){
                b.at<uint8_t>(i, j) = color;
                g.at<uint8_t>(i, j) = color;
                r.at<uint8_t>(i, j) = color;
            }
        }
    }
    std::vector<cv::Mat> channels = {b, g, r};
    cv::Mat result;
    cv::merge(channels, gray);
    */
    hb_buffer_destroy(buffer);
    FT_Done_Face(font.face);
   hb_font_destroy(font.hb_font);
   FT_Done_FreeType(library);


    //Mat bgMask = (gray==0);
    //gray.setTo(100,bgMask);

    //Customize for My Work

    GaussianBlur(gray, gray, Size(3, 3), 2.5);
    //cvtColor(gray,gray,CV_BGR2GRAY);
    if(genText.length()>=3)
    {
        int startX = centerBegin -10;
        if(startX<0)
            startX=0;

        Mat cropped (gray,Rect(startX,0,startX+centerWidth+20,gray.rows-198));
        resize(cropped,cropped,Size(32,32));
        return cropped;
    }
    else  //only 1 char
    {
        Mat cropped (gray,Rect(0,0,gray.cols-200,gray.rows-195));
        resize(cropped,cropped,Size(32,32));
        return cropped;
    }


}
