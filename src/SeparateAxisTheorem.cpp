#include "SeparateAxisTheorem.hpp"

std::vector<sf::Vector2f> SATHelper::getPoints(sf::Sprite sprite) {
    sf::FloatRect sizeRect = sprite.getLocalBounds();
    sprite.setOrigin(sizeRect.width / 2, sizeRect.height / 2);
    sf::Vector2f spritePos = sprite.getPosition();
    std::vector<sf::Vector2f> returnVectors;
    float angle = sprite.getRotation() * M_PI / 180;
    float c = cos(angle);
    float s = sin(angle);
    //find the 4 points surrounding our sprite and return as a vector
    returnVectors.push_back(sf::Vector2f(spritePos.x + (sizeRect.width/2) * c - (sizeRect.height/2) * s, 
                    spritePos.y + (sizeRect.width/2) * s + (sizeRect.height/2) * c));
    
    returnVectors.push_back(sf::Vector2f(spritePos.x - (sizeRect.width/2) * c - (sizeRect.height/2) * s, 
                    spritePos.y - (sizeRect.width/2) * s + (sizeRect.height/2) * c));
    
    returnVectors.push_back(sf::Vector2f(spritePos.x - (sizeRect.width/2) * c + (sizeRect.height/2) * s, 
                    spritePos.y - (sizeRect.width/2) * s - (sizeRect.height/2) * c));
    
    returnVectors.push_back(sf::Vector2f(spritePos.x + (sizeRect.width/2) * c + (sizeRect.height/2) * s, 
                    spritePos.y + (sizeRect.width/2) * s - (sizeRect.height/2) * c));

    return returnVectors;
}


//overloaded function to be used on hitboxes like system items.
std::vector<sf::Vector2f> SATHelper::getPoints(sf::RectangleShape shape) {
    sf::FloatRect sizeRect = shape.getLocalBounds();
    shape.setOrigin(sizeRect.width / 2, sizeRect.height / 2);
    sf::Vector2f shapePos = shape.getPosition();
    std::vector<sf::Vector2f> returnVectors;
    float angle = shape.getRotation() * M_PI / 180;
    float c = cos(angle);
    float s = sin(angle);

    returnVectors.push_back(sf::Vector2f(shapePos.x + (sizeRect.width/2) * c - (sizeRect.height/2) * s, 
                    shapePos.y + (sizeRect.width/2) * s + (sizeRect.height/2) * c));
    
    returnVectors.push_back(sf::Vector2f(shapePos.x - (sizeRect.width/2) * c - (sizeRect.height/2) * s, 
                    shapePos.y - (sizeRect.width/2) * s + (sizeRect.height/2) * c));
    
    returnVectors.push_back(sf::Vector2f(shapePos.x - (sizeRect.width/2) * c + (sizeRect.height/2) * s, 
                    shapePos.y - (sizeRect.width/2) * s - (sizeRect.height/2) * c));
    
    returnVectors.push_back(sf::Vector2f(shapePos.x + (sizeRect.width/2) * c + (sizeRect.height/2) * s, 
                    shapePos.y + (sizeRect.width/2) * s - (sizeRect.height/2) * c));

    return returnVectors;
}


std::vector<sf::Vector2f> SATHelper::getAxes(std::vector<sf::Vector2f> pointsA) {
    std::vector<sf::Vector2f> returnVector;
    
    for (int i = 0; i < pointsA.size(); i++) {
        //make a vector given our 2 points joined together.
        sf::Vector2f edgeVector = pointsA.at((i+1) % pointsA.size()) - pointsA.at(i);
        //return the perpendicular axis
        sf::Vector2f normal = sf::Vector2f(-edgeVector.y, edgeVector.x);
        float length = hypot(normal.x, normal.y);
        normal = sf::Vector2f(normal.x / length, normal.y / length);

        returnVector.push_back(normal);
    }

    return returnVector;
}

