#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <variant>
#include <random>
#include <fstream>
#include <string>

#include "include/Object.h"
#include "include/Pipe.h"
#include "include/Score.h"

unsigned int WIN_WIDTH = 600;
unsigned int WIN_HEIGHT = 720;

unsigned int SCR_WIDTH;
unsigned int SCR_HEIGHT;

void loadTexture(sf::Texture* texture, const char* filepath, bool smoothTex);
void loadFont(sf::Font* font);
void clamp(float* val, float min, float max);
void updateBird(Object &objects);
float randomFloat(float min, float max);
void pipeSetup(std::vector<Pipe>* pipes);

sf::Clock TimeClock;
sf::Clock scoreClock;
float currentTime;
float prevTime = 0;
float deltaTime;

sf::Vector2 speed(0.0f, 1.0f);
float gravity = 0;
float rotation = 0;
bool isJumping = false;

sf::Vector2<float> pipe_pos(600, 0);
float pipesXdistance = 400.0f;
float pipesYdistance = 750.0f;
float pipesSpeed = -270.0f;
int maxPipes = 6;
std::vector<Pipe> pipes = {  };
sf::Texture pipeTexture;

bool gamePaused = false;
bool afterPause = false;

int score = 0;
int highScore = 0;
float scoreTime = 0;
float timeAfterPause = 0;

int main()
{
    SCR_WIDTH = sf::VideoMode::getDesktopMode().width;
    SCR_HEIGHT = sf::VideoMode::getDesktopMode().height;
    sf::RenderWindow window(sf::VideoMode(WIN_WIDTH, WIN_HEIGHT), "Flappy Bird");
    window.setPosition(sf::Vector2((int)(SCR_WIDTH / 2 - WIN_WIDTH / 2), (int)(SCR_HEIGHT / 2 - WIN_HEIGHT / 2)));
    window.setVerticalSyncEnabled(true);

    sf::Texture birdTextrue, backgroundTexture, gameoverTexture;
    sf::Font fontFont;
    loadTexture(&birdTextrue, "res/images/flappy.png", false);
    loadTexture(&pipeTexture, "res/images/pipe-green.png", true);
    loadTexture(&backgroundTexture, "res/images/background.png", false);
    loadTexture(&gameoverTexture, "res/images/gameover.png", false);
    loadFont(&fontFont);

    Object bird(sf::Vector2(((float)WIN_WIDTH / 2), ((float)WIN_HEIGHT / 2 - 100.0f)), 0.0f, sf::Vector2(1.5f, 1.5f), &birdTextrue);
    Object background(sf::Vector2(((float)WIN_WIDTH / 2), ((float)WIN_HEIGHT / 2)), 0.0f, sf::Vector2(2.2f, 2.0f), &backgroundTexture);
    Object gameover(sf::Vector2((float)WIN_WIDTH / 2, (float)WIN_HEIGHT / 2), 0.0f, sf::Vector2(1.2f, 1.2f), &gameoverTexture);
    Score playerScore(fontFont, std::to_string(score), 50);
    Score playerHighScore(fontFont, "highscore: " + std::to_string(highScore), 50);
    playerHighScore.setPosition(sf::Vector2(160.0f, 450.0f));
    playerScore.setPosition(sf::Vector2((float)WIN_WIDTH / 2, (float)WIN_WIDTH / 2 - 200.0f));

    pipeSetup(&pipes);

    while (window.isOpen())
    {
        currentTime = TimeClock.getElapsedTime().asSeconds();
        deltaTime = currentTime - prevTime;
        prevTime = TimeClock.getElapsedTime().asSeconds();
        scoreTime += deltaTime;

        gravity += 15.0f * deltaTime;
        speed.x *= deltaTime; speed.y *= deltaTime;  
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
                window.close();

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Space)
                {
                    if (!isJumping)
                    {
                        gravity = -5.0f;
                    }

                    isJumping = true;
                }
            }

            if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Space)
                {
                    isJumping = false;
                }
            }
        }

        window.clear();

        if (!gamePaused)
        {
            for (size_t i = pipes.size(); i > 0; --i) 
            {
                size_t index = i - 1;

                pipes[index].move(sf::Vector2(pipesSpeed * deltaTime, 0.0f));
                if ((bird.getSprite().getGlobalBounds().intersects(pipes[index].getSprite().getGlobalBounds())) || bird.getSprite().getPosition().y + bird.getSprite().getGlobalBounds().height / 2 >= (float)WIN_HEIGHT || bird.getSprite().getPosition().y - bird.getSprite().getGlobalBounds().height / 2 <= 0.0f)
                {
                    gamePaused = true;
                }

                if ((bird.getSprite().getPosition().x + bird.getSprite().getGlobalBounds().width / 2 >= pipes[index].getSprite().getPosition().x + pipes[index].getSprite().getGlobalBounds().width / 2 - 5.0f && bird.getSprite().getPosition().x + bird.getSprite().getGlobalBounds().width / 2 <= pipes[index].getSprite().getPosition().x + pipes[index].getSprite().getGlobalBounds().width / 2) && index % 2 != 0 && scoreTime >= 0.5f)
                {
                    score++;
                    scoreTime = 0;
                }

                if (pipes[index].getSprite().getPosition().x < -100.0f) 
                {
                    if (index % 2 != 0)
                    {
                        pipes[index].setPosition(sf::Vector2(pipes[pipes.size() - 1].getSprite().getPosition().x + pipesXdistance, randomFloat(-100.0f, 100.0f)));
                    }
                    else
                    {
                        pipes[index].setPosition(sf::Vector2(pipes[pipes.size() - 1].getSprite().getPosition().x, pipes[pipes.size() - 1].getSprite().getPosition().y + pipesYdistance));
                    }

                    Pipe temp_pipe = pipes[index];
                    pipes.erase(pipes.begin() + index);
                    pipes.push_back(temp_pipe);
                }

            }

            window.draw(background.getSprite());

            for (Pipe &pipe : pipes)
            {
                window.draw(pipe.getSprite());
            }

            playerScore.setString(std::to_string(score));
            playerScore.draw(window);

            window.draw(bird.getSprite());

            updateBird(bird);
        }
        else
        {
            timeAfterPause += deltaTime;
            std::ifstream infile("res/dev/score.txt");
            if (infile.is_open())
            {
                std::string line;
                std::getline(infile, line);
                std::string strHighScore = std::to_string(highScore);
                strHighScore = line;
                highScore = std::stoi(strHighScore);
            }
            else
            {
                std::cerr << "Failed to open file\n";
            }

            if (score > highScore)
            {
                highScore = score;
                std::ofstream outfile("res/dev/score.txt", std::ios::trunc);
                if (outfile.is_open())
                {
                    outfile << std::to_string(highScore);
                    outfile.close();
                }
                else
                {
                    std::cerr << "Failed to open file\n";
                }
            }

            playerHighScore.setString("highscore: " + std::to_string(highScore));

            window.draw(gameover.getSprite());
            playerScore.setPosition(sf::Vector2(200.0f, 400.0f));
            playerScore.setString("score: " + std::to_string(score));
            playerScore.draw(window);
            playerHighScore.draw(window);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && timeAfterPause > 0.3f)
            {
                scoreTime = 0;
                score = 0;
                playerScore.setPosition(sf::Vector2((float)WIN_WIDTH / 2, (float)WIN_WIDTH / 2 - 200.0f));
                playerScore.setString(std::to_string(score));
                bird.setPosition(sf::Vector2(((float)WIN_WIDTH / 2), ((float)WIN_HEIGHT / 2)));
                pipes.clear();
                afterPause = true;
                pipeSetup(&pipes);
                timeAfterPause = 0.0f;
                gamePaused = false;
            }
        }

        window.display();
    }

    return 0;
}

