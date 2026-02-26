#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include "Player.h" // ดึงข้อมูล Class Player มาใช้

using namespace std;

int main() {
    // สร้างหน้าต่างเกม
    sf::RenderWindow window(sf::VideoMode({800, 600}), "My Epic RPG");
    window.setFramerateLimit(60);

    // 1. โหลดฟอนต์ (ใช้ฟอนต์ Arial ที่มีในเครื่อง Mac)
    sf::Font font;
    if (!font.openFromFile("/System/Library/Fonts/Supplemental/Arial.ttf")) {
        if (!font.openFromFile("/Library/Fonts/Arial.ttf")) {
            cout << "Error: Font not found!" << endl;
            return -1;
        }
    }

    // 2. เตรียม UI สำหรับหน้าเมนู (State 0)
    sf::Text titleText(font, "MY RPG GAME", 50);
    titleText.setPosition({220.f, 100.f});
    titleText.setFillColor(sf::Color::Yellow);

    sf::Text namePrompt(font, "Enter Name: ", 30);
    namePrompt.setPosition({200.f, 250.f});
    
    string playerName = ""; // เก็บชื่อที่พิมพ์
    sf::Text nameInputText(font, playerName, 30);
    nameInputText.setPosition({400.f, 250.f});
    nameInputText.setFillColor(sf::Color::Cyan);

    sf::Text startPrompt(font, "Press ENTER to Start", 20);
    startPrompt.setPosition({280.f, 400.f});

    // 3. เตรียมตัวละครสำหรับหน้าเล่นเกม (State 1)
    sf::RectangleShape playerShape(sf::Vector2f(50.0f, 50.0f));
    playerShape.setFillColor(sf::Color::Green);
    playerShape.setPosition({375.0f, 275.0f});

    // *** ตัวแปรกำหนดหน้าจอ ***
    int gameState = 0; // 0 = หน้าใส่ชื่อ, 1 = หน้าเล่นเกม

    // ================= GAME LOOP =================
    while (window.isOpen()) {
        
        // --- 1. จัดการ Event (การกดปุ่ม/พิมพ์) ---
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // ถ้าอยู่หน้าเมนู (State 0) ให้รับค่าการพิมพ์
            if (gameState == 0) {
                if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                    if (textEvent->unicode == 8 && !playerName.empty()) { 
                        playerName.pop_back(); // กด Backspace ลบตัวอักษร
                    } 
                    else if (textEvent->unicode < 128 && textEvent->unicode != 8 && textEvent->unicode != 13) {
                        playerName += static_cast<char>(textEvent->unicode); // พิมพ์ตัวอักษรเพิ่ม
                    }
                    nameInputText.setString(playerName); // อัปเดตข้อความบนจอ
                }

                // กด Enter เพื่อเข้าเกม
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter) && !playerName.empty()) {
                    gameState = 1; // เปลี่ยนหน้าจอเป็น State 1
                    cout << "Welcome to the game, " << playerName << "!" << endl;
                }
            }
        }

        // --- 2. อัปเดตเกมเพลย์ (เฉพาะตอนอยู่หน้าเล่นเกม State 1) ---
        if (gameState == 1) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) playerShape.move({0, -5});
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) playerShape.move({0, 5});
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) playerShape.move({-5, 0});
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) playerShape.move({5, 0});
        }

        // --- 3. วาดภาพลงจอ (Draw) ---
        window.clear(sf::Color::Black);
        
        if (gameState == 0) {
            // วาดหน้าเมนู
            window.draw(titleText);
            window.draw(namePrompt);
            window.draw(nameInputText);
            window.draw(startPrompt);
        } 
        else if (gameState == 1) {
            // วาดหน้าเล่นเกม (กล่องเขียว)
            window.draw(playerShape);
        }
        
        window.display();
    }

    return 0;
}