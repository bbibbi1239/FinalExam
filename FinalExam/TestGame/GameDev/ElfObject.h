#ifndef ELFOBJECT_H
#define ELFOBJECT_H
#include "ElfMath.h"

//게임 오브젝트
typedef struct
{
    //Transform
    Vector2 Position;
    Vector2 Scale;
    float Rotation;
    //Geometry
    Vector2 Line[3];  // 배열 크기를 3으로 수정
    //픽셀채울 글자
    const char* Symbol;  // const 추가
    //현재 라인 개수
    int LineCount;    // 라인 카운트 추가
} GameObject_Line;

//점 오브젝트
typedef struct
{
    //Transform
    Vector2 Position;
    Vector2 Scale;
    float Rotation;
    //단일 점의 위치
    Vector2 Point;
    //픽셀채울 글자
    const char* Symbol;
} GameObject_Dot;

#endif