void loadTexture(sf::Texture* texture, const char* filepath, bool smoothTex)
{       
    if (!texture->loadFromFile(filepath))
    {
        std::cerr << "Failed to load texture\n";
    }

    texture->setSmooth(smoothTex);
}

void loadFont(sf::Font* font)
{
    if (!font->loadFromFile("res/dev/font.ttf"))
    {
        std::cerr << "Failed to laod font\n";
    }
}

void clamp(float* val, float min, float max)
{
    if (*val < min)
        *val = min;
    if (*val > max)
        *val = max;
}

void updateBird(Object &bird)
{
    speed.y += gravity;
    bird.move(speed);
    rotation = gravity * 5.0f;
    clamp(&rotation, -60.0f, 60.0f);
    bird.rotate(rotation);
}

float randomFloat(float min, float max)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> dist(min, max);

    return dist(gen);
}

void pipeSetup(std::vector<Pipe>* pipes)
{
    for (int i = 0; i < maxPipes; i++)
    {
        float pipe_rotation = 0;

        if (i%2 == 0)
        {
            pipe_pos.x = ((i == 0 && afterPause) ? pipesXdistance + 300.0f : pipe_pos.x + pipesXdistance);
            afterPause = false;

            pipe_pos.y = randomFloat(600.0f, 900.0f);
        }
        else
        {
            pipe_rotation = 180;
            pipe_pos.y -= pipesYdistance;
        }

        pipes->push_back(Pipe(pipe_pos, pipe_rotation, sf::Vector2(1.0f, 1.0f), &pipeTexture));
    }
}