projection SATHelper::project(std::vector<sf::Vector2f> points, sf::Vector2f axis) {
    projection returnProjection;
    //our min and max shall be the first point for our base case
    std::get<0>(returnProjection) = points.at(0).x * axis.x + points.at(0).y * axis.y;
    std::get<1>(returnProjection) = points.at(0).x * axis.x + points.at(0).y * axis.y;


    for (auto& point: points) {
        float nextProjection = point.x * axis.x + point.y * axis.y;
        if (nextProjection > std::get<1>(returnProjection)) {
            std::get<1>(returnProjection) = nextProjection;
        } else if (nextProjection < std::get<0>(returnProjection)) {
            std::get<0>(returnProjection) = nextProjection;
        }
    }
    
    return returnProjection;
}

bool SATHelper::areProjectionsOverlapping(projection projectionA, projection projectionB) {
    bool result = true;

    if ((std::get<1>(projectionA) <= std::get<0>(projectionB)) || (std::get<1>(projectionB) <= std::get<0>(projectionA)))
		result = false;
    else {
        //std::cout << "Min, Max of projection A: " << std::get<0>(projectionA) << ", " << std::get<1>(projectionA) << std::endl;
        //std::cout << "Min, Max of projection B: " << std::get<0>(projectionB) << ", " << std::get<1>(projectionB) << std::endl;
    }

    return result;
}

bool SATHelper::checkCollision(sf::Sprite spriteA, sf::Sprite spriteB) {

    pointsA = getPoints(spriteA);
    
    pointsB = getPoints(spriteB);
    //program lags intensely to the point of freezing even when axes is empty! where is all that compute going
    axes = getAxes(pointsA);

    for (auto& axis: axes) {
        projection projA = (project(pointsA, axis));
        projection projB = (project(pointsB, axis));
        if (!areProjectionsOverlapping(projA, projB)) {
            return false; // The sprites are not colliding
        }
    }
    //std::cout << "BOOM" << std::endl;
    return true;
}

bool SATHelper::checkCollision(sf::Sprite spriteA, sf::RectangleShape shapeB) {

    pointsA = getPoints(spriteA);
    
    pointsB = getPoints(shapeB);
    
    axes = getAxes(pointsA);

    for (auto& axis: axes) {
        projection projA = (project(pointsA, axis));
        projection projB = (project(pointsB, axis));
        if (!areProjectionsOverlapping(projA, projB)) {
            return false; // The sprites are not colliding
        }
    }
    //std::cout << "BOOM" << std::endl;
    return true;
}

bool SATHelper::checkCollision(sf::RectangleShape shapeA, sf::RectangleShape shapeB) {

    pointsA = getPoints(shapeA);
    
    pointsB = getPoints(shapeB);
    
    axes = getAxes(pointsA);

    for (auto& axis: axes) {
        projection projA = (project(pointsA, axis));
        projection projB = (project(pointsB, axis));
        if (!areProjectionsOverlapping(projA, projB)) {
            return false; // The sprites are not colliding
        }
    }
    //std::cout << "BOOM" << std::endl;
    return true;
}

sf::RectangleShape SATHelper::returnBoundingBox(sf::Sprite sprite) {
    sf::FloatRect sizeRect = sprite.getLocalBounds();
    sf::RectangleShape returnRectangle(sf::Vector2f(sizeRect.width, sizeRect.height));
    returnRectangle.setOrigin(sizeRect.width / 2, sizeRect.height / 2);
    returnRectangle.setRotation(sprite.getRotation());
    returnRectangle.setPosition(sprite.getPosition());

    returnRectangle.setFillColor(sf::Color(255,255,255,0));
    returnRectangle.setOutlineColor(sf::Color(255,0,0,255));
    returnRectangle.setOutlineThickness(1);

    return returnRectangle;
}

