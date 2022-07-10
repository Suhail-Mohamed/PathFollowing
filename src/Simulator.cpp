#include "Simulator.hpp"

Simulator::Simulator() :
    window (sf::VideoMode(cons::WIND_SIZE, cons::WIND_SIZE), "PathFinder", 
            sf::Style::Titlebar | sf::Style::Close)
{
    window.setFramerateLimit(60);

    if (!font.loadFromFile("C:\\Windows\\Fonts\\Comic.ttf"))
        throw "Failed to load Font";

    displayMsg.setFont(font);
    displayMsg.setCharacterSize(18);
    displayMsg.setPosition({500, 0});
    setTextnColor(sf::Color::Red);
}

void Simulator::startSimulation() {
    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            
            if (event.type == sf::Event::KeyPressed) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
                    grid.resetGrid(true);
                
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
                    settingGoal = !settingGoal;

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                    drawObstacle = !drawObstacle;
            }
            if (event.type == sf::Event::MouseButtonPressed)
                grid.addObstacleOrGoal(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)), 
                                    settingGoal);
            else if (drawObstacle && !settingGoal)
                grid.addObstacleOrGoal(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)), 
                                    settingGoal, drawObstacle);
        }

        if (!settingGoal || grid.goalSet) setTextnColor(sf::Color::Red);
        if (settingGoal && !grid.goalSet) setTextnColor(sf::Color::Green);

        window.clear();
        grid.draw(window);
        window.draw(displayMsg);
        window.display();
    }
}

/*****************************************************************************/
/*
    If Colour red set display message to Currently not placing goal in red colour
    if green set display message to placing goal in green colour
*/
void Simulator::setTextnColor(const sf::Color color) {
    displayMsg.setFillColor(color);
    displayMsg.setString((color == sf::Color::Green) ? greenStr : redStr);
}