#ifndef ORB_H
#define ORB_H

#include <algorithm>

#include "game.h"
#include "components.h"

#include "utility.h"

struct COrbArmBehaviour: Component {
    CGun *cGun = nullptr;
    CTransform *cTransform = nullptr;
    CParent *cParent = nullptr;

    void init() override {
        cGun = &entity->getComponent<CGun>();
        cTransform = &entity->getComponent<CTransform>();
        cParent = &entity->getComponent<CParent>();
    }

    void update(float elapsedTime) override {
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::K)) {

            sf::Transform t = cParent->getTransform();
            sf::Vector2f gunPos = cTransform->position;
            gunPos.x -= 64.f;
            gunPos.y += (cGun->projShot % 2 ?  -5 : 5);
            sf::Vector2f globalPosition = t.transformPoint(gunPos);
            float globalAngle = atan2(t.getMatrix()[1], t.getMatrix()[0]) * 180 / PI;

            cGun->fire(globalPosition, globalAngle);
        }
    }
};

struct CRLBehaviour: Component {
    CTransform *cTransform = nullptr;
    CParent *cParent = nullptr;
    CGun *cGun = nullptr;

    bool opening = false;
    bool closing = false;
    bool open = false;

    void init() override {
        cTransform = &entity->getComponent<CTransform>();
        cParent = &entity->getComponent<CParent>();
        cGun = &entity->getComponent<CGun>();
    }

    void update(float elapsedTime) override {
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::J)) {
            opening = true;
            closing = false;
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::H)) {
            closing = true;
            opening = false;
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
            sf::Transform t = cParent->getTransform();
            sf::Vector2f gunPos = cTransform->position;

            gunPos.y += (cGun->projShot % 2 ?  -5 : -15);

            gunPos.x -= 16.f;
            sf::Vector2f globalPosition = t.transformPoint(gunPos);
            float globalAngle = atan2(t.getMatrix()[1], t.getMatrix()[0]) * 180 / PI;

            cGun->fire(globalPosition, globalAngle, Groups::player);
        }

        if(opening) {
            cTransform->position.y += -30.f * elapsedTime;
            if(cTransform->position.y < -65.f) {
                cTransform->position.y = -65.f;
                opening = false;
                open = true;
            }
        }
        else if(closing) {
            cTransform->position.y += 30.f * elapsedTime;
            if(cTransform->position.y > -32.f) {
                cTransform->position.y = -32.f;
                closing = false;
                open = false;
            }
        }
    }
};

struct COrbBehaviour : Component {
    CLaserGun *cLaserGun = nullptr;
    CAnimatedSprite *cSprite = nullptr;
    CTarget *cTarget = nullptr;

    enum States : std::size_t {
        none,
        normal_to_close,
        close_to_open,
        open_laser,
        close_laser,
        open_to_close,
        close_to_normal
    };

    States currentState = States::none;

    void init() override {
        cLaserGun = &entity->getComponent<CLaserGun>();
        cSprite = &entity->getComponent<CAnimatedSprite>();
        cTarget = &entity->getComponent<CTarget>();
    }

    void update(float elapsedTime) override {
        switch(currentState) {
            case States::normal_to_close:
                cSprite->sprite.stop();
                cSprite->play("normal_to_close", [&]() { currentState = States::close_to_open; });
                currentState = States::none;
                break;
            case States::close_to_open:
                cSprite->sprite.stop();
                cSprite->play("close_to_open", [&]() { currentState = States::open_laser; });
                currentState = States::none;
                break;
            case States::open_laser:
                cLaserGun->openLaser();
                currentState = States::none;
                break;
            case States::close_laser:
                cLaserGun->closeLaser();
                currentState = States::open_to_close;
                break;
            case States::open_to_close:
                cSprite->sprite.stop();
                cSprite->play("open_to_close", [&]() { currentState = States::close_to_normal; });
                currentState = States::none;
                break;
            case States::close_to_normal:
                cSprite->sprite.stop();
                cSprite->play("close_to_normal");
                currentState = States::none;
                break;
            default:
                break;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
            // open laser
            currentState = States::normal_to_close;
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
            // close laser
            currentState = States::close_laser;
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::I)) {
            cTarget->targetOn = false;
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::U)) {
            cTarget->targetOn = true;
        }
    }
};

