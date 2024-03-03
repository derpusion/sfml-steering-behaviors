#include <SFML/Graphics.hpp>
#include <cmath>
#pragma once

#define maxAccel 10.0f
#define maxRot 1.0f

float mapToRange(double rotation);
sf::Vector2f normalize(sf::Vector2f vector);

struct SteeringData
{
    sf::Vector2f linear;
    float angular;
};

struct Kinematic {
    sf::Vector2f position;
    float orientation;
    sf::Vector2f velocity;
    float rotation;

    void update(SteeringData steering, float time)
    {
        // Clip to maxAccel
        if (sqrt(pow(steering.linear.x, 2) + pow(steering.linear.y, 2)) > maxAccel)
        {
            steering.linear = normalize(steering.linear);
            steering.linear.x = steering.linear.x * maxAccel;
            steering.linear.y = steering.linear.y * maxAccel;
        }

        // Clip to maxRot
        if (abs(steering.angular) > maxRot)
        {
            steering.angular = maxRot * (steering.angular / abs(steering.angular));
        }
        
        position += velocity * time;
        orientation += rotation * time;
        velocity += steering.linear * time;
        rotation += steering.angular * time;

        
    } 
};

class SteeringBehavior
{
    public:
    virtual SteeringData calculateAcceleration(Kinematic character, Kinematic target) = 0;
    float maxVelocity;
    float maxRotation;
};

class Align : public SteeringBehavior {
public:
    Align(float maxRotation, float angleSatisfaction, float angleDeceleration, float timeToTarget);
    float angleSatisfaction;
    float angleDeceleration;
    float timeToTarget;
    virtual SteeringData calculateAcceleration(Kinematic character, Kinematic target);
};

class Arrive : public SteeringBehavior {
public:
    Arrive(float maxVelocity, float rSatisfaction, float rDeceleration, float timeToTarget);
    float rSatisfaction;
    float rDeceleration;
    float timeToTarget;
    virtual SteeringData calculateAcceleration(Kinematic character, Kinematic target);
};

class VelocityMatch : public SteeringBehavior {
public:
    VelocityMatch(float timeToTarget);
    float timeToTarget;

    virtual SteeringData calculateAcceleration(Kinematic character, Kinematic target);
};

class RotationMatch : public SteeringBehavior {
public:
    RotationMatch(float timeToTarget);
    float timeToTarget;

    virtual SteeringData calculateAcceleration(Kinematic character, Kinematic target);
};

class Face : public Align {
public:
    Face(float maxRotation, float angleSatisfaction, float angleDeceleration, float timeToTarget);

    virtual SteeringData face(Kinematic character, Kinematic target);
};

class LookWhereYoureGoing : public Align {
public:
    LookWhereYoureGoing(float maxRotation, float angleSatisfaction, float angleDeceleration, float timeToTarget);

    virtual SteeringData look(Kinematic character);
};