// wander.hpp
#include "steering.hpp"
#include "boid.hpp"
#include <SFML/Graphics.hpp>

#pragma once

float randBinomial();

sf::Vector2f asVector(float orientation);

class Wander {
    public:
        float wanderOffset;
        float wanderRadius;
        float wanderRate;

        Wander(float wanderOffset, float wanderRadius, float wanderRate, float timeToTarget,
               float maxRotation, float rSatisfaction, float rDeceleration,
               float maxVelocity, float angleSatisfaction, float angleDeceleration);  

        SteeringData wander(Kinematic character);

        SteeringData wanderB(Kinematic character);

    private:
        Arrive arriveBehavior;
        Face faceBehavior;
        LookWhereYoureGoing lookBehavior;
};
