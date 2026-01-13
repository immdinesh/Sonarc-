#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <string>
#include "SerialPort.h"

#define PI 3.14159265

int main() {
    sf::RenderWindow window(sf::VideoMode(1366, 768), "Sonar Radar - C++");

    SerialPort arduino("COM5");   // change if needed
    char buffer[256];
    std::string data = "";

    int iAngle = 0;
    int iDistance = 0;

    sf::Vector2f center(683, 720);
    float maxRadius = 600;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // -------- SERIAL READ --------
        if (arduino.isConnected()) {
            int bytesRead = arduino.readData(buffer, 255);
            if (bytesRead > 0) {
                buffer[bytesRead] = '\0';
                data += buffer;

                if (data.find('.') != std::string::npos) {
                    int comma = data.find(',');
                    iAngle = std::stoi(data.substr(0, comma));
                    iDistance = std::stoi(data.substr(comma + 1));
                    data.clear();
                }
            }
        }

        window.clear(sf::Color::Black);

        // -------- RADAR CIRCLES --------
        for (int i = 1; i <= 4; i++) {
            sf::CircleShape circle(maxRadius * i / 4);
            circle.setFillColor(sf::Color::Transparent);
            circle.setOutlineThickness(2);
            circle.setOutlineColor(sf::Color::Green);
            circle.setOrigin(circle.getRadius(), circle.getRadius());
            circle.setPosition(center);
            window.draw(circle);
        }

        // -------- RADAR LINES --------
        for (int i = 30; i <= 150; i += 30) {
            float rad = i * PI / 180;
            sf::Vertex line[] = {
                sf::Vertex(center, sf::Color::Green),
                sf::Vertex(sf::Vector2f(center.x - maxRadius * cos(rad),
                                         center.y - maxRadius * sin(rad)), sf::Color::Green)
            };
            window.draw(line, 2, sf::Lines);
        }

        // -------- SCAN LINE --------
        float scanRad = iAngle * PI / 180;
        sf::Vertex scanLine[] = {
            sf::Vertex(center, sf::Color::Green),
            sf::Vertex(sf::Vector2f(center.x + maxRadius * cos(scanRad),
                                     center.y - maxRadius * sin(scanRad)), sf::Color::Green)
        };
        window.draw(scanLine, 2, sf::Lines);

        // -------- OBJECT --------
        if (iDistance < 40) {
            sf::CircleShape point(6);
            point.setFillColor(sf::Color::Red);

            float objX = center.x + (iDistance * 10) * cos(scanRad);
            float objY = center.y - (iDistance * 10) * sin(scanRad);

            point.setPosition(objX, objY);
            window.draw(point);
        }

        window.display();
    }

    return 0;
}
