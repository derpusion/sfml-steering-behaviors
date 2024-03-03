#include "../headers/steering.hpp"
#include "../headers/boid.hpp"
#include "../headers/wander.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <numeric>

const float maxVelocity = 5.0;
const float maxRotation = 0.5;
const float rDeceleration = 150.0;
const float rSatisfaction = 25.0;
const float angleDeceleration = .25 * M_PI;
const float angleSatisfaction = .05 * M_PI;
const float timeToTarget = 0.5;

Align alignBehaviorF(maxRotation, angleSatisfaction, angleDeceleration, timeToTarget);
Arrive arriveBehaviorF(maxVelocity, rSatisfaction, rDeceleration, timeToTarget);
Face faceBehaviorF(maxRotation, angleSatisfaction, angleDeceleration, timeToTarget);
VelocityMatch velocityMatchBehaviorF(timeToTarget);
LookWhereYoureGoing lookBehaviorF(maxRotation, angleSatisfaction, angleDeceleration, timeToTarget);

float distance(sf::Vector2f a, sf::Vector2f b)
{
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

class flockingBoid: public boid
{
    public:
        float separationRadius;
        float cohesionRadius;
        float alignmentRadius;

        flockingBoid(sf::RenderWindow* window, sf::Texture& texture, Kinematic* initial, float separationRadius, float cohesionRadius, float alignmentRadius, std::vector<crumb>* crumbs, float drop_timer) 
        : boid(window, texture, initial, crumbs, drop_timer)
        {
            this->separationRadius = separationRadius;
            this->cohesionRadius = cohesionRadius;
            this->alignmentRadius = alignmentRadius;
        }
};

class separation
{
    public:
        float threshold;
        float decayCoefficient;
        float maxAcceleration;

        separation(float threshold, float decayCoefficient, float maxAcceleration)
        {
            this->threshold = threshold;
            this->decayCoefficient = decayCoefficient;
            this->maxAcceleration = maxAcceleration;
        }

        SteeringData calculateAcceleration(Kinematic character, std::vector<flockingBoid*>& otherBoids)
        {
            SteeringData result;
            result.linear = sf::Vector2f(0, 0);
            result.angular = 0;

            for (int i = 0; i < otherBoids.size(); i++) {
                if (otherBoids[i]->position != character.position) {
                    sf::Vector2f direction = character.position - otherBoids[i]->position;
                    float distance = sqrt(pow(direction.x, 2) + pow(direction.y, 2));
                    if (distance < threshold) {
                        float strength = std::min(decayCoefficient * distance * distance, maxAcceleration);
                        direction = normalize(direction);
                        result.linear += strength * direction;

                        // Add to the result
                        result.linear = normalize(result.linear);
                        result.linear *= maxAcceleration;
                    }
                }
            }
            return result;
        }
};

// Used to position match center of mass of nearby boids to the character
class cohesion
{
    public:
        float threshold;

        cohesion(float threshold) 
        {
            this->threshold = threshold;
        }

        SteeringData calculateAcceleration(Kinematic character, std::vector<flockingBoid*>& otherBoids) 
        {
            SteeringData result;
            result.linear = sf::Vector2f(0, 0);
            result.angular = 0;

            sf::Vector2f centerOfMass = sf::Vector2f(0, 0);
            int count = 0;
            for (int i = 0; i < otherBoids.size(); i++) {
                if (otherBoids[i]->position != character.position) {
                    sf::Vector2f direction = character.position - otherBoids[i]->position;
                    float distance = sqrt(pow(direction.x, 2) + pow(direction.y, 2));
                    if (distance < threshold) {
                        centerOfMass += otherBoids[i]->position;
                        count++;
                    }
                }
            }
            if (count > 0) {
                centerOfMass.x /= (float)count;
                centerOfMass.y /= (float)count;
            }    

            Kinematic target;
            target.position = centerOfMass;
            target.orientation = 0;
            target.velocity = sf::Vector2f(0, 0);
            target.rotation = 0;

            result = arriveBehaviorF.calculateAcceleration(character, target);

            return result;
        }

};

// Velocity match center of mass of nearby boids to the character
class alignment
{
    public:
        float threshold;

        alignment(float threshold) 
        {
            this->threshold = threshold;
        }

        SteeringData calculateAcceleration(Kinematic character, std::vector<flockingBoid*>& otherBoids) 
        {
            SteeringData result;
            result.linear = sf::Vector2f(0, 0);
            result.angular = 0;

            sf::Vector2f averageVelocity = sf::Vector2f(0, 0);
            int count = 0;
            for (int i = 0; i < otherBoids.size(); i++) {
                if (otherBoids[i]->position != character.position) {
                    sf::Vector2f direction = character.position - otherBoids[i]->position;
                    float distance = sqrt(pow(direction.x, 2) + pow(direction.y, 2));
                    if (distance < threshold) {
                        averageVelocity += otherBoids[i]->velocity;
                        count++;
                    }
                }
            }
            if (count > 0) {
                averageVelocity.x /= (float)count;
                averageVelocity.y /= (float)count;
            }    

            Kinematic target;
            target.position = character.position;
            target.orientation = 0;
            target.velocity = averageVelocity;
            target.rotation = 0;

            result = velocityMatchBehaviorF.calculateAcceleration(character, target);

            return result;
        }
};

int main(int argc, char* argv[])
{
    bool crumbMode = false;
    if (argc != 1) {
        if (std::string(argv[1]) == "crumb") {
            crumbMode = true;
            printf("Crumb mode enabled\n");
        }
    }
    sf::RenderWindow window(sf::VideoMode(640, 480), "Flocking");
    sf::Texture texture;
    texture.loadFromFile("images/boid.png");

    // Create a flock of boids
    std::vector<flockingBoid*> boids;

    std::vector<crumb> breadcrumbs[100]; // Changed the array to a vector of vectors
    for (int i = 0; i < 100; i++) {
        
        breadcrumbs[i] = std::vector<crumb>();
        for (int j = 0; j < 4; j++) { // Changed the loop variable from i to j
            crumb c(j, 2.0f);
            breadcrumbs[i].push_back(c);
        }
        

        Kinematic initial;
        initial.position = sf::Vector2f(rand() % 640, rand() % 480);
        initial.orientation = rand() % 360;
        initial.velocity = sf::Vector2f(0.0f, 0.0f);
        initial.rotation = 0.0f;
        flockingBoid* b = new flockingBoid(&window, texture, &initial, 20, 150, 100, &breadcrumbs[i], 0.75f);
        b->setScale(0.01, 0.01);
        boids.push_back(b);
    }

    // Weights
    float alignmentWeight = .3;
    float cohesionWeight = .1;
    float separationWeight = .2;
    float wanderWeight = .4;

    // Behavior
    float wanderOffset = 150.0;
    float wanderRadius = 60.0;
    float wanderRate = 2 * M_PI;
    Wander wanderBehavior(wanderOffset, wanderRadius, wanderRate, timeToTarget,
                         maxRotation, rSatisfaction, rDeceleration,
                         maxVelocity, angleSatisfaction, angleDeceleration);
    float separationThreshold = 10.0;
    float separationDecay = 0.5;
    float separationMaxAcceleration = 5.0;
    separation separationBehavior(separationThreshold, separationDecay, separationMaxAcceleration);

    float cohesionThreshold = 100.0;
    cohesion cohesionBehavior(cohesionThreshold);

    float alignmentThreshold = 80.0;
    alignment alignmentBehavior(alignmentThreshold);

    // Clock for maintaining 60 FPS
    sf::Clock clock;
    const sf::Time frameTime = sf::seconds(1.0f / 60.0f);

    // Main loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Use simple blending to calculate the steering force
        for (int i = 0; i < boids.size(); i++) {
            // Boundary behavior
            if (boids[i]->position.x < 0) {
                boids[i]->position.x = 640;
            } else if (boids[i]->position.x > 640) {
                boids[i]->position.x = 0;
            }

            if (boids[i]->position.y < 0) {
                boids[i]->position.y = 480;
            } else if (boids[i]->position.y > 480) {
                boids[i]->position.y = 0;
            }

            // Calculate the steering force
            SteeringData result;
            SteeringData separationSteering = separationBehavior.calculateAcceleration(*boids[i], boids);
            SteeringData cohesionSteering = cohesionBehavior.calculateAcceleration(*boids[i], boids);
            SteeringData alignmentSteering = alignmentBehavior.calculateAcceleration(*boids[i], boids);
            SteeringData lookWhereSteering = lookBehaviorF.look(*boids[i]);
            SteeringData wanderSteering = wanderBehavior.wander(*boids[i]);

            result.linear = separationWeight * separationSteering.linear + cohesionWeight * cohesionSteering.linear + alignmentWeight * alignmentSteering.linear + wanderWeight * wanderSteering.linear;
            // result.linear = separationWeight * separationSteering.linear + cohesionWeight * cohesionSteering.linear + alignmentWeight * alignmentSteering.linear;
            result.angular = lookWhereSteering.angular;

            // Update the boid with the calculated steering force
            boids[i]->move(result, .5);
            boids[i]->draw();

            // // Draw the crumbs for each boid
            if (crumbMode) {
                for (int j = 0; j < breadcrumbs[i].size(); j++) {
                    breadcrumbs[i][j].draw(&window);
                }
            }
        }
        window.display();
        window.clear(sf::Color::White);

        // Sleep to maintain 60 FPS
        sf::sleep(frameTime - clock.getElapsedTime());
        clock.restart();
    }
    return 0;
}