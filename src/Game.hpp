#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <vector>

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
        sf::Sprite playerShip;
        sf::Texture playerTexture;
        sf::Vector2i mousePosition;
        bool weaponSelected; // a variable for which weapon has been selected will eventually be necessary.

        std::vector<Projectile*> projectilesList;
        

        float playerSpeedx = 0; // these will eventually belong to the ship class.
        float playerSpeedy = 0;

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

        void fireWeapon(sf::Sprite firingShip);
        void renderProjectiles(); 

        void render();
        
       
};