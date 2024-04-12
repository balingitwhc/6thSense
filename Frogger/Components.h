//
// Created by David Burchill on 2023-09-27.
//

#ifndef BREAKOUT_COMPONENTS_H
#define BREAKOUT_COMPONENTS_H


#include <memory>
#include <SFML/Graphics.hpp>
#include "Utilities.h"
#include "Animation.h"
#include <bitset>


struct Component
{
    bool		has{ false };
    Component() = default;
};


struct CAnimation : public Component {
    Animation   animation;

    CAnimation() = default;
    CAnimation(const Animation& a) : animation(a) {}

};

struct CSprite : public Component {
    sf::Sprite sprite;

    CSprite() = default;

    CSprite(const sf::Texture &t)
            : sprite(t) {
        centerOrigin(sprite);
    }

    CSprite(const sf::Texture &t, sf::IntRect r)
            : sprite(t, r) {
        centerOrigin(sprite);
    }
};


struct CTransform : public Component
{

    sf::Transformable  tfm;
    sf::Vector2f	pos			{ 0.f, 0.f };
    sf::Vector2f	prevPos		{ 0.f, 0.f };
    sf::Vector2f	vel			{ 0.f, 0.f };
    sf::Vector2f	scale		{ 1.f, 1.f };

    float           angVel{ 0 };
    float	        angle{ 0.f };

    CTransform() = default;
    CTransform(const sf::Vector2f& p) : pos(p)  {}
    CTransform(const sf::Vector2f& p, const sf::Vector2f& v) : pos(p), prevPos(p),  vel(v){}

    CTransform(const sf::Vector2f& p, const sf::Vector2f& v, float a) : pos(p), prevPos(p), vel(v), angle(a) {
    }

};



struct CBoundingBox : public Component
{
    sf::Vector2f size{0.f, 0.f};
    sf::Vector2f halfSize{ 0.f, 0.f };

    CBoundingBox() = default;
    CBoundingBox(const sf::Vector2f& s) : size(s), halfSize(0.5f * s)
    {}
};

struct CState : public Component {
    std::string state{"none"};

    CState() = default;
    CState(const std::string& s) : state(s){}

};


struct CInput : public Component
{
    bool up{ false };
    bool left{ false };
    bool right{ false };
    bool down{ false };

    CInput() = default;
};

struct CShape : public Component
{
    sf::CircleShape circle;

    CShape() = default;


    CShape(float r, size_t points, const sf::Color& fill, const sf::Color& outline = sf::Color::Black, float thickness = 5.f)
        : circle(r, points)
    {
        circle.setFillColor(fill);
        circle.setOutlineColor(outline);
        circle.setOutlineThickness(thickness);
        circle.setOrigin(r, r);
    }
};

struct CScore : public Component
{
    int score{ 0 };

    CScore() = default;
    CScore(int p) : score(p) {}
};


struct CLifespan : public Component
{
    sf::Time total{ sf::Time::Zero };
    sf::Time remaining{ sf::Time::Zero };

    CLifespan() = default;
    CLifespan(float t) : total(sf::seconds(t)), remaining{ sf::seconds(t) } {}

};
#endif //BREAKOUT_COMPONENTS_H
