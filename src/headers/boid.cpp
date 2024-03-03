#include "steering.hpp"
#include "boid.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>

crumb::crumb(int id, float size)
{
    //set initial position and size breadcrumbs   
    this->id = id;         
    this->setRadius(size);
    this->setFillColor(sf::Color::Blue);
    this->setPosition(-100, -100);
}

//tell breadcrumb to render self, using current render window
void crumb::draw(sf::RenderWindow* window)
{
    window->draw(*this);
}

//set position of breadcrumb
void crumb::drop(float x, float y)
{
    this->setPosition(x, y);
}

//set position of breadcrumb
void crumb::drop(sf::Vector2f position)
{
    this->setPosition(position);
}

boid::boid(sf::RenderWindow* w, sf::Texture& tex, Kinematic* initial, std::vector<crumb>* crumbs, float drop_timer)
{
    window = w;
    sprite.setTexture(tex);
    sprite.setScale(0.04f, 0.04f);
    sprite.setOrigin(tex.getSize().x * 0.5f, tex.getSize().y * 0.5f);
    position = initial->position;
    orientation = initial->orientation;
    velocity = initial->velocity;
    rotation = initial->rotation;
    this->crumbs = crumbs;
    crumb_idx = 0;
    this->drop_timer = drop_timer;
    countdown = drop_timer;
}

void boid::move(SteeringData steering, float time)
{
    Kinematic::update(steering, time);
    sprite.setPosition(position);
    sprite.setRotation(orientation * 180 / M_PI);
    if (crumbs->size() > 0) {
        if (countdown > 0) {
            countdown -= 0.1f;
        }
        else {
            countdown = drop_timer;
            crumbs->at(crumb_idx).drop(position);

            if (crumb_idx < crumbs->size() - 1)
                crumb_idx++;
            else
                crumb_idx = 0;
        }
}
       
    
}

void boid::draw()
{
    window->draw(sprite);
}

sf::Vector2f boid::getPosition()
{
    return position;
}

void boid::setScale(float x, float y)
{
    sprite.setScale(x, y);
}

std::vector<crumb>* boid::getCrumbs()
{
    return crumbs;
}