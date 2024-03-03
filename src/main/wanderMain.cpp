#include "../headers/steering.hpp"
#include "../headers/boid.hpp"
#include "../headers/wander.hpp"
#include <SFML/Graphics.hpp>

const float wanderOffset = 150.0;
const float wanderRadius = 60.0;
const float wanderRate = M_PI;

const float maxVelocity = 5.0;
const float maxRotation = .85;
const float rDeceleration = 150.0;
const float rSatisfaction = 25.0;
const float angleDeceleration = .25 * M_PI;
const float angleSatisfaction = .05 * M_PI;
const float timeToTarget = 0.5;
const float dTime = 1.0f / 60.0f;

int main(int argc, char* argv[])
{
    bool crumbMode = false;
    if (argc != 1) {
        if (std::string(argv[1]) == "crumb") {
            crumbMode = true;
            printf("Crumb mode enabled\n");
        }
    }
    sf::RenderWindow window(sf::VideoMode(640, 480), "Wander");
    sf::Texture texture;
    texture.loadFromFile("images/boid.png");

    // Set up the initial kinematic state for the boid
    Kinematic initial;
    initial.position = sf::Vector2f(320.0f, 240.0f);
    initial.orientation = 0.0f;
    initial.velocity = sf::Vector2f(0.0f, 0.0f);
    initial.rotation = 0.0f;

    // Make some crumbs
    std::vector<crumb> breadcrumbs = std::vector<crumb>();
    for (int i = 0; i < 10; i++) {
        crumb c(i, 10.0f);
        breadcrumbs.push_back(c);
    }

    // Create a boid 
    boid b = boid(&window, texture, &initial, &breadcrumbs, 50.0f);

    // Behavior
    Wander wanderBehavior(wanderOffset, wanderRadius, wanderRate, timeToTarget,
                         maxRotation, rSatisfaction, rDeceleration,
                         maxVelocity, angleSatisfaction, angleDeceleration);

    // Clock for maintaining 60 FPS
    sf::Clock clock;
    const sf::Time frameTime = sf::seconds(1.0f / 60.0f);

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear(sf::Color::White);

        if (b.position.x < 0) {
            b.position.x = 640;
        } else if (b.position.x > 640) {
            b.position.x = 0;
        }

        if (b.position.y < 0) {
            b.position.y = 480;
        } else if (b.position.y > 480) {
            b.position.y = 0;
        }
        
        // Update the boid's position based on the Wander behavior
        SteeringData steering = wanderBehavior.wander(b);
        b.move(steering, dTime);
        b.draw();

        if (crumbMode) {
            for (int i = 0; i < breadcrumbs.size(); i++) {
                breadcrumbs[i].draw(&window);
            }
        }

        window.display();
    }
    return 0;
}