//merely for visualization of SAT.
std::vector<sf::RectangleShape> SATHelper::returnNormals(sf::Sprite sprite) {
    std::vector<sf::RectangleShape> returnVector;
    std::vector<sf::Vector2f> points = getPoints(sprite);

    for (int i = 0; i < points.size(); i++) {
        //make a vector given our 2 points joined together.
        sf::Vector2f edgeVector = points.at((i+1) % points.size()) - points.at(i);
        //return the perpendicular axis
        sf::Vector2f normal = sf::Vector2f(-edgeVector.y, edgeVector.x);
        float angle = atan2(normal.y, normal.x) * 180 / M_PI;
        float length = hypot(normal.x, normal.y);
        normal = sf::Vector2f(normal.x / length, normal.y / length);
        sf::Vector2f midpoint = points.at(i) + 0.5f * edgeVector;

        
        sf::RectangleShape returnRectangle(sf::Vector2f(1, hypot(edgeVector.x, edgeVector.y)));
        returnRectangle.setOrigin(0.5, 0);
        returnRectangle.setRotation(angle+90);
        returnRectangle.setPosition(midpoint);

        returnRectangle.setFillColor(sf::Color(255,255,255,0));
        returnRectangle.setOutlineColor(sf::Color(255,0,0,255));
        returnRectangle.setOutlineThickness(1);

        returnVector.push_back(returnRectangle);

    }

    return returnVector;
}

std::vector<sf::RectangleShape> SATHelper::returnPoints(sf::Sprite sprite) {
    std::vector<sf::RectangleShape> returnVector;
    std::vector<sf::Vector2f> points = getPoints(sprite);

    for (int i = 0; i < points.size(); i++) {
        //make a vector given our 2 points joined together.

        sf::RectangleShape returnRectangle(sf::Vector2f(1,1));
        returnRectangle.setOrigin(0.5, 0.5);
        returnRectangle.setPosition(points.at(i));

        returnRectangle.setFillColor(sf::Color(255,255,255,0));
        returnRectangle.setOutlineColor(sf::Color(0,255,0,255));
        returnRectangle.setOutlineThickness(1);

        returnVector.push_back(returnRectangle);

    }

    return returnVector;
}

std::vector<sf::RectangleShape> SATHelper::returnNormals(sf::RectangleShape shape) {
    std::vector<sf::RectangleShape> returnVector;
    std::vector<sf::Vector2f> points = getPoints(shape);

    for (int i = 0; i < points.size(); i++) {
        //make a vector given our 2 points joined together.
        sf::Vector2f edgeVector = points.at((i+1) % points.size()) - points.at(i);
        //return the perpendicular axis
        sf::Vector2f normal = sf::Vector2f(-edgeVector.y, edgeVector.x);
        float angle = atan2(normal.y, normal.x) * 180 / M_PI;
        float length = hypot(normal.x, normal.y);
        normal = sf::Vector2f(normal.x / length, normal.y / length);
        sf::Vector2f midpoint = points.at(i) + 0.5f * edgeVector;

        
        sf::RectangleShape returnRectangle(sf::Vector2f(1, hypot(edgeVector.x, edgeVector.y)));
        returnRectangle.setOrigin(0, 0);
        returnRectangle.setRotation(angle+90);
        returnRectangle.setPosition(midpoint);

        returnRectangle.setFillColor(sf::Color(255,255,255,0));
        returnRectangle.setOutlineColor(sf::Color(255,0,0,255));
        returnRectangle.setOutlineThickness(1);

        returnVector.push_back(returnRectangle);

    }

    return returnVector;
}

std::vector<sf::RectangleShape> SATHelper::returnPoints(sf::RectangleShape shape) {
    std::vector<sf::RectangleShape> returnVector;
    std::vector<sf::Vector2f> points = getPoints(shape);

    for (int i = 0; i < points.size(); i++) {
        //make a vector given our 2 points joined together.

        sf::RectangleShape returnRectangle(sf::Vector2f(1,1));
        returnRectangle.setOrigin(0,0);
        returnRectangle.setPosition(points.at(i));

        returnRectangle.setFillColor(sf::Color(255,255,255,0));
        returnRectangle.setOutlineColor(sf::Color(0,255,0,255));
        returnRectangle.setOutlineThickness(1);

        returnVector.push_back(returnRectangle);

    }

    return returnVector;
}
