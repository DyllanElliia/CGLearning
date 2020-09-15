#ifndef VarH
#define VarH

#include <iostream>
#include <cstdio>
#include <string>
#include <math.h>

class var
{
    protected:
        #define MODE int
        #define intData 0x1
        #define floData 0x0
        #define chaData 0x2
    private:
        float saveFloat;
        int saveInt;
        char saveChar;
        MODE mode;
        var(float saveFloat,
        int saveInt,
        char saveChar,
        MODE mode)
        :   saveFloat(saveFloat),
            saveInt(saveInt),
            saveChar(saveChar),
            mode(mode) {}
        inline MODE dealMode(var &v1, var &v2)
        {
            int changeMode = v1.mode<v2.mode?v1.mode:v2.mode;
            switch (changeMode)
            {
            case intData:
                if(changeMode!=v1.mode)
                    switch (v1.mode)
                    {
                    case intData:
                        break;
                    case floData:
                        break;
                    case chaData:
                        v1.saveInt = int(v1.saveChar);
                        v1.mode = changeMode;
                        break;
                    default:
                        break;
                    }
                else
                    switch (v2.mode)
                    {
                    case intData:
                        break;
                    case floData:
                        break;
                    case chaData:
                        v2.saveInt = int(v2.saveChar);
                        v2.mode = changeMode;
                        break;
                    default:
                        break;
                    }
                break;
            case floData:
                if (changeMode != v1.mode)
                    switch (v1.mode)
                    {
                    case intData:
                        v1.saveFloat = float(v1.saveInt);
                        v1.mode = changeMode;
                        break;
                    case floData:
                        break;
                    case chaData:
                        v1.saveFloat = float(v1.saveChar);
                        v1.mode = changeMode;
                        break;
                    default:
                        break;
                    }
                else
                    switch (v2.mode)
                    {
                    case intData:
                        v2.saveFloat = float(v2.saveInt);
                        v2.mode = changeMode;
                        break;
                    case floData:
                        break;
                    case chaData:
                        v2.saveFloat = float(v2.saveChar);
                        v2.mode = changeMode;
                        break;
                    default:
                        break;
                    }
                break;
            case chaData:
                /* code */
                break;
            default:
                break;
            }
        }
    public:
        var() :saveInt(0),saveFloat(0),mode(intData) {}
        var(int data) : saveInt(data),saveFloat(float(saveInt)),mode(intData) {}
        var(float data) : saveInt(int(data)),saveFloat(float(data)),mode(floData) {}
        var(char data) : saveChar(data),mode(chaData) {}

        inline const var &operator+(){return *this;}
        inline var operator-(){return var(-saveFloat,-saveInt,-saveChar,mode);}

        inline var &operator+=(var &v2)
        {
            MODE saveMode = dealMode(*this,v2);
        }
        inline var &operator-=(var &v2);
        inline var &operator*=(var &v2);
        inline var &operator/=(var &v2);

        inline var &operator+=(const int &v2);
        inline var &operator-=(const int &v2);
        inline var &operator*=(const int &v2);
        inline var &operator/=(const int &v2);

        inline var &operator+=(const float &v2);
        inline var &operator-=(const float &v2);
        inline var &operator*=(const float &v2);
        inline var &operator/=(const float &v2);

        inline var &operator+=(const char &v2);
        inline var &operator-=(const char &v2);
        inline var &operator*=(const char &v2);
        inline var &operator/=(const char &v2);
};

#endif