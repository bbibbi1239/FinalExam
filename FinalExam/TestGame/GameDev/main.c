#include "ElfMath.h"
#include "Elf2D.h"
#include "ElfTime.h"
#include "ElfObject.h"

#define WIDTH 40
#define HEIGHT 24

void Initialize(GameObject_Line* obj, int objNum)
{
    // 삼각형의 중심점 설정
    obj[0].Position.x = WIDTH / 2;
    obj[0].Position.y = HEIGHT / 2;
    obj[0].Rotation = 0;

    // 정삼각형의 꼭짓점 설정
    float size = 5.0f; // 삼각형 크기
    float height = size * sqrt(3); // 정삼각형의 높이

    // 세 꼭짓점 설정 (정삼각형)
    // 아래 두 점
    obj[0].Line[0].x = -size;
    obj[0].Line[0].y = -height / 3;

    obj[0].Line[1].x = size;
    obj[0].Line[1].y = -height / 3;

    // 위의 한 점
    obj[0].Line[2].x = 0;
    obj[0].Line[2].y = height * 2 / 3;  // 중심에서 위쪽으로

    // 출력 심볼 설정
    obj[0].Symbol = "@@";

    // 크기 설정
    obj[0].Scale.x = 1;
    obj[0].Scale.y = 1;
}

// 0: 입력없음
// 99 : 게임 종료
int Input()
{
    // ESC 키를 눌렀는지 확인
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

}

void Elf2DDrawLine2(float x1, float y1, float x2, float y2, char* Buffer, int width, int height, const char* symbol)
{
    Elf2DDrawLine((int)(x1 + 0.5f), (int)(y1 + 0.5f),
        (int)(x2 + 0.5f), (int)(y2 + 0.5f),
        Buffer, width, height, symbol);
}

void Render(GameObject_Line* obj, int objNum, char* Buf, int width, int height)
{
    Vector3 points[3];  // 삼각형의 세 점을 저장할 배열
    Matrix3x3 world = identity_matrix();

    // 변환 행렬 설정
    Matrix3x3 scale = scale_matrix(obj[0].Scale.x, obj[0].Scale.y);
    Matrix3x3 rotation = rotation_matrix(obj[0].Rotation);
    Matrix3x3 translation = translation_matrix(obj[0].Position.x, obj[0].Position.y);

    // 행렬 곱셈으로 최종 변환 행렬 생성
    world = multiply_matrices(scale, world);
    world = multiply_matrices(rotation, world);
    world = multiply_matrices(translation, world);

    // 세 점의 좌표를 변환
    for (int i = 0; i < 3; i++) {
        Vector3 point = { obj[0].Line[i].x, obj[0].Line[i].y, 1 };
        points[i] = multiply_matrix_vector(world, point);
    }

    // 삼각형의 세 변을 그림
    Elf2DDrawLine2(points[0].x, points[0].y, points[1].x, points[1].y, Buf, width, height, obj[0].Symbol);
    Elf2DDrawLine2(points[1].x, points[1].y, points[2].x, points[2].y, Buf, width, height, obj[0].Symbol);
    Elf2DDrawLine2(points[2].x, points[2].y, points[0].x, points[0].y, Buf, width, height, obj[0].Symbol);
}



int main() {
    int fps = 30;  // 60fps에서 30fps로 변경
    double frameTime = 1000.0 / fps;  // 이제 한 프레임당 약 33.33ms

    char screenBuffer[(WIDTH * 2 + 1) * HEIGHT];
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

        if (gameEvent == 99)
        {
            isGameRunning = 0;
        }

        Update(LineObj, 3, gameEvent);

        Elf2DClearScreen(screenBuffer, screenWidth, screenHeight);

        Render(LineObj, 3, screenBuffer, screenWidth, screenHeight);

        Elf2DDrawBuffer(screenBuffer);

        ElfTimeCalculateDeltaTime();
        Elf2DSleep(frameTime - ElfTimeGetDeltaTime());
    }
    return 0;
}