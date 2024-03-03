#include "../headers/steering.hpp"
#include "../headers/boid.hpp"
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(640, 480), "Velocity Matching");
    sf::Texture texture;
    texture.loadFromFile("images/boid.png");
    
    // Set up the initial kinematic state for the boid
    Kinematic initial;
    initial.position = sf::Vector2f(320.0f, 240.0f); // Set the initial position
    initial.orientation = 0.0f; // Set the initial orientation
    initial.velocity = sf::Vector2f(0.0f, 0.0f); // Set the initial velocity
    initial.rotation = 0.0f; // Set the initial rotation

    // Make a crumb
    std::vector<crumb> breadcrumbs = std::vector<crumb>();
    crumb c(0, 10.0f);
    breadcrumbs.push_back(c);



    // Create a boid with the initial kinematic state
    boid b = boid(&window, texture, &initial, &breadcrumbs, 10.0f);

    // Set up the initial mouse position
    sf::Vector2i mousePosPast = sf::Mouse::getPosition(window);
    sf::Vector2i mousePosPresent = sf::Mouse::getPosition(window);

    // Main loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Sample the mouse pointer's location
        mousePosPresent = sf::Mouse::getPosition(window);

        // Calculate velocity from past and present mouse positions
        sf::Vector2f mouseVelocity = sf::Vector2f(mousePosPresent - mousePosPast);
        mouseVelocity /= 1.0f; // Assuming time interval is 1.0 seconds

        // Update past mouse position for the next iteration
        mousePosPast = mousePosPresent;

        // Create a Kinematic structure for the mouse velocity
        Kinematic mouseKinematic;
        mouseKinematic.velocity = mouseVelocity;

        // Calculate velocity matching steering force
        VelocityMatch velocityMatchBehavior(2.0f);
        SteeringData velocityMatchingSteering = velocityMatchBehavior.calculateAcceleration(b, mouseKinematic);

        // Update the boid with the calculated steering force
        b.move(velocityMatchingSteering, 1.0f);
        b.draw();
        
        window.display();
        window.clear(sf::Color::White);
    }

    return 0;
}