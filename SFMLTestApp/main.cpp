#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>

#include "ScoreSaver.h"

using namespace sf;

const int M = 20;
const int N = 10;

sf::Font font;
sf::Text scoreLabel, currentScore, nextFigure;

int cIndx, nIndx, _score = 0;

int field[M][N] = { 0 };

Vector2u spawn(4, -4);
Vector2u nextSpawn(401, 121);
Vector2u ScoreLabelSpawn(401, 18);
Vector2u NextLabelSpawn(401, 98);
Vector2u ScoresSpawn(20, 0);

std::vector<sc::Score> _scores;
std::vector<Text> _scoresText;

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

void setScores() {

    int count = 0;
    for (auto score : _scores)
    {
        count++;
        sf::Text s;
        s.setFont(font);
        s.setString(std::to_string(count) + " - " + std::to_string(score.Value));
        s.setCharacterSize(20);
        if (count == 1) s.setFillColor(sf::Color(255, 236, 54));
        if (count == 2) s.setFillColor(sf::Color(188, 188, 188));
        if (count == 3) s.setFillColor(sf::Color(245, 129, 18));
        if (count  > 3) s.setFillColor(sf::Color::Black);
        s.move(ScoresSpawn.x, ScoresSpawn.y + count * 20);
        if (count > 20) break;
        _scoresText.push_back(s);
    }
}

void loadScores() {
    sv::ScoreSaver saver;
    _scores.clear();
    _scores = saver.loadScore();
    std::sort(_scores.begin(), _scores.end(), sc::sorter);
    setScores();
}

void saveScores() {
    sv::ScoreSaver saver;
    sc::Score s;
    std::string stringValue = currentScore.getString();

    std::cout << "score "<< atoi(stringValue.c_str()) << std::endl;
    s.Value = atoi(stringValue.c_str());
    _scores.push_back(s);

   saver.saveScore(_scores);
   std::cout << "save" << std::endl;
}



void setupFont()
{
    font.loadFromFile("fonts/SFSquareHead.ttf");
    // Score label
    nextFigure.setFont(font);
    nextFigure.setString("Next:");
    nextFigure.setCharacterSize(20);
    nextFigure.setFillColor(sf::Color::Black);
    nextFigure.move(NextLabelSpawn.x, NextLabelSpawn.y);


    // Score label
    scoreLabel.setFont(font);
    scoreLabel.setString("Score:");
    scoreLabel.setCharacterSize(20); 
    scoreLabel.setFillColor(sf::Color::Black);
    scoreLabel.move(ScoreLabelSpawn.x, ScoreLabelSpawn.y);

    // Score
    currentScore.setFont(font);
    currentScore.setString(std::to_string(_score));
    currentScore.setCharacterSize(18);
    currentScore.setFillColor(sf::Color::Black);
    currentScore.move(ScoreLabelSpawn.x, ScoreLabelSpawn.y + scoreLabel.getCharacterSize()+1);
}

void clearField() 
{
    std::cout << "clear field" << std::endl;
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
    bool isGameOver = false;
    for (int i = 0;i < 4;i++)
        if (current[i].y < 0)
        {
            std::cout << current[i].x << ":" << current[i].y << std::endl;
            isGameOver = true;
        }

    return isGameOver;
};

bool isInBounds() {
    for (int i = 0;i < 4;i++)
        if (current[i].x < 0 || current[i].x >= N || current[i].y >= M) {
            std::cout << "bounds" << std::endl; 
            return 0;
        }
    return 1;
}

bool isBlocked() {
    for (int i = 0;i < 4;i++)
       if (field[current[i].y][current[i].x]) {
                std::cout << "blocked" << std::endl; 
                return 1;
       }
    return 0;
}


bool check()
{
    for (int i = 0;i < 4;i++)
        if (current[i].x < 0 || current[i].x >= N || current[i].y >= M) {
            std::cout << "bounds/" << std::endl; return 0;
        }
        else if (field[current[i].y][current[i].x]) {
            std::cout << "blocked/" << std::endl;return 0;
        }

    return 1;
};

int main()
{
    srand(time(0));
    loadScores();
    setupFont();
    figureInit(current, cIndx);
    figureInit(next, nIndx);
    
    RenderWindow window(VideoMode(522,442), "TETRIS");
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
                    if (!isPlayable)
                    {
                        std::cout << "\nnew game" << std::endl;
                        isPlayable = true;
                        figureInit(current, cIndx);
                        figureInit(next, nIndx);
                        loadScores();
                    }
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
            if (!isInBounds() || isBlocked()) for (int i = 0;i < 4;i++) current[i] = previous[i];

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
                if (!isInBounds() || isBlocked()) for (int i = 0;i < 4;i++) current[i] = previous[i];
            }

            ///////Tick//////
            if (timer > delay)
            {
                for (int i = 0;i < 4;i++) { previous[i] = current[i]; current[i].y += 1; }

                if (isBlocked())
                {
                    if (isGameOver())
                    {
                        for (int i = 0;i < 4;i++) { current[i] = next[i] = {0}; }
                        std::cout <<"game over" << std::endl;
                        isPlayable = false;
                        clearField();
                        window.clear(Color::White);
                        window.draw(background);

                        saveScores();
                        loadScores();
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
                //std::cout << "score: " << _score << std::endl;
                currentScore.setString(std::to_string(_score));
                
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
        if (isPlayable) 
        {
            window.draw(scoreLabel);
            window.draw(currentScore);
            window.draw(nextFigure);
            for (auto score : _scoresText)
            {
                window.draw(score);
            }
            window.display();
        }
        else 
        {
            window.clear(Color::White);
            window.draw(background);
            window.draw(scoreLabel);
            window.draw(nextFigure);
            for (auto score : _scoresText)
            {
                window.draw(score);
            }
            window.display();
        }
    }
    return 0;
}