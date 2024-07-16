#include "Projectile.hpp"

Projectile::Projectile(std::string texturePath, float x, float y, sf::Vector2f directionOfTravel, float speed, float damage) {
    projectileSprite.setOrigin(projectileSprite.getLocalBounds().width / 2, 0);
    friendly = false;
    this->speed = speed;
    this->spawnedAt.x = x;
    this->spawnedAt.y = y;
    this->directionOfTravel = directionOfTravel;
    this->damage = damage;
    this->texturePath = texturePath;
    if (!this->projectileTexture.loadFromFile(texturePath)) {
        std::cout << "Failed to load." << std::endl;
    }
    this->projectileSprite.setPosition(x, y);
    this->projectileSprite.setTexture(projectileTexture);
    
    this->touchingTarget = false;
    this->missed = false;
    this->hasPositionInitialized = false;
    this->hitChance = 0;
}

void Projectile::render(sf::RenderWindow* window) {
    window->draw(this->projectileSprite);
}

sf::Sprite Projectile::getSprite() {
    return this->projectileSprite;
}

void Projectile::setFaction(std::string factionName) {
    faction = factionName;
}

void Projectile::setFriendly() {
    friendly = true;
}

sf::Vector2f Projectile::getPosition() {
    return projectileSprite.getPosition();
}

Projectile::~Projectile() {
    
}

Torpedo::Torpedo(std::string texturePath, float x, float y, sf::Vector2f directionOfTravel, float speed, float damage) : 
Projectile(texturePath, x, y, directionOfTravel, speed, damage) {
    this->targetingSystem = false;
}

Disruptor::Disruptor(std::string texturePath, float x, float y, sf::Vector2f directionOfTravel, float speed, float damage) : 
Projectile(texturePath, x, y, directionOfTravel, speed, damage) {
    this->hitChance = 0;
    secondShot = false;
    secondShotDelay = 0;
    this->projectileSprite.setScale(4, 1);
}

Phaser::Phaser(std::string texturePath, float x, float y, sf::Vector2f directionOfTravel, float speed, float damage) : 
Projectile(texturePath, x, y, directionOfTravel, speed, damage) {
    collidedDeleteTimer = 0;
    hasCollided = false;
    this->hitChance = 0;
    this->projectileSprite.setScale(1, 0.25);
    phaserTimer = 0;
    phaserScaleX = 0;
    this->targetShip = nullptr;

    //hide sprite for testing
    //this->projectileSprite.setColor(sf::Color(255, 255, 255, 0));

    sf::Rect localBounds = projectileSprite.getLocalBounds();
    
    phaserRect = sf::RectangleShape(sf::Vector2f(localBounds.getSize().x, localBounds.getSize().y));
    phaserRect.setOrigin(phaserRect.getSize().x / 2, phaserRect.getSize().y / 2);
    phaserRect.setRotation(projectileSprite.getRotation());
    phaserRect.setPosition(projectileSprite.getPosition());

    phaserRect.setFillColor(sf::Color(255,255,0,128));
    //phaserRect.setOutlineColor(sf::Color(255,255,0,0));

}

