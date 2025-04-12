#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

using namespace sf;

// Bat (paddle) class
class Bat {
private:
    Vector2f m_Position;
    RectangleShape m_Shape;
    float m_Speed = 1000.0f;
    bool m_MovingRight = false;
    bool m_MovingLeft = false;
    int m_Lives = 3;  // Player starts with 3 lives

public:
    Bat(float startX, float startY) {
        m_Position.x = startX;
        m_Position.y = startY;
        m_Shape.setSize(Vector2f(50, 5));
        m_Shape.setPosition(m_Position);
    }

    FloatRect getPosition() {
        return m_Shape.getGlobalBounds();
    }

    RectangleShape getShape() {
        return m_Shape;
    }

    void moveLeft() {
        m_MovingLeft = true;
    }

    void moveRight() {
        m_MovingRight = true;
    }

    void stopLeft() {
        m_MovingLeft = false;
    }

    void stopRight() {
        m_MovingRight = false;
    }

    void update(Time dt) {
        if (m_MovingLeft) {
            m_Position.x -= m_Speed * dt.asSeconds();
        }
        if (m_MovingRight) {
            m_Position.x += m_Speed * dt.asSeconds();
        }
        
        // Keep paddle within the window bounds
        if (m_Position.x < 0) {
            m_Position.x = 0;
        }
        if (m_Position.x > 1920 - m_Shape.getSize().x) {
            m_Position.x = 1920 - m_Shape.getSize().x;
        }
        
        m_Shape.setPosition(m_Position);
    }
    
    // Methods for lives management
    int getLives() {
        return m_Lives;
    }

    void decreaseLives() {
        if (m_Lives > 0) {
            m_Lives--;
        }
    }

    bool isAlive() {
        return m_Lives > 0;
    }
};

// Ball class
class Ball {
private:
    Vector2f m_Position;
    CircleShape m_Shape;
    float m_Speed = 400.0f;
    Vector2f m_Velocity;

public:
    Ball(float startX, float startY) {
        m_Position.x = startX;
        m_Position.y = startY;
        
        m_Shape.setRadius(10);
        m_Shape.setFillColor(Color::White);
        m_Shape.setOrigin(10, 10);
        m_Shape.setPosition(m_Position);
        
        // Initial velocity - 45-degree angle
        m_Velocity.x = m_Speed;
        m_Velocity.y = m_Speed;
    }

    FloatRect getPosition() {
        return m_Shape.getGlobalBounds();
    }

    CircleShape getShape() {
        return m_Shape;
    }

    void reboundSides() {
        m_Velocity.x = -m_Velocity.x;
    }

    void reboundTop() {
        m_Velocity.y = -m_Velocity.y;
    }

    void reboundBat() {
        m_Position.y -= (m_Velocity.y / 100);
        m_Velocity.y = -m_Velocity.y;
    }

    void update(Time dt) {
        m_Position.x += m_Velocity.x * dt.asSeconds();
        m_Position.y += m_Velocity.y * dt.asSeconds();
        
        m_Shape.setPosition(m_Position);
    }

    void reset(float startX, float startY) {
        m_Position.x = startX;
        m_Position.y = startY;
        m_Shape.setPosition(m_Position);
        
        // Reset with a slightly randomized angle
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> angleDist(-0.5f, 0.5f);
        float angleOffset = angleDist(gen);
        
        m_Velocity.x = m_Speed * (angleOffset + 1.0f); // Slight randomization
        m_Velocity.y = m_Speed;
    }
};

// Bomb class
class Bomb {
private:
    Vector2f m_Position;
    CircleShape m_Shape;
    float m_Speed = 300.0f;
    Vector2f m_Velocity;
    bool m_IsActive = false;

public:
    Bomb(float startX, float startY) {
        m_Position.x = startX;
        m_Position.y = startY;
        
        // Create a circular bomb shape
        m_Shape.setRadius(15);
        m_Shape.setFillColor(Color::Red);
        m_Shape.setOrigin(15, 15);
        m_Shape.setPosition(m_Position);
        
        // Set random initial velocity
        setRandomVelocity();
    }

    void setRandomVelocity() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> angleDist(0, 2 * 3.14159f);
        float angle = angleDist(gen);
        
        m_Velocity.x = std::cos(angle) * m_Speed;
        m_Velocity.y = std::sin(angle) * m_Speed;
    }

    FloatRect getPosition() {
        return m_Shape.getGlobalBounds();
    }

    CircleShape getShape() {
        return m_Shape;
    }

    void launch() {
        m_IsActive = true;
    }

    void update(Time dt, int windowWidth, int windowHeight) {
        if (m_IsActive) {
            // Move the bomb
            m_Position.x += m_Velocity.x * dt.asSeconds();
            m_Position.y += m_Velocity.y * dt.asSeconds();
            
            // Handle wall collisions
            if (m_Position.x <= 0 || m_Position.x >= windowWidth) {
                m_Velocity.x = -m_Velocity.x;
            }
            
            if (m_Position.y <= 0 || m_Position.y >= windowHeight) {
                m_Velocity.y = -m_Velocity.y;
            }
            
            m_Shape.setPosition(m_Position);
        }
    }

    bool isActive() {
        return m_IsActive;
    }

    void setActive(bool active) {
        m_IsActive = active;
    }

    void reset(float x, float y) {
        m_Position.x = x;
        m_Position.y = y;
        m_Shape.setPosition(m_Position);
        
        // Reset with a new random velocity
        setRandomVelocity();
        
        m_IsActive = false;
    }
};

