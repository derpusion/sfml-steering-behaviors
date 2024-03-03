#include "../headers/steering.hpp"
#include "../headers/boid.hpp"
#include <SFML/Graphics.hpp>

int main(int argc, char* argv[])
{
    bool crumbMode = false;
    if (argc != 1) {
        if (std::string(argv[1]) == "crumb") {
            crumbMode = true;
            printf("Crumb mode enabled\n");
        }
    }

    sf::RenderWindow window(sf::VideoMode(640, 480), "Align and Arrive");
    sf::Texture texture;
    texture.loadFromFile("images/boid.png");

    // Set up the initial kinematic state for the boid
    Kinematic initial;
    initial.position = sf::Vector2f(0.0f, 0.0f);
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
    boid b = boid(&window, texture, &initial, &breadcrumbs, 5.0f);

    // Initialize behaviors
    float maxVelocity = 10.0;
    float maxRotation = 0.5;
    float rDeceleration = 100.0;
    float rSatisfaction = 25.0;
    float angleDeceleration = .25 * M_PI;
    float angleSatisfaction = .05 * M_PI;
    float timeToTarget = 0.5;
    Align alignBehavior(maxRotation, angleSatisfaction, angleDeceleration, timeToTarget);
    Arrive arriveBehavior(maxVelocity, rSatisfaction, rDeceleration, timeToTarget);
    Face faceBehavior(maxRotation, angleSatisfaction, angleDeceleration, timeToTarget);

    // Set up the initial target position (invalid position to start with)
    sf::Vector2i targetPosition(-1, -1);

    // Clock for maintaining 60 FPS
    sf::Clock clock;
    const sf::Time frameTime = sf::seconds(1.0f / 60.0f);

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                // Update the targetPosition when a mouse click occurs
                targetPosition = sf::Mouse::getPosition(window);
            }
        }
        window.clear(sf::Color::White);
        
        // If a valid target position is set, update the boid's behavior
        if (targetPosition.x != -1 && targetPosition.y != -1) {
            // Update the boid's position based on the Arrive behavior
            Kinematic target;
            target.position = sf::Vector2f(targetPosition);
            target.orientation = 0.0f;
            target.velocity = sf::Vector2f(0.0f, 0.0f);
            target.rotation = 0.0f;

            // Update the boid's velocity based on the Arrive behavior
            SteeringData arriveSteering = arriveBehavior.calculateAcceleration(b, target);
            b.move(arriveSteering, 0.1f);

            // Update the boid's orientation based on the Face/Align behavior
            SteeringData alignSteering = faceBehavior.face(b, target);
            b.move(alignSteering, 0.1f);
        }

        // Draw the boid
        b.draw();

        window.draw(b.sprite);
        
        // Crumbs
        if (crumbMode) {
            for (int i = 0; i < breadcrumbs.size(); i++) {
                breadcrumbs[i].draw(&window);
            }
        }

        window.display();

        

        // Sleep to maintain 60 FPS
        sf::sleep(frameTime - clock.getElapsedTime());
        clock.restart();
    }

    return 0;
}
