#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <list>

#include "Ship.hpp"
#include "Projectile.hpp"
#include "Menu.hpp"


class Game {
    private:
        //Window
        sf::RenderWindow* window;
        sf::VideoMode videoMode;
        sf::Event event;
        sf::View view;
        sf::View uiView;

        //Private functions
        void initVariables();
        void initWindow();

        //bounds of the screen the player has already visited
        int alreadySeenLeft;
        int alreadySeenRight;
        int alreadySeenTop;
        int alreadySeenBottom;
        std::map<std::tuple<int, int>, std::vector<std::shared_ptr<sf::Sprite>>*> generatedChunks;

        sf::Texture newStarTexture;
        sf::Texture newStarTexture2;

        //Game Objects
        
        std::list<std::shared_ptr<sf::Sound>> soundsList;
        std::list<std::shared_ptr<sf::SoundBuffer>> buffers;

        sf::Clock clock;
        sf::Clock timer;
        float deltaTime;
        float torpedoTime;

        std::vector<Projectile*> projectilesList;
        std::vector<Ship*> enemyShips;
        std::vector<Ship*> allShips;
        std::vector<Ship*> friendlyShips;

        bool usingWarp;

                
        //Objects associated with game objects
        Ship playerShipObj;
        Ship* playerShipPointer;

        //debug objects
        bool debugMode;
        std::vector<sf::RectangleShape> debugHitboxes;
        std::vector<sf::RectangleShape> enemyHitboxes;
        std::vector<sf::RectangleShape> friendlyHitboxes;

        bool paused;

        void makeDecision(Ship* ship);

        SATHelper satHelper;

        sf::Font arial;
        sf::Font tos;
        sf::Font okuda;

        //log for recording ship, crew, systems damage and other events.
        std::vector<std::tuple<std::string, bool>> eventLog;
        std::vector<std::tuple<sf::Text, int>> miniTextVect;

        std::vector<std::tuple<std::shared_ptr<sf::RectangleShape>, std::shared_ptr<sf::RectangleShape>, 
                    std::shared_ptr<sf::Text>, std::string>> cooldownUIElements;

        MainMenu menuClass;

        Button::buttonActions mouseOver = Button::buttonActions::NONE;
    public:
        //Constructors and Destructors
        Game();
        virtual ~Game();

        const bool getWindowIsOpen() const;

        enum class GameState {
            MainMenu,
            Playing
        };

        GameState state;

        //Public functions
        
        void updateEvents();
        void update();
        void updateMainMenu();
        void updateMainMenuEvents();

        void initPlayer();
        void updatePlayer();
        void movePlayer();
        void movePlayerWarp();
        void renderPlayer();

        void createShipHitboxes(Ship* enemyShip);
        void initEnemy();
        void renderEnemy();
        void updateEnemy();
        void renderEnemyHitboxes();

        void showRoomDamage();

        void updateAllShips();
        void moveShip(Ship* ship, sf::Vector2f moveTo);
        void rotateBeforeWarp(Ship* ship, sf::Vector2f moveTo);
        void rotatePlayerBeforeWarp();
        void moveShipWarp(Ship* ship, sf::Vector2f moveTo);

        void fireTorpedo(Ship& firingShip, int hitChance, float damage, sf::Vector2f parentTip);
        void fireDisruptor(Ship& firingShip, int hitChance, float damage, sf::Vector2f parentTip, sf::Vector2f offset, std::shared_ptr<System> firingSystem);
        void firePhaser(Ship& firingShip, int hitChance, float damage, sf::Vector2f parentTip);
        void fireTorpedoSpread(Ship& firingShip, int hitChance, float damage, sf::Vector2f parentTip);

        void fireTorpedo(Ship& firingShip, sf::Vector2f targetP, int hitChance, float damage, sf::Vector2f parentTip);
        void fireDisruptor(Ship& firingShip, sf::Vector2f targetP, int hitChance, float damage, sf::Vector2f parentTip, sf::Vector2f offset, std::shared_ptr<System> firingSystem);
        void firePhaser(Ship& firingShip, sf::Vector2f targetP, int hitChance, float damage, sf::Vector2f parentTip);
        void fireTorpedoSpread(Ship& firingShip, sf::Vector2f targetP, int hitChance, float damage, sf::Vector2f parentTip);
        void renderProjectiles(); 
    
        void moveProjectiles();
        void updateHitboxes();

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

        void renderMainMenu();

        void logEvent(std::string event, bool friendly);

        void displayEvents();

        void miniTextCreate(std::string text, sf::Vector2f pos);

        void displayMiniText();

        void createDebugBoxes(Ship* enemyShipObj);

        bool checkCollisionRectangleShape(sf::RectangleShape rect, sf::Vector2f vect);

        void checkForMenuItemClicks(std::vector<std::shared_ptr<Button>> menuItems);

        void performMainMenuActions(Button::buttonActions action);

        void generateStars();
        void renderStars();

        sf::Vector2i getChunkNumber(sf::Vector2f& coordinate);
        sf::Vector2i getChunkNumber();

        void renderCoordinates();

        void setGameView(sf::Vector2f viewCoordinates);

        void renderUI();

        void cooldownUI();
        void drawCooldownUI();

        void renderEnemyArrows();

        void placeTriangle(const sf::Vector2f& position, Ship* ship);

        void playSound(std::string filePath);

        void cleanupSounds();

        void toggleMainMenu();
};