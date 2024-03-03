#include "steering.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>

float mapToRange(double rotation)
{
    rotation = fmod(rotation, 2*M_PI);
    if (abs(rotation) <= M_PI) {
        return rotation;
    } else if (rotation > M_PI) {
        return rotation - 2*M_PI;
    } else {
        return rotation + 2*M_PI;
    }
}

sf::Vector2f normalize(sf::Vector2f vector)
{
    float magnitude = sqrt(pow(vector.x, 2) + pow(vector.y, 2));
    vector.x /= magnitude;
    vector.y /= magnitude;
    return vector;
}

Align::Align(float maxRotation, float angleSatisfaction, float angleDeceleration, float timeToTarget)
{
    this->maxRotation = maxRotation;
    this->angleSatisfaction = angleSatisfaction;
    this->angleDeceleration = angleDeceleration;
    this->timeToTarget = timeToTarget;
}

SteeringData Align::calculateAcceleration(Kinematic character, Kinematic target)
{
    SteeringData result;
    float rotation = target.orientation - character.orientation;
    rotation = mapToRange(rotation);
    float rotationSize = std::fabs(rotation);
    float goalRotation;

    if (rotationSize < angleSatisfaction) {
        goalRotation = 0.0;
    } else if (rotationSize > angleDeceleration) {
        goalRotation = maxRotation;
    } else {
        goalRotation = maxRotation * rotationSize / angleDeceleration;
    }

    goalRotation *= rotation / std::fabs(rotation);
    result.angular = goalRotation - character.rotation;
    result.angular /= timeToTarget;
    result.linear = sf::Vector2f(0, 0);

    return result;
}

Arrive::Arrive(float maxVelocity, float rSatisfaction, float rDeceleration, float timeToTarget)
{
    this->maxVelocity = maxVelocity;
    this->rSatisfaction = rSatisfaction;
    this->rDeceleration = rDeceleration;
    this->timeToTarget = timeToTarget;
}

SteeringData Arrive::calculateAcceleration(Kinematic character, Kinematic target) {
    SteeringData result;
    sf::Vector2f direction = target.position - character.position;
    float distance = sqrt(pow(direction.x, 2) + pow(direction.y, 2));
    float goalSpeed;

    if (distance < rSatisfaction) {
        goalSpeed = 0;
    } else if (distance > rDeceleration) {
        goalSpeed = maxVelocity;
    } else {
        goalSpeed = maxVelocity * distance / rDeceleration;
    }

    sf::Vector2f goalVelocity = direction;
    goalVelocity = normalize(goalVelocity);
    goalVelocity *= goalSpeed;

    result.linear = goalVelocity - character.velocity;
    result.linear /= timeToTarget;
    result.angular = 0;

    return result;
}

VelocityMatch::VelocityMatch(float timeToTarget) {
    this->timeToTarget = timeToTarget;
}

SteeringData VelocityMatch::calculateAcceleration(Kinematic character, Kinematic target) {
    SteeringData result;
    result.linear = target.velocity - character.velocity;
    result.linear /= timeToTarget;
    result.angular = 0;
    return result;
}

RotationMatch::RotationMatch(float timeToTarget) {
    this->timeToTarget = timeToTarget;
}

SteeringData RotationMatch::calculateAcceleration(Kinematic character, Kinematic target) {
    SteeringData result;
    result.angular = target.rotation - character.rotation;
    result.angular /= timeToTarget;
    result.linear = sf::Vector2f(0, 0);
    return result;
}

Face::Face(float maxRotation, float angleSatisfaction, float angleDeceleration, float timeToTarget) : Align(maxRotation, angleSatisfaction, angleDeceleration, timeToTarget) {}

SteeringData Face::face(Kinematic character, Kinematic target) {
    // Calculate the desired orientation to face the target
    sf::Vector2f direction = target.position - character.position;
    float targetOrientation = atan2(direction.y, direction.x);

    target.orientation = targetOrientation;

    // Use the Align behavior to calculate the steering force
    return calculateAcceleration(character, target);
}

LookWhereYoureGoing::LookWhereYoureGoing(float maxRotation, float angleSatisfaction, float angleDeceleration, float timeToTarget) : Align(maxRotation, angleSatisfaction, angleDeceleration, timeToTarget) {}

SteeringData LookWhereYoureGoing::look(Kinematic character) {
    if (character.velocity.x == 0 && character.velocity.y == 0) {
        SteeringData result;
        result.linear = sf::Vector2f(0, 0);
        result.angular = 0;
        return result;
    }

    // Calculate the desired orientation to face the target
    sf::Vector2f direction = character.velocity;
    float targetOrientation = atan2(direction.y, direction.x);

    Kinematic target;
    target.rotation = 0;
    target.velocity = sf::Vector2f(0, 0);
    target.position = character.position;
    target.orientation = targetOrientation;

    // Use the Align behavior to calculate the steering force
    return calculateAcceleration(character, target);
}