#ifndef ELFOBJECT_H
#define ELFOBJECT_H

#include "ElfMath.h"

//���� ������Ʈ
typedef struct
{
    //Transform
    Vector2 Position;
    Vector2 Scale;
    float Rotation;

    //Geometry
    Vector2 Line[3];  // �迭 ũ�⸦ 3���� ����

    //�ȼ�ä�� ����
    const char* Symbol;  // const �߰�

    //���� ���� ����
    int LineCount;    // ���� ī��Ʈ �߰�

} GameObject_Line;

#endif