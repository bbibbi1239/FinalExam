#include "Elf2D.h"
#include "ElfTime.h"
#include "ElfObject.h"
#include <conio.h>
#include <math.h>

#define WIDTH 40
#define HEIGHT 24
#define PI 3.14159265358979323846
void Initialize(GameObject_Line* obj, int objNum)
{
    // 삼각형의 중심점 설정
    obj[0].Position.x = WIDTH / 2;
    obj[0].Position.y = HEIGHT / 2;
    obj[0].Rotation = 0;

    // 정삼각형의 꼭짓점 설정
    float size = 5.0f;
    float height = size * sqrt(3);

    // 세 꼭짓점 설정 (정삼각형, 위쪽 방향)
    obj[0].Line[0].x = 0;        // 상단 꼭짓점
    obj[0].Line[0].y = -height * 2 / 3;

    obj[0].Line[1].x = -size;    // 좌하단 꼭짓점
    obj[0].Line[1].y = height / 3;

    obj[0].Line[2].x = size;     // 우하단 꼭짓점
    obj[0].Line[2].y = height / 3;

    // 출력 심볼 설정
    obj[0].Symbol = "@@";

    // 크기 설정
    obj[0].Scale.x = 1;
    obj[0].Scale.y = 1;
}
int Input()
{
    if (_kbhit()) {
        char ch = _getch();
        if (ch == 27) {  // ESC 키가 눌리면 게임 종료
            return 99;
        }
    }
    return 0;
}
void Update(GameObject_Line* obj, int objNum, int e)
{
    // 회전 애니메이션 추가
    obj[0].Rotation += 0.02f;
    if (obj[0].Rotation > 2 * PI) {
        obj[0].Rotation -= 2 * PI;
    }
}
void Elf2DDrawLine2(float x1, float y1, float x2, float y2, char* Buffer, int width, int height, const char* symbol)
{
    // float 좌표를 int로 변환 (반올림)
    int ix1 = (int)(x1 + 0.5f);
    int iy1 = (int)(y1 + 0.5f);
    int ix2 = (int)(x2 + 0.5f);
    int iy2 = (int)(y2 + 0.5f);
    // 원본 Elf2DDrawLine 함수 호출
    Elf2DDrawLine(ix1, iy1, ix2, iy2, Buffer, width, height, symbol);
}
void Render(GameObject_Line* obj, int objNum, char* Buf, int width, int height)
{
    // 월드 변환 행렬 초기화
    Matrix3x3 world = identity_matrix();
    // 변환 행렬 설정
    Matrix3x3 scaleMatrix = scale_matrix(obj[0].Scale.x, obj[0].Scale.y);
    Matrix3x3 rotationMatrix = rotation_matrix(obj[0].Rotation);
    Matrix3x3 translationMatrix = translation_matrix(obj[0].Position.x, obj[0].Position.y);
    // 행렬 곱셈으로 최종 변환 행렬 생성 (Scale -> Rotate -> Translate 순서)
    world = multiply_matrices(scaleMatrix, world);
    world = multiply_matrices(rotationMatrix, world);
    world = multiply_matrices(translationMatrix, world);
    // 삼각형의 세 점을 저장할 배열
    Vector3 points[3];
    // 각 점을 변환
    for (int i = 0; i < 3; i++) {
        Vector3 point = { obj[0].Line[i].x, obj[0].Line[i].y, 1 };
        points[i] = multiply_matrix_vector(world, point);
    }
    // 삼각형의 세 변을 그림
    for (int i = 0; i < 3; i++) {
        int next = (i + 1) % 3;
        Elf2DDrawLine2(
            points[i].x,
            points[i].y,
            points[next].x,
            points[next].y,
            Buf,
            width,
            height,
            obj[0].Symbol
        );
    }
}
int main() {
    int fps = 30;
    double frameTime = 1000.0 / fps;
    // 버퍼 크기 계산 (각 픽셀이 2개의 문자로 구성)
    char screenBuffer[WIDTH * 2 * HEIGHT + HEIGHT];  // 각 줄의 끝에 \n 문자를 위한 공간 포함
    int screenWidth = WIDTH;
    int screenHeight = HEIGHT;
    GameObject_Line LineObj[3];
    Initialize(LineObj, 3);
    Elf2DInitScreen();
    Elf2DClearScreen(screenBuffer, screenWidth, screenHeight);
    Elf2DDrawBuffer(screenBuffer);
    ElfTimeInitialize();
    int isGameRunning = 1;
    while (isGameRunning) {
        int gameEvent = Input();
        if (gameEvent == 99) {
            isGameRunning = 0;
        }
        Update(LineObj, 3, gameEvent);
        Elf2DClearScreen(screenBuffer, screenWidth, screenHeight);
        Render(LineObj, 3, screenBuffer, screenWidth, screenHeight);
        Elf2DDrawBuffer(screenBuffer);
        ElfTimeCalculateDeltaTime();
        double remainingTime = frameTime - ElfTimeGetDeltaTime();
        if (remainingTime > 0) {
            Elf2DSleep((int)remainingTime);
        }
    }
    return 0;
}