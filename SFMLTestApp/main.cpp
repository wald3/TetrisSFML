#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>

using namespace sf;

const int M = 20;
const int N = 10;

sf::Font font;
sf::Text scoreLabel, score;

int cIndx, nIndx, _score = 0;

int field[M][N] = { 0 };

Vector2u spawn(4, -4);
Vector2u nextSpawn(401, 121);
Vector2u ScoreLabelSpawn(401, 18);

struct Point
{
    int x, y;
} current[4], previous[4], next[4];

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

void setupFont()
{
    font.loadFromFile("fonts/SevenSidedGames.ttf");

    // Score label
    scoreLabel.setFont(font);
    scoreLabel.setString("Score");
    scoreLabel.setCharacterSize(20); 
    scoreLabel.setFillColor(sf::Color::Black);
    scoreLabel.move(ScoreLabelSpawn.x, ScoreLabelSpawn.y);

    // Score
    score.setFont(font);
    score.setString(std::to_string(_score));
    score.setCharacterSize(18);
    score.setFillColor(sf::Color::Black);
    score.move(ScoreLabelSpawn.x, ScoreLabelSpawn.y + scoreLabel.getCharacterSize()+1);
}




void clearField() 
{
    for (int i = -4; i < 20; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            field[i][j] = 0;
        }
    }
}

void figureInit(Point p[], int &index) {
    int n = rand() % 7;
    index = n;
    for (int i = 0;i < 4;i++)
    {
        p[i].x = figures[n][i] % 2 + spawn.x;
        p[i].y = figures[n][i] / 2 + spawn.y;
    }
}

bool isGameOver()
{
    for (int i = 0;i < 4;i++)
        if (current[i].y < 0)
        {
            return true;
        }

    return false;
};

bool check()
{
    for (int i = 0;i < 4;i++)
        if (current[i].x < 0 || current[i].x >= N || current[i].y >= M) {
            std::cout << "bounds" << std::endl; return 0;
        }
        else if (field[current[i].y][current[i].x]) {
            std::cout << "blocked" << std::endl;return 0;
        }

    return 1;
};

int main()
{
    srand(time(0));
    setupFont();
    figureInit(current, cIndx);
    figureInit(next, nIndx);
    
    RenderWindow window(VideoMode(522,442), "The Game!");
    Texture t1, t2, t3;
    t1.loadFromFile("img/titles2.png");
    t2.loadFromFile("img/new-background(522x442).png");

    
        
    Sprite s(t1), background(t2);
    
    std::cout << s.getGlobalBounds().width << ":" << s.getGlobalBounds().height;

    int dx = 0, colorCurrent = 1, colorNext = 2;
    bool rotate = 0, isPlayable = true; 
    float timer = 0.0, delay = 0.3;

    Clock clock;

    while (window.isOpen())
    {
        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();

            if (e.type == Event::KeyPressed)
                if (e.key.code == Keyboard::Up) rotate = true;
                else if (e.key.code == Keyboard::Left) dx = -1;
                else if (e.key.code == Keyboard::Right) dx = 1;
                else if (e.key.code == Keyboard::Space) 
                {
                    std::cout << "\nnew game" << std::endl;
                    isPlayable = true;
                    figureInit(current, cIndx);
                    figureInit(next, nIndx);
                }
        }
        
        if (isPlayable)
        {
            //std::cout << "---game---" << std::endl;
            float time = clock.getElapsedTime().asSeconds();
            clock.restart();
            timer += time;
            

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
                    if (isGameOver())
                    {
                        //Point p = {0};
                        for (int i = 0;i < 4;i++) { current[i] = next[i] = {0}; }
                        std::cout <<"game over" << std::endl;
                        isPlayable = false;
                        clearField();
                        window.clear(Color::White);
                        window.draw(background);
                        //window.display();
                    }
                    else
                    {
                        for (int i = 0;i < 4;i++) field[previous[i].y][previous[i].x] = colorCurrent;
                        for (int i = 0;i < 4;i++) { current[i] = next[i]; }
                        colorCurrent = colorNext;

                        colorNext = 1 + rand() % 5;
                        int n = rand() % 7;
                        for (int i = 0;i < 4;i++)
                        {
                            next[i].x = figures[n][i] % 2 + spawn.x;
                            next[i].y = figures[n][i] / 2 + spawn.y;
                        }
                    }
                }

                timer = 0;
            }

            ///////check lines//////////
            int k = M - 1, lines = 0;
            for (int i = M - 1;i > 0;i--)
            {
                int count = 0;
                for (int j = 0;j < N;j++)
                {
                    if (field[i][j]) count++;
                    field[k][j] = field[i][j];
                }
                
                if (count < N) { k--; }
                else { lines++; }
            }

            if (lines) {
                std::cout << lines << std::endl;
                _score += (100 * lines) * lines;
                score.setString(std::to_string(_score));
                //std::cout << _score << std::endl;
            }

            
            

            dx = 0; rotate = 0; delay = 0.3;

            if (isPlayable) {
                /////////draw//////////
                window.clear(Color::White);
                window.draw(background);

                // field draw
                for (int i = 0;i < M;i++)
                    for (int j = 0;j < N;j++)
                    {
                        if (field[i][j] == 0) continue;
                        s.setTextureRect(IntRect(field[i][j] * 20, 0, 20, 20));
                        s.setPosition(j * 20, i * 20);
                        s.move(160, 20); //offset
                        window.draw(s);
                    }

                // next figure draw
                for (int i = 0;i < 4;i++)
                {
                    s.setTextureRect(IntRect(colorNext * 20, 0, 20, 20));
                    s.setPosition(next[i].x * 20, next[i].y * 20);
                    s.move(nextSpawn.x - (spawn.x * 20) + 20, nextSpawn.y - (spawn.y * 20) - 1); //offset
                    window.draw(s);
                }

                // current figure draw
                for (int i = 0;i < 4;i++)
                {
                    s.setTextureRect(IntRect(colorCurrent * 20, 0, 20, 20));
                    s.setPosition(current[i].x * 20, current[i].y * 20);
                    s.move(160, 20); //offset
                    if (s.getPosition().y > 0)
                    {
                        window.draw(s);
                    }
                }
            }
        }
        if (isPlayable) { window.draw(scoreLabel); window.draw(score); window.display(); }
        else {
            
            window.clear(Color::White);
            window.draw(background);
            window.display();
        }
        
        
    }
    return 0;
}
