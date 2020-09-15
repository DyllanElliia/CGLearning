#ifndef PLYH
#define PLYH

#include "vec3.h"
#include "polygon.h"
#include "hitable.h"
#include "hitable_list.h"
#include <fstream>
#include <cstdio>
#include <stack>
#include <cstring>

#define EXCITING_BEGIN {
#define EXCITING_END   }

inline void compare(char data1[], char data2[], int size)
{
    if (strncmp(data1, data2,size) != 0)
    {
        cout << "false!\n"
             << data1 << endl;
        system("pause");
        exit(EXIT_FAILURE);
    }
}

inline void haveThisString(char data1[], char data2[], int size)
{
    for(int i = 0; i < size; i++)
        if(data1[i]!=data2[i])
        {
            cout << "false!\n"
                 << data1 << endl;
            system("pause");
            exit(EXIT_FAILURE);
        }
}

inline bool ifHaveThisString(char data1[], char data2[], int size)
{
    for (int i = 0; i < size; i++)
        if (data1[i] != data2[i])
        {
            return false;
        }
    return true;
}

inline int readInt(ifstream &file, char data[], int size)
{
    char ch;
    for(int i =0 ; i < size; i++)
    {
        file >>ch;
        if(ch != data[i])
        {
            cout << "false!\n"
                 << ch << endl;
            system("pause");
            exit(EXIT_FAILURE);
        }
    }
    int number;
    file >> number;
    return number;
}

inline char readChar(ifstream &file, char data[], int size)
{
    char ch;
    for (int i = 0; i < size; i++)
    {
        file >> ch;
        if (ch != data[i])
        {
            cout << "false!\n"
                 << ch << endl;
            system("pause");
            exit(EXIT_FAILURE);
        }
    }
    char *number;
    file >> number;
    return number[0];
}

inline char readCh(ifstream &file, char data[], int size)
{
    char ch;
    for (int i = 0; i < size; i++)
    {
        file >> ch;
        if (ch != data[i])
            return '\0';
    }
    char *number;
    file >> number;
    return number[0];
}

void readRank_(ifstream &file, char *rank, int &numebr)
{
    queue <char> ch;
    char c = ' ';
    numebr = 0;
    //while()
}

class ply_file
{
    private:
        string file_name;
        string version;
        int element_vertex;
        char *readRank;
        int numberReadRank;
        int element_face;
        vec3 *vertexList;
        vec3 *faceList;
    public:
        ply_file() {}
        ply_file(string fileName);
        int sizeFace(){return element_face;}
        vec3 getAVertex(int number,int i)
            const
        {
            switch (i)
            {
            case 0:
                return vertexList[int(faceList[number].x())];
                break;
            case 1:
                return vertexList[int(faceList[number].y())];
                break;
            case 2:
                return vertexList[int(faceList[number].y())];
                break;
            default:
                return vec3();
                break;
            }
        }
        vec3 *getThreeVertex(int number)
            const
        {
            vec3 *threeVertex = new vec3[3];
            threeVertex[0] = vertexList[int(faceList[number].x())];
            threeVertex[1] = vertexList[int(faceList[number].y())];
            threeVertex[2] = vertexList[int(faceList[number].z())];
            return threeVertex;
        }
        void getThreeVertex(vec3 (&threeVertex)[3], int number)
            const
        {
            threeVertex[0] = vertexList[int(faceList[number].x())];
            threeVertex[1] = vertexList[int(faceList[number].y())];
            threeVertex[2] = vertexList[int(faceList[number].z())];
        }
        inline int returnElementVertex()    { return element_vertex;}
        inline int returnElementFace()      { return element_face;}
        inline int returnNumberReadRank()   { return numberReadRank;}
};

ply_file::ply_file(string fileName)
{
    file_name = fileName;
    ifstream ply_file_(file_name, ios::in);
    ply_file_.seekg(ios::beg);
    char data[100];
    ply_file_>>data;
    compare(data,"ply",4);
    ply_file_>>data;
    haveThisString(data, "format",7);
    do{
        version += data+' ';
        ply_file_>>data;
        //cout << data;
    }while(!ifHaveThisString(data,"comment",7));
    do{
        ply_file_ >> data;
        //cout <<data;
    }while(!ifHaveThisString(data,"element",7));
    element_vertex = readInt(ply_file_, "vertex", sizeof("vertex")/sizeof(char)-1);
    stack <char> ch;
    ply_file_ >> data;
    while(!ifHaveThisString(data,"comment",7)&&!ifHaveThisString(data,"element",7))
    {
        char in_queue;
        ply_file_>>data;
        ply_file_>>in_queue;
        ch.push(in_queue);
        ply_file_>>data;
    }
    while(!ifHaveThisString(data,"element",7))
    {
        if(!strncmp(data,"float",6))
        {
            ply_file_ >> data;
            ch.push(' ');
        }else
            ply_file_ >> data;
        //cout <<data;
    }
    numberReadRank = ch.size();
    readRank = new char[numberReadRank];
    for(int i = ch.size(); i>0; i--)
    {
        readRank[i-1] = ch.top();
        ch.pop();
    }
    /*for(int i = 0; i < sizeof(readRank)/sizeof(readRank[0]);i++)
    {
        if(readRank[i]==' ')
        {    cout << "i"<<endl;continue;}
        cout << readRank[i]<<endl;
    }*/
    element_face = readInt(ply_file_, "face", sizeof("face") / sizeof(char) - 1);
    do{
        ply_file_ >> data;
        //cout <<data;
    }while(!ifHaveThisString(data,"end",3));
    EXCITING_BEGIN
    int count = numberReadRank;
    float get_number;
    vertexList = new vec3[element_vertex];
    for(int i = 0; i < element_vertex;i++)
        for(int j = 0; j < count; j++)
        {
            ply_file_>>get_number;
            switch (readRank[j])
            {
            case 'x':
                vertexList[i][0] = get_number;
                break;
            case 'y':
                vertexList[i][1] = get_number;
                break;
            case 'z':
                vertexList[i][2] = get_number;
                break;
            default:
                break;
            }
        }
    EXCITING_END
    faceList = new vec3[element_face];
    for(int i = 0; i < element_face; i++)
    {
        int get_number;
        ply_file_>> get_number;
        if(get_number==3)
            for(int j = 0; j < get_number; j++)
                ply_file_>>faceList[i][j];
        else{
            cout <<"not 3"<<endl;
            faceList[i][0] = faceList[i][1] = faceList[i][2] = 0;
            float get_;
            for(int j = 0; j < get_number; j++)
                ply_file_>>get_;
        }
    }
    cout<<vertexList[element_vertex-1]<<endl;
    cout<<faceList[element_face-1]<<endl;
}

#endif