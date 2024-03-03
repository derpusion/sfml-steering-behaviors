#include "steering.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>

#pragma once

class crumb:sf::CircleShape
{
    public:
        crumb(int id, float size);

        void draw(sf::RenderWindow* window);

        void drop(float x, float y);

        void drop(sf::Vector2f position);

    private:
        int id;
        float size;
};

class boid:public Kinematic {
    public:
        sf::Sprite sprite;
        
        boid(sf::RenderWindow* w, sf::Texture& tex, Kinematic* initial, std::vector<crumb>* crumbs, float drop_timer);

        virtual void move(SteeringData steering, float time);

        virtual void draw();

        virtual sf::Vector2f getPosition();   

        virtual void setScale(float x, float y);

        virtual std::vector<crumb>* getCrumbs();

    private: 
        int crumb_idx;
        float drop_timer;
        float countdown;
        sf::RenderWindow* window;    
        std::vector<crumb>* crumbs;

};