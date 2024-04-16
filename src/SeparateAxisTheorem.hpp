#include <SFML/Graphics.hpp>
#include <vector>
#include <tuple>
#include <cmath>
#include <iostream>

//The pair of vectors will form a line (the sf::Vector2f).

//2 floats to show min and max
typedef std::tuple<float, float> projection;

class SATHelper {

    //functions should be created to define a bounding box around an sf::Sprite. 

    /*
    Vertex 1: (x + w/2 cos(θ) - l/2 sin(θ), y + w/2 sin(θ) + l/2 cos(θ))
    Vertex 2: (x - w/2 cos(θ) - l/2 sin(θ), y - w/2 sin(θ) + l/2 cos(θ))
    Vertex 3: (x - w/2 cos(θ) + l/2 sin(θ), y - w/2 sin(θ) - l/2 cos(θ))
    Vertex 4: (x + w/2 cos(θ) + l/2 sin(θ), y + w/2 sin(θ) - l/2 cos(θ))
    */

    public:
        //SATHelper();

        std::vector<sf::Vector2f> pointsA;
        std::vector<sf::Vector2f> pointsB;

        //A vector of paired sf::Vector2f objects. The pair of vectors will form a line (the sf::Vector2f).
        std::vector<sf::Vector2f> axes;
        
        std::vector<projection> projections;

        // use the trigonometry above to find the 4 points around an sfml sprite.
        std::vector<sf::Vector2f> getPoints(sf::Sprite sprite);
        std::vector<sf::Vector2f> getPoints(sf::RectangleShape shape);

        // get the axes parallel to the edges of the rect 
        std::vector<sf::Vector2f> getAxes(std::vector<sf::Vector2f> pointsA);

        //project points onto the target sf::Vector2f.
        projection project(std::vector<sf::Vector2f> points, sf::Vector2f axis);

        //check if 2 shapes' projections onto the same sf::Vector2f overlap.
        bool areProjectionsOverlapping(projection projectionA, projection projectionB);

        /*use all functions above to see if 2 shapes overlap.
        
        */ 
        bool checkCollision(sf::Sprite spriteA, sf::Sprite spriteB);

        bool checkCollision(sf::Sprite spriteA, sf::RectangleShape shapeB);

        sf::RectangleShape returnBoundingBox(sf::Sprite sprite);
        std::vector<sf::RectangleShape> returnNormals(sf::Sprite sprite);
        sf::RectangleShape returnProjections(sf::Sprite sprite);
        std::vector<sf::RectangleShape> returnPoints(sf::Sprite sprite);

    private:

};