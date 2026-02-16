#include <SFML/Graphics.hpp>
#include <iostream>
#include "Player.h"

using namespace std;

class Player {
public:
    string name;
    int age;
    Player(string n, int a) : name(n), age(a) {}
    void showStatus() {
        cout << "Name: " << name << " | Age: " << age << endl;
    }
};

int main() {
    string inputName;
    int inputAge;

    cout << "=== CREATE CHARACTER ===" << endl;
    cout << "Enter Name: ";
    cin >> inputName;
    cout << "Enter Age: ";
    cin >> inputAge;

    Player p1(inputName, inputAge);
    p1.showStatus();

    sf::RenderWindow window(sf::VideoMode({800, 600}), "Dungeon of Fate");
    window.setFramerateLimit(60);

    sf::RectangleShape playerShape(sf::Vector2f(50.0f, 50.0f));
    playerShape.setFillColor(sf::Color::Green);
    playerShape.setPosition({375.0f, 275.0f});

    while (window.isOpen()) {
        
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // --- การควบคุม (Keyboard) ---
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) playerShape.move({0, -5});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) playerShape.move({0, 5});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) playerShape.move({-5, 0});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) playerShape.move({5, 0});

        // --- วาดภาพ ---
        window.clear(sf::Color::Black);
        window.draw(playerShape);
        window.display();
    }

    return 0;
}
