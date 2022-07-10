#include <SFML/Graphics.hpp>
#include <string>

#include "Constants.hpp"
#include "Vehicle.hpp"
#include "Grid.hpp"

class Simulator {
    public:
        Simulator();
        void startSimulation();

    private:
        void setTextnColor(sf::Color color);

        Grid              grid;
        sf::RenderWindow  window;
        sf::Text          displayMsg;
        sf::Font          font;
        const std::string redStr   = "Currently not Placing Goal";
        const std::string greenStr = "Placing Goal";
        bool              settingGoal  = false;
        bool              drawObstacle = false;
};  