Entity& createRightArm(Game *game, Entity& parent) {
    auto& entity = game->manager.addEntity();

    entity.addComponent<CTransform>(sf::Vector2f(0.f,-110.f));
    entity.addComponent<CParent>(&parent);
    entity.addComponent<CSprite>(game, sf::Sprite(game->resource["orb"], {384,128,128,32}));
    entity.addComponent<CGun>(game, sf::Sprite(game->resource["orb"], {0,256,32,16}), 3.f, 200.f);
    entity.addComponent<COrbArmBehaviour>();

    entity.addGroup(Groups::drawable);

    return entity;
}

Entity& createLeftArm(Game *game, Entity& parent) {
    auto& entity = game->manager.addEntity();

    entity.addComponent<CTransform>(sf::Vector2f(0.f,110.f));
    entity.addComponent<CParent>(&parent);
    entity.addComponent<CSprite>(game, sf::Sprite(game->resource["orb"], {256,128,128,32}));
    entity.addComponent<CGun>(game, sf::Sprite(game->resource["orb"], {0,256,32,16}), 2.f, 200.f);
    entity.addComponent<COrbArmBehaviour>();

    entity.addGroup(Groups::drawable);

    return entity;
}

Entity& createRightRL(Game *game, Entity& parent) {
    auto& entity = game->manager.addEntity();

    entity.addComponent<CTransform>(sf::Vector2f(0.f,-32.f));
    entity.addComponent<CParent>(&parent);
    entity.addComponent<CSprite>(game, sf::Sprite(game->resource["orb"], {0,128,32,64}));
    entity.addComponent<CGun>(game, sf::Sprite(game->resource["orb"], {0,256,32,16}), 2.f, 200.f);
    entity.addComponent<CRLBehaviour>();

    entity.addGroup(Groups::drawable);
    entity.setLayer(-1);

    return entity;
}

Entity& createLeftRL(Game *game, Entity& parent) {
    auto& entity = game->manager.addEntity();

    entity.addComponent<CTransform>(sf::Vector2f(-1.5f,33.f));
    entity.addComponent<CParent>(&parent);
    entity.addComponent<CSprite>(game, sf::Sprite(game->resource["orb"], {128,128,32,64}));

    entity.addGroup(Groups::drawable);
    entity.setLayer(-1);

    return entity;
}

void createOrb(Game *game) {
    auto& entity = game->manager.addEntity();

    float width = 128.f, height = 128.f;

    entity.addComponent<CTransform>(sf::Vector2f(200.f,200.f));
    entity.addComponent<CAnimatedSprite>(game, AnimatedSprite(sf::seconds(0.8), true, false), width/2, height/2);

    Animation openEyeAnimation, closeEyeAnimation;

    openEyeAnimation.setSpriteSheet(game->resource["orb"]);
    closeEyeAnimation.setSpriteSheet(game->resource["orb"]);

    for(int i = 0; i < 4; i++)  openEyeAnimation.addFrame(sf::IntRect(width * i, 0, width, height));
    for(int i = 3; i > -1; i--) closeEyeAnimation.addFrame(sf::IntRect(width * i, 0, width, height));

    auto& sprite = entity.getComponent<CAnimatedSprite>();

    sprite.animations["openEyeAnimation"] = openEyeAnimation;
    sprite.animations["closeEyeAnimation"] = closeEyeAnimation;
    sprite.setAnimation("openEyeAnimation");

    entity.addComponent<CEnemyInput>();
    entity.addComponent<CTarget>(game, Groups::player, 0.5f, 0.8f);
    entity.addComponent<CLaserGun>(game, sf::Sprite(game->resource["orb"], {0,224,512,32}));
    entity.addComponent<COrbBehaviour>();

    entity.addGroup(Groups::drawable);

    createLeftArm(game, entity);
    createRightArm(game, entity);
    createRightRL(game, entity);
    createLeftRL(game, entity);
}


#endif /* ORB_H */