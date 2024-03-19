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

        //Private functions
        void initVariables();
        void initWindow();
        

        //Game Objects
        
        sf::Vector2i mousePosition;
        sf::Clock clock;
        float deltaTime;
        bool weaponSelected; // a variable for which weapon has been selected will eventually be necessary.

        std::vector<Projectile*> projectilesList;
        std::vector<Ship*> enemyShips;

        float playerSpeedx = 0; // these will eventually belong to the ship class.
        float playerSpeedy = 0;
                
        //Classes associated with game objects
        Ship playerShipObj;

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

        void fireWeapon(Ship firingShip);
        void renderProjectiles(); 
        void moveProjectiles(Projectile* projectile, int i);

        void render();
        
};