int main() {
    // Create a window with the same aspect ratio as the monitor
    int windowWidth = 1024;
    int windowHeight = 768;
    RenderWindow window(VideoMode(windowWidth, windowHeight), "Pong with Bombs!");
    
    // Set frame rate limit
    window.setFramerateLimit(60);
    
    // Create the bat (paddle) at the bottom of the screen
    Bat bat(windowWidth / 2, windowHeight - 20);
    
    // Create a ball in the middle of the screen
    Ball ball(windowWidth / 2, windowHeight / 2);
    
    // Create a bomb
    Bomb bomb(windowWidth / 2, windowHeight / 2 - 100);
    float bombSpawnTimer = 0.0f;
    float bombSpawnInterval = 5.0f; // Spawn a new bomb every 5 seconds
    
    // Create a Text object for the HUD
    Text hud;
    Font font;
    if (!font.loadFromFile("arial.ttf")) {
        // If font loading fails, use a system default font or handle the error
        // For simplicity in this example, we'll continue without text
    }
    
    hud.setFont(font);
    hud.setCharacterSize(30);
    hud.setFillColor(Color::White);
    
    // Game stats
    int score = 0;
    
    // Game clock
    Clock clock;
    
    // Main game loop
    while (window.isOpen()) {
        /*
        ****************************************
        Handle the player input
        ****************************************
        */
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            
            if (event.type == Event::KeyPressed) {
                // Quit
                if (Keyboard::isKeyPressed(Keyboard::Escape)) {
                    window.close();
                }
                
                // Move paddle
                if (Keyboard::isKeyPressed(Keyboard::Left)) {
                    bat.moveLeft();
                } else if (Keyboard::isKeyPressed(Keyboard::Right)) {
                    bat.moveRight();
                }
            }
            
            if (event.type == Event::KeyReleased) {
                // Stop paddle movement
                if (event.key.code == Keyboard::Left) {
                    bat.stopLeft();
                } else if (event.key.code == Keyboard::Right) {
                    bat.stopRight();
                }
            }
        }

        /*
        ****************************************
        Update the game state
        ****************************************
        */
        
        // Get elapsed time
        Time dt = clock.restart();
        
        // Handle bomb spawning
        bombSpawnTimer += dt.asSeconds();
        if (bombSpawnTimer >= bombSpawnInterval) {
            bombSpawnTimer = 0.0f;
            if (!bomb.isActive()) {
                bomb.launch();
            }
        }
        
        // Update paddle, ball, and bomb
        if (bat.isAlive()) {
            bat.update(dt);
            ball.update(dt);
            bomb.update(dt, windowWidth, windowHeight);
            
            // Handle ball collision with walls
            if (ball.getPosition().left < 0 || ball.getPosition().left + ball.getPosition().width > windowWidth) {
                ball.reboundSides();
            }
            
            // Handle ball collision with ceiling
            if (ball.getPosition().top < 0) {
                ball.reboundTop();
            }
            
            // Handle ball collision with paddle
            if (ball.getPosition().intersects(bat.getPosition())) {
                ball.reboundBat();
                score++;
            }
            
            // Handle ball going past the paddle (bottom of screen)
            if (ball.getPosition().top > windowHeight) {
                ball.reset(windowWidth / 2, windowHeight / 2);
                bat.decreaseLives();
            }
            
            // Handle bomb collision with paddle
            if (bomb.isActive() && bomb.getPosition().intersects(bat.getPosition())) {
                bat.decreaseLives();
                bomb.reset(windowWidth / 2, windowHeight / 2 - 100);
            }
        }
        
        /*
        ****************************************
        Draw the game
        ****************************************
        */
        
        // Clear the window
        window.clear(Color(50, 50, 50, 255));
        
        // Update HUD text
        std::stringstream ss;
        ss << "Score: " << score << "    Lives: " << bat.getLives();
        hud.setString(ss.str());
        
        // Draw game elements
        window.draw(bat.getShape());
        window.draw(ball.getShape());
        window.draw(hud);
        
        // Draw the bomb if active
        if (bomb.isActive()) {
            window.draw(bomb.getShape());
        }
        
        // Draw game over message if player has no more lives
        if (!bat.isAlive()) {
            Text gameOver;
            gameOver.setFont(font);
            gameOver.setCharacterSize(75);
            gameOver.setFillColor(Color::Red);
            gameOver.setString("GAME OVER!");
            
            // Position text in center
            FloatRect textRect = gameOver.getLocalBounds();
            gameOver.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
            gameOver.setPosition(Vector2f(windowWidth / 2.0f, windowHeight / 2.0f));
            
            window.draw(gameOver);
        }
        
        // Display the window contents
        window.display();
    }

    return 0;
}