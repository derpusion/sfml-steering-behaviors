// wander.cpp
#include "wander.hpp"
#include "steering.hpp"
#include "boid.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>

float randBinomial() {
    return (rand() - rand()) / (float)RAND_MAX;
}

sf::Vector2f asVector(float orientation) {
    return sf::Vector2f(cos(orientation), sin(orientation));
}

Wander::Wander(float wanderOffset, float wanderRadius, float wanderRate, float timeToTarget,
               float maxRotation, float rSatisfaction, float rDeceleration,
               float maxVelocity, float angleSatisfaction, float angleDeceleration)
               : arriveBehavior(maxVelocity, rSatisfaction, rDeceleration, timeToTarget),
                 faceBehavior(maxRotation, angleSatisfaction, angleDeceleration, timeToTarget),
                 lookBehavior(maxRotation, angleSatisfaction, angleDeceleration, timeToTarget){
    this->wanderOffset = wanderOffset;
    this->wanderRadius = wanderRadius;
    this->wanderRate = wanderRate;
}

SteeringData Wander::wander(Kinematic character) {
    float wanderOrientation = randBinomial() * wanderRate;
    float targetOrientation = wanderOrientation + character.orientation;

    // Find circle center
    sf::Vector2f targetPosition = character.position + wanderOffset * asVector(character.orientation);
    targetPosition += wanderRadius * asVector(targetOrientation);

    // Set the target position and orientation
    Kinematic target;
    target.position = targetPosition;
    target.orientation = targetOrientation;
    target.velocity = sf::Vector2f(0.0f, 0.0f);
    target.rotation = 0.0f;

    SteeringData result;
    result = arriveBehavior.calculateAcceleration(character, target);
    result.angular = faceBehavior.calculateAcceleration(character, target).angular;

    return result;
}

SteeringData Wander::wanderB(Kinematic character) {
    float wanderOrientation = randBinomial() * wanderRate;
    float targetOrientation = wanderOrientation + character.orientation;

    // Find circle center
    sf::Vector2f targetPosition = character.position + wanderOffset * asVector(character.orientation);
    targetPosition += wanderRadius * asVector(targetOrientation);

    // Set the target position and orientation
    Kinematic target;
    target.position = targetPosition;
    target.orientation = targetOrientation;
    target.velocity = sf::Vector2f(0.0f, 0.0f);
    target.rotation = 0.0f;

    SteeringData result;
    result = arriveBehavior.calculateAcceleration(character, target);
    result.angular = lookBehavior.calculateAcceleration(character, target).angular;

    return result;
}