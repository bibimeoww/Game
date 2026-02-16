#include <SFML/Graphics.hpp>
#include <iostream>
#include "Player.h" // <--- บรรทัดนี้สำคัญมาก มันจะไปดึงโค้ดจากไฟล์เมื่อกี้มาเอง

using namespace std;

int main() {
    // --- ส่วนรับค่า Input ---
    string inputName;
    int inputAge;
    int jobChoice;

    cout << "=== CREATE CHARACTER ===" << endl;
    cout << "Enter Name: ";
    cin >> inputName;
    cout << "Enter Age: ";
    cin >> inputAge;

    // สร้างตัวละคร
    Player p1(inputName, inputAge);

    // ให้เลือกอาชีพ
    cout << "\nSelect Class:" << endl;
    cout << "[1] Warrior (High HP, Balanced ATK)" << endl;
    cout << "[2] Mage (Low HP, High ATK)" << endl;
    cout << "Choose (1-2): ";
    cin >> jobChoice;

    p1.setJob(jobChoice);
    p1.showStatus(); 

    cout << "\nGame Starting... (Check the Window!)" << endl;

    // --- ส่วนสร้างหน้าต่าง SFML ---
    sf::RenderWindow window(sf::VideoMode({800, 600}), "RPG Game Project");
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

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) playerShape.move({0, -5});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) playerShape.move({0, 5});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) playerShape.move({-5, 0});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) playerShape.move({5, 0});

        window.clear(sf::Color::Black);
        window.draw(playerShape);
        window.display();
    }

    return 0;
}