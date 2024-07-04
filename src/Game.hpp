#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <vector>

#include "Ship.hpp"
#include "Projectile.hpp"

class Game {
    private:
        //Window
        sf::RenderWindow* window;
        sf::VideoMode videoMode;
        sf::Event event;
        sf::View view;

        //Private functions
        void initVariables();
        void initWindow();

        //bounds of the screen the player has already visited
        int alreadySeenLeft;
        int alreadySeenRight;
        int alreadySeenTop;
        int alreadySeenBottom;
        std::map<std::tuple<int, int>, bool> generatedChunks;

        std::vector<sf::Sprite> starSprites;
        sf::Texture newStarTexture;
        sf::Texture newStarTexture2;

        //Game Objects
        
        sf::Vector2i mousePosition;
        sf::Clock clock;
        sf::Clock timer;
        float deltaTime;
        float torpedoTime;

        std::vector<Projectile*> projectilesList;
        std::vector<Ship*> enemyShips;
        std::vector<Ship*> allShips;
        std::vector<Ship*> friendlyShips;

        float playerSpeedx = 0; // these will eventually belong to the ship class.
        float playerSpeedy = 0;
                
        //Objects associated with game objects
        Ship playerShipObj;
        Ship* playerShipPointer;

        //debug objects
        bool debugMode;
        std::vector<sf::RectangleShape> debugHitboxes;
        std::vector<sf::RectangleShape> enemyHitboxes;
        std::vector<sf::RectangleShape> friendlyHitboxes;

        //for moving the enemy for test purposes
        float mov;

        void makeDecision(Ship* ship);

        SATHelper satHelper;

        sf::Font font;

        //log for recording ship, crew, systems damage and other events.
        std::vector<std::tuple<std::string, bool>> eventLog;
        std::vector<std::tuple<sf::Text, int>> miniTextVect;
    public:
        //Constructors and Destructors
        Game();
        virtual ~Game();

        const bool getWindowIsOpen() const;

        //Public functions
        
        void updateEvents();
        void update();

        void initPlayer();
        void updatePlayer();
        void movePlayer();
        void renderPlayer();

        void createShipHitboxes(Ship* enemyShip);
        void initEnemy();
        void renderEnemy();
        void updateEnemy();
        void renderEnemyHitboxes();

        void showRoomDamage();

        void updateAllShips();
        void moveShip(Ship* ship, sf::Vector2f moveTo);

        void fireTorpedo(Ship& firingShip, int hitChance, float damage, sf::Vector2f parentTip);
        void fireDisruptor(Ship& firingShip, int hitChance, float damage, sf::Vector2f parentTip);
        void firePhaser(Ship& firingShip, int hitChance, float damage, sf::Vector2f parentTip);
        void fireTorpedoSpread(Ship& firingShip, int hitChance, float damage, sf::Vector2f parentTip);

        void fireTorpedo(Ship& firingShip, sf::Vector2f targetP, int hitChance, float damage, sf::Vector2f parentTip);
        void fireDisruptor(Ship& firingShip, sf::Vector2f targetP, int hitChance, float damage, sf::Vector2f parentTip);
        void firePhaser(Ship& firingShip, sf::Vector2f targetP, int hitChance, float damage, sf::Vector2f parentTip);
        void fireTorpedoSpread(Ship& firingShip, sf::Vector2f targetP, int hitChance, float damage, sf::Vector2f parentTip);
        void renderProjectiles(); 
    

        void moveTorpedoes(Torpedo* projectile, int i);
        void moveDisruptors(Disruptor* projectile, int i);
        void movePhasers(Phaser* phaser, int i);

        bool pickWeapon(Ship& ship);
        bool pickWeapon(Ship* ship);
        void useWeapon(Ship& ship);
        void useWeapon(Ship* ship, sf::Vector2f enemyPosition);

        void checkCollisions();

        void renderDebugObjects();

        void render();

        void logEvent(std::string event, bool friendly);

        void displayEvents();

        void miniTextCreate(std::string text, sf::Vector2f pos);

        void displayMiniText();

        void createDebugBoxes(Ship* enemyShipObj);

        bool checkCollisionRectangleShape(sf::RectangleShape rect, sf::Vector2f vect);

        void generateStarsStart();
        void generateStars();
        void renderStars();

        void renderCoordinates();

        void setGameView(sf::Vector2f viewCoordinates);
};