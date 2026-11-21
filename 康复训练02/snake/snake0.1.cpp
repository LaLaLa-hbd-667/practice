#include <iostream>
#include <conio.h>      // _kbhit, _getch
#include <windows.h>    // SetConsoleCursorPosition, GetStdHandle, Sleep
#include <vector>
#include <deque>
#include <cstdlib>
#include <ctime>

using namespace std;

const int WIDTH = 40;      // 游戏区域宽度（列数）
const int HEIGHT = 10;     // 游戏区域高度（行数）
const int INIT_LENGTH = 4; // 初始蛇长

// 坐标结构
struct Point {
    int x, y;
    Point(int _x = 0, int _y = 0) : x(_x), y(_y) {}
};

class SnakeGame {
private:
    deque<Point> snake;        // 蛇身，头部在 front
    Point food;                // 食物位置
    int direction;             // 0上 1右 2下 3左
    int score;
    bool gameOver;
    HANDLE hConsole;

public:
    SnakeGame() {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        srand(static_cast<unsigned>(time(nullptr)));
        initGame();
    }

    void initGame() {
        // 初始蛇：水平放置，头部在右
        snake.clear();
        for (int i = 0; i < INIT_LENGTH; ++i) {
            snake.push_back(Point(INIT_LENGTH - 1 - i, HEIGHT / 2));
        }
        direction = 1; // 向右
        score = 0;
        gameOver = false;
        generateFood();
        drawBorder();
        updateScore();
    }

    // 生成食物（不在蛇身上）
    void generateFood() {
        do {
            food.x = rand() % (WIDTH - 2) + 1;
            food.y = rand() % (HEIGHT - 2) + 1;
        } while (isOnSnake(food));
    }

    bool isOnSnake(const Point& p) {
        for (auto& seg : snake) {
            if (seg.x == p.x && seg.y == p.y) return true;
        }
        return false;
    }

    // 绘制边框
    void drawBorder() {
        system("cls"); // 清屏
        for (int i = 0; i < WIDTH + 2; ++i) cout << '#';
        cout << '\n';
        for (int y = 0; y < HEIGHT; ++y) {
            cout << '#';
            for (int x = 0; x < WIDTH; ++x) {
                cout << ' ';
            }
            cout << "#\n";
        }
        for (int i = 0; i < WIDTH + 2; ++i) cout << '#';
        cout << '\n';
        // 移动光标到边框内（分数区域）
        gotoxy(0, HEIGHT + 2);
        cout << "Score: " << score << "  (WASD/箭头移动)";
    }

    // 移动光标到指定位置（行, 列）
    void gotoxy(int x, int y) {
        COORD coord;
        coord.X = x;
        coord.Y = y;
        SetConsoleCursorPosition(hConsole, coord);
    }

    // 更新分数显示
    void updateScore() {
        gotoxy(0, HEIGHT + 2);
        cout << "Score: " << score;
    }

    // 绘制当前帧（蛇和食物）
    void draw() {
        
        // 绘制蛇身（从头到尾）
        for (int i = 0; i < snake.size(); ++i) {
            gotoxy(snake[i].x + 1, snake[i].y + 1);
            if (i == 0) cout << 'O';  // 蛇头
            else cout << 'o';
        }
        // 清除蛇尾（用空格覆盖）
        Point tail = snake.back();
        gotoxy(tail.x + 1, tail.y + 1);
        cout << ' ';

        // 绘制食物
        gotoxy(food.x + 1, food.y + 1);
        cout << '*';
    }

    // 处理键盘输入
    void input() {
        if (_kbhit()) {
            int key = _getch();
            if (key == 224) { // 方向键
                key = _getch();
                switch (key) {
                    case 72: if (direction != 2) direction = 0; break; // 上
                    case 77: if (direction != 3) direction = 1; break; // 右
                    case 80: if (direction != 0) direction = 2; break; // 下
                    case 75: if (direction != 1) direction = 3; break; // 左
                }
            } else { // WASD
                switch (tolower(key)) {
                    case 'w': if (direction != 2) direction = 0; break;
                    case 'd': if (direction != 3) direction = 1; break;
                    case 's': if (direction != 0) direction = 2; break;
                    case 'a': if (direction != 1) direction = 3; break;
                    case 'x': gameOver = true; break; // 按 x 退出
                }
            }
        }
    }

    // 移动蛇，返回是否吃到食物
    bool moveSnake() {
        Point head = snake.front();
        Point newHead = head;
        switch (direction) {
            case 0: newHead.y--; break;
            case 1: newHead.x++; break;
            case 2: newHead.y++; break;
            case 3: newHead.x--; break;
        }

        // 检测是否吃到食物
        bool ate = (newHead.x == food.x && newHead.y == food.y);

        // 插入新头
        snake.push_front(newHead);
        if (!ate) {
            snake.pop_back(); // 没吃到则移除尾
        }

        return ate;
    }

    // 检查碰撞
    bool checkCollision() {
        Point head = snake.front();
        // 撞墙
        if (head.x <= 0 || head.x >= WIDTH-1  || head.y+1 <= 0 || head.y >= HEIGHT )
            return true;
        // 撞自身（头部可能和身体其他部分重叠，跳过头部自身）
        for (int i = 1; i < snake.size(); ++i) {
            if (snake[i].x == head.x && snake[i].y == head.y)
                return true;
        }
        return false;
    }

    // 游戏主循环
    void run() {
        while (!gameOver) {
            input();
            bool ate = moveSnake();
            if (checkCollision()) {
                gameOver = true;
                break;
            }
            if (ate) {
                score += 10;
                updateScore();
                generateFood();
            }
            draw();
            Sleep(150); // 帧间隔（毫秒）
        }
        // 游戏结束信息
        gotoxy(0, HEIGHT + 4);
        cout << "Game Over! Final Score: " << score << "\nPress any key to exit...";
        _getch();
    }
};

int main() {
    SnakeGame game;
    game.run();
    return 0;
}