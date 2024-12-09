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

    // 정삼각형의 크기 계산
    float size = 4.0f;  // 기본 크기를 좀 더 작게 조정

    // 정삼각형 꼭짓점 계산 (중심이 (0,0)인 정삼각형)
    // 상단 꼭짓점
    obj[0].Line[0].x = 0;
    obj[0].Line[0].y = -size;

    // 좌하단 꼭짓점
    obj[0].Line[1].x = -size;
    obj[0].Line[1].y = size;

    // 우하단 꼭짓점
    obj[0].Line[2].x = size;
    obj[0].Line[2].y = size;

    obj[0].Symbol = "@@";
    obj[0].Scale.x = 1;
    obj[0].Scale.y = 1;

    // 점도 같은 방식으로 설정
    obj[1].Position.x = WIDTH / 2;
    obj[1].Position.y = HEIGHT / 2;
    obj[1].Rotation = 0;

    // 점의 위치도 상단 꼭짓점과 동일하게
    obj[1].Line[0].x = 0;
    obj[1].Line[0].y = -size;
    obj[1].Line[1].x = 0;
    obj[1].Line[1].y = -size;

    obj[1].Symbol = "++";
    obj[1].Scale.x = 1;
    obj[1].Scale.y = 1;
}

int Input()
{
    if (_kbhit()) {
        char ch = _getch();
        switch (ch) {
        case 27:  // ESC 키
            return 99;
        case 'a':  // 좌회전
            return 1;
        case 'd':  // 우회전
            return 2;
        case 'w':  // 전진
            return 3;
        case 's':  // 후진
            return 4;
        default:
            return 0;
        }
    }
    return 0;
}
void Update(GameObject_Line* obj, int objNum, int e)
{
    // 키 입력에 따른 회전 (1도씩)
    if (e == 1) {  // 'a' 키: 좌회전
        obj[0].Rotation -= 1.0f;
        obj[1].Rotation = obj[0].Rotation;
    }
    else if (e == 2) {  // 'd' 키: 우회전
        obj[0].Rotation += 1.0f;
        obj[1].Rotation = obj[0].Rotation;
    }
    else if (e == 3 || e == 4) {  // 'w' 키: 전진, 's' 키: 후진
        // 현재 회전 각도를 라디안으로 변환
        float radians = obj[0].Rotation * (PI / 180.0f);

        // 이동 방향 계산 (삼각형의 상단 꼭지점 방향)
        float dx = sinf(radians) * 0.1f;  // x 방향 이동량
        float dy = -cosf(radians) * 0.1f; // y 방향 이동량 (y축이 아래로 증가하므로 음수)

        // 전진 또는 후진
        if (e == 3) {  // 전진
            obj[0].Position.x += dx;
            obj[0].Position.y += dy;
            obj[1].Position.x += dx;
            obj[1].Position.y += dy;
        }
        else {  // 후진
            obj[0].Position.x -= dx;
            obj[0].Position.y -= dy;
            obj[1].Position.x -= dx;
            obj[1].Position.y -= dy;
        }
    }

    // 회전각이 360도를 넘어가면 조정
    if (obj[0].Rotation < 0) {
        obj[0].Rotation += 360.0f;
        obj[1].Rotation = obj[0].Rotation;
    }
    else if (obj[0].Rotation >= 360.0f) {
        obj[0].Rotation -= 360.0f;
        obj[1].Rotation = obj[0].Rotation;
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
    // 각 오브젝트에 대해 반복
    for (int obj_idx = 0; obj_idx < 2; obj_idx++) {
        Matrix3x3 world = identity_matrix();
        Matrix3x3 scaleMatrix = scale_matrix(obj[obj_idx].Scale.x, obj[obj_idx].Scale.y);
        Matrix3x3 rotationMatrix = rotation_matrix(obj[obj_idx].Rotation);
        Matrix3x3 translationMatrix = translation_matrix(obj[obj_idx].Position.x, obj[obj_idx].Position.y);

        world = multiply_matrices(scaleMatrix, world);
        world = multiply_matrices(rotationMatrix, world);
        world = multiply_matrices(translationMatrix, world);

        // 점인 경우 (obj_idx == 1)
        if (obj_idx == 1) {
            Vector3 point = { obj[obj_idx].Line[0].x, obj[obj_idx].Line[0].y, 1 };
            Vector3 transformed = multiply_matrix_vector(world, point);

            Elf2DDrawLine2(
                transformed.x,
                transformed.y,
                transformed.x,
                transformed.y,
                Buf,
                width,
                height,
                obj[obj_idx].Symbol
            );
        }
        // 삼각형인 경우 (obj_idx == 0)
        else {
            Vector3 points[3];
            for (int i = 0; i < 3; i++) {
                Vector3 point = { obj[obj_idx].Line[i].x, obj[obj_idx].Line[i].y, 1 };
                points[i] = multiply_matrix_vector(world, point);
            }

            // 삼각형을 채우기 위한 스캔라인 알고리즘
            float minY = points[0].y;
            float maxY = points[0].y;
            for (int i = 1; i < 3; i++) {
                if (points[i].y < minY) minY = points[i].y;
                if (points[i].y > maxY) maxY = points[i].y;
            }

            // 각 y좌표에 대해 스캔라인 실행
            for (int y = (int)minY; y <= (int)maxY; y++) {
                float intersections[3];
                int intersectCount = 0;

                // 각 변에 대해 교차점 찾기
                for (int i = 0; i < 3; i++) {
                    int next = (i + 1) % 3;
                    float y1 = points[i].y;
                    float y2 = points[next].y;

                    if ((y1 <= y && y2 > y) || (y2 <= y && y1 > y)) {
                        float x1 = points[i].x;
                        float x2 = points[next].x;

                        if (y1 != y2) {
                            float t = (y - y1) / (y2 - y1);
                            intersections[intersectCount++] = x1 + t * (x2 - x1);
                        }
                    }
                }

                // 교차점 정렬
                if (intersectCount >= 2) {
                    if (intersections[0] > intersections[1]) {
                        float temp = intersections[0];
                        intersections[0] = intersections[1];
                        intersections[1] = temp;
                    }

                    // 선 그리기
                    for (float x = intersections[0]; x <= intersections[1]; x++) {
                        Elf2DDrawLine2(x, y, x, y, Buf, width, height, obj[obj_idx].Symbol);
                    }
                }
            }

            // 외곽선도 그리기
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
                    obj[obj_idx].Symbol
                );
            }
        }
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