#include <SFML/Graphics.hpp>
#include <time.h>

using namespace sf;

const int M = 20;
const int N = 10;

int field[M][N] = { 0 };

Vector2u spawn(4, -4);

struct Point
{
    int x, y;
} current[4], previous[4];

int figures[7][4] =
{
    1,3,5,7, // I
    2,4,5,7, // Z
    3,5,4,6, // S
    3,5,4,7, // T
    2,3,5,7, // L
    3,5,7,6, // J
    2,3,4,5, // O
};

bool check()
{
    for (int i = 0;i < 4;i++)
        if (current[i].x < 0 || current[i].x >= N || current[i].y >= M) return 0;
        else if (field[current[i].y][current[i].x]) return 0;
        //else if (a[i].y < 0) return 1;

    return 1;
};


int main()
{
    srand(time(0));

    RenderWindow window(VideoMode(522,442), "The Game!");
    //window.setSize(Vector2u(360, 720));
    Texture t1, t2, t3;
    t1.loadFromFile("img/titles2.png");
    t2.loadFromFile("img/new-background(522x442).png");

    Sprite s(t1), background(t2);

    int dx = 0; bool rotate = 0; int colorNum = 1;
    float timer = 0, delay = 0.3;

    Clock clock;

    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed)
                if (e.key.code == Keyboard::Up) rotate = true;
                else if (e.key.code == Keyboard::Left) dx = -1;
                else if (e.key.code == Keyboard::Right) dx = 1;
        }

        if (Keyboard::isKeyPressed(Keyboard::Down)) delay = 0.06;

        //// <- Move -> ///
        for (int i = 0;i < 4;i++) { previous[i] = current[i]; current[i].x += dx; }
        if (!check()) for (int i = 0;i < 4;i++) current[i] = previous[i];

        //////Rotate//////
        if (rotate)
        {
            Point p = current[1]; //center of rotation
            for (int i = 0;i < 4;i++)
            {
                int x = current[i].y - p.y;
                int y = current[i].x - p.x;
                current[i].x = p.x - x;
                current[i].y = p.y + y;
            }
            if (!check()) for (int i = 0;i < 4;i++) current[i] = previous[i];
        }

        ///////Tick//////
        if (timer > delay)
        {
            for (int i = 0;i < 4;i++) { previous[i] = current[i]; current[i].y += 1; }

            if (!check())
            {
                for (int i = 0;i < 4;i++) field[previous[i].y][previous[i].x] = colorNum;

                colorNum = 1 + rand() % 5;
                int n = rand() % 7;
                for (int i = 0;i < 4;i++)
                {
                    current[i].x = figures[n][i] % 2 + spawn.x;
                    current[i].y = figures[n][i] / 2 + spawn.y;
                }
            }

            timer = 0;
        }

        ///////check lines//////////
        int k = M - 1;
        for (int i = M - 1;i > 0;i--)
        {
            int count = 0;
            for (int j = 0;j < N;j++)
            {
                if (field[i][j]) count++;
                field[k][j] = field[i][j];
            }
            if (count < N) k--;
        }

        dx = 0; rotate = 0; delay = 0.3;

        /////////draw//////////
        window.clear(Color::White);
        window.draw(background);
        
        for (int i = 0;i < M;i++)
            for (int j = 0;j < N;j++)
            {
                if (field[i][j] == 0) continue;
                s.setTextureRect(IntRect(field[i][j] * 20, 0, 20, 20));
                s.setPosition(j * 20, i * 20);
                s.move(160, 20); //offset
                window.draw(s);
            }

        for (int i = 0;i < 4;i++)
        {
            s.setTextureRect(IntRect(colorNum * 20, 0, 20, 20));
            s.setPosition(current[i].x * 20, current[i].y * 20);
            s.move(160, 20); //offset
            if (s.getPosition().y > 0)
            {
                window.draw(s);
            }
            
        }

        //window.draw(frame);
        window.display();
    }

    return 0;
}
