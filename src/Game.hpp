#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <vector>

#include "ship.hpp"
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
        

        //Game Objects
        
        sf::Vector2i mousePosition;
        sf::Clock clock;
        sf::Clock timer;
        float deltaTime;
        float torpedoTime;
        int weaponSelected; // a variable for which weapon has been selected will eventually be necessary.

        std::vector<Projectile*> projectilesList;
        std::vector<Ship*> enemyShips;
        std::vector<Ship*> allShips;

        float playerSpeedx = 0; // these will eventually belong to the ship class.
        float playerSpeedy = 0;
                
        //Objects associated with game objects
        Ship playerShipObj;
        Ship* playerShipPointer;

        //debug objects
        bool debugMode;
        std::vector<sf::RectangleShape> debugHitboxes;
        std::vector<sf::RectangleShape> enemyHitboxes;

        //for moving the enemy for test purposes
        float mov;


        SATHelper satHelper;

        sf::Font font;

        //log for recording ship, crew, systems damage and other events.
        std::vector<std::string> eventLog;

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

        void createEnemyHitboxes(Ship* enemyShip);
        void initEnemy();
        void renderEnemy();
        void updateEnemy();
        void renderEnemyHitboxes();

        void showRoomDamageEnemy();

        void updateAllShips();

        void fireTorpedo(Ship& firingShip);
        void fireDisruptor(Ship& firingShip);
        void firePhaser(Ship& firingShip);
        void renderProjectiles(); 
    

        void moveTorpedoes(Torpedo* projectile, int i);
        void moveDisruptors(Disruptor* projectile, int i);
        void movePhasers(Phaser* phaser, int i);


        void checkCollisions();

        void renderDebugObjects();

        void render();

        void logEvent(std::string event);

        void displayEvents();

        void createDebugBoxes(Ship* enemyShipObj);

        bool checkCollisionRectangleShape(sf::RectangleShape rect, sf::Vector2f vect);
};