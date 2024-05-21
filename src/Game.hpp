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
        bool weaponSelected; // a variable for which weapon has been selected will eventually be necessary.

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

        //for moving the enemy for test purposes
        float mov = 0.2;


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

        void initEnemy();
        void renderEnemy();
        void updateEnemy();

        void showRoomDamageEnemy();

        void updateAllShips();

        void fireWeapon(Ship& firingShip);
        void renderProjectiles(); 
        void moveProjectiles(Projectile* projectile, int i);

        void checkCollisions();

        void renderDebugObjects();

        void render();

        void logEvent(std::string event);

        void displayEvents();

        void createDebugBoxes(Ship* enemyShipObj);

        bool checkCollisionRectangleShape(sf::RectangleShape rect, sf::Vector2f vect);
};