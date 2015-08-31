#include "text_generator.h"
#include <iostream>
#include <vector>
#include <dirent.h>
#include <opencv2/opencv.hpp>
#include <sys/stat.h>
#include <sys/types.h>
#include <omp.h>
using namespace std;
using namespace cv;
RNG rng(12345);
vector<int> baseCharUnicode;
vector<string> fontFilesList;
void baseCharList()
{
    //for(int i=30; i<=39; i++)
    //    baseCharUnicode.push_back(i);
    baseCharUnicode.push_back(3585);
    baseCharUnicode.push_back(3586);
    baseCharUnicode.push_back(3588);
    for(int i=3590; i<=3632; i++)
        baseCharUnicode.push_back(i);
    baseCharUnicode.push_back(3634);
    for(int i=3650; i<=3652; i++)
        baseCharUnicode.push_back(i);
    baseCharUnicode.push_back(3654);
}
void readFontList(string fontDir)
{
    struct dirent *de=NULL;
    DIR *d = opendir(fontDir.c_str());
    int len;
    while(de = readdir(d))
    {
        len = strlen (de->d_name);
        if (len >= 4)
        {
            if (strcmp (".ttf", &(de->d_name[len - 4])) == 0)
            {
                fontFilesList.push_back(de->d_name);
            }
        }
    }
}
int main()
{
    string fontDir = "/home/thananop/Works/Font/";
    string outputDir = "/home/thananop/temp/";
    int noPerClass = 50000;
    baseCharList();

    readFontList(fontDir);
    string currentFile;
/*
    vector<text_generator> generater;
    for(int i=0; i<fontFilesList.size(); i++)
    {
        currentFile = fontDir + fontFilesList[i];
        text_generator a = text_generator(currentFile);
        if(a.checkThaiGlyphExist())
        {
            generater.push_back(a);
        }
    }
*/


    for(int charNo = 0 ; charNo<baseCharUnicode.size(); charNo++)
    {   mkdir((outputDir+to_string(charNo)).c_str(), 0777);
        cout<<"charNo:"<<charNo<<endl;
        #pragma omp parallel for
        for(int genNo = 0 ; genNo<noPerClass ; genNo++)
        {
            wstring genText;
            int numChar = rng.uniform(0,3);
            if(numChar==0){
                genText.push_back(baseCharUnicode[charNo]);
            }
            else{
                genText.push_back(baseCharUnicode[rng.uniform(0,baseCharUnicode.size())]);
                genText.push_back(baseCharUnicode[charNo]);
                genText.push_back(baseCharUnicode[rng.uniform(0,baseCharUnicode.size())]);
            }
            int fontNo = rng.uniform(0,fontFilesList.size());
             //cout<<"GenNo:"<<genNo<<fontFilesList[fontNo]<<endl;
            text_generator a = text_generator(fontDir + fontFilesList[fontNo]);
           imwrite(outputDir+to_string(charNo)+"//"+to_string(genNo)+".jpg",a.genText(genText,50,rng.uniform(0,256)));
        }

    }
    //cout<<currentFile<<endl;
    //Mat output = a.genText(L"ทมน",40,rng.uniform(100,256));
    //imwrite("/home/thananop/temp/"+to_string(i)+"_"+fontFilesList[i]+".jpg",output);

}
