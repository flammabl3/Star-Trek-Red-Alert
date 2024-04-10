#include "SeparateAxisTheorem.hpp"

std::vector<sf::Vector2f> SATHelper::getPoints(sf::Sprite sprite) {
    
    sf::FloatRect sizeRect = sprite.getLocalBounds();
    sprite.setOrigin(sizeRect.width / 2, sizeRect.height / 2);
    sf::Vector2f spritePos = sprite.getPosition();
    std::vector<sf::Vector2f> returnVectors;

    //find the 4 points surrounding our sprite and return as a vector
    returnVectors.push_back(sf::Vector2f(spritePos.x + (sizeRect.width/2) * cos(sprite.getRotation()) - (sizeRect.height/2) * sin(sprite.getRotation()), 
                    spritePos.y + (sizeRect.width/2) * cos(sprite.getRotation()) + (sizeRect.height/2) * sin(sprite.getRotation())));
    
    returnVectors.push_back(sf::Vector2f(spritePos.x - (sizeRect.width/2) * cos(sprite.getRotation()) - (sizeRect.height/2) * sin(sprite.getRotation()), 
                    spritePos.y + (sizeRect.width/2) * cos(sprite.getRotation()) - (sizeRect.height/2) * sin(sprite.getRotation())));
    
    returnVectors.push_back(sf::Vector2f(spritePos.x - (sizeRect.width/2) * cos(sprite.getRotation()) - (sizeRect.height/2) * sin(sprite.getRotation()), 
                    spritePos.y - (sizeRect.width/2) * cos(sprite.getRotation()) - (sizeRect.height/2) * sin(sprite.getRotation())));
    
    returnVectors.push_back(sf::Vector2f(spritePos.x + (sizeRect.width/2) * cos(sprite.getRotation()) - (sizeRect.height/2) * sin(sprite.getRotation()), 
                    spritePos.y + (sizeRect.width/2) * cos(sprite.getRotation()) - (sizeRect.height/2) * sin(sprite.getRotation())));

    return returnVectors;
}

//overloaded function to be used on hitboxes like system items.
std::vector<sf::Vector2f> SATHelper::getPoints(sf::RectangleShape shape) {
    sf::FloatRect sizeRect = shape.getLocalBounds();
    shape.setOrigin(sizeRect.width / 2, sizeRect.height / 2);
    sf::Vector2f shapePos = shape.getPosition();
    std::vector<sf::Vector2f> returnVectors;

    returnVectors.push_back(sf::Vector2f(shapePos.x + (sizeRect.width/2) * cos(shape.getRotation()) - (sizeRect.height/2) * sin(shape.getRotation()), 
                    shapePos.y + (sizeRect.width/2) * cos(shape.getRotation()) + (sizeRect.height/2) * sin(shape.getRotation())));
    
    returnVectors.push_back(sf::Vector2f(shapePos.x - (sizeRect.width/2) * cos(shape.getRotation()) - (sizeRect.height/2) * sin(shape.getRotation()), 
                    shapePos.y + (sizeRect.width/2) * cos(shape.getRotation()) - (sizeRect.height/2) * sin(shape.getRotation())));
    
    returnVectors.push_back(sf::Vector2f(shapePos.x - (sizeRect.width/2) * cos(shape.getRotation()) - (sizeRect.height/2) * sin(shape.getRotation()), 
                    shapePos.y - (sizeRect.width/2) * cos(shape.getRotation()) - (sizeRect.height/2) * sin(shape.getRotation())));
    
    returnVectors.push_back(sf::Vector2f(shapePos.x + (sizeRect.width/2) * cos(shape.getRotation()) - (sizeRect.height/2) * sin(shape.getRotation()), 
                    shapePos.y + (sizeRect.width/2) * cos(shape.getRotation()) - (sizeRect.height/2) * sin(shape.getRotation())));

    return returnVectors;
}

std::vector<sf::Vector2f> SATHelper::getAxes(std::vector<sf::Vector2f> pointsA, std::vector<sf::Vector2f> pointsB) {
    std::vector<sf::Vector2f> returnVector;
    
    for (int i = 0; i < pointsA.size() - 1; i++) {
        //make a vector given our 2 points joined together.
        sf::Vector2f edgeVector = pointsA.at(i+1) - pointsA.at(i);
        //return the perpendicular axis
        sf::Vector2f normal = sf::Vector2f(-edgeVector.y, edgeVector.x);
        
        returnVector.push_back(normal);

    }

    for (int i = 0; i < pointsB.size() - 1; i++) {
        sf::Vector2f edgeVector = pointsB.at(i+1) - pointsB.at(i);
        sf::Vector2f normal = sf::Vector2f(-edgeVector.y, edgeVector.x);
        
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
        std::cout << "Min, Max of projection A: " << std::get<0>(projectionA) << ", " << std::get<1>(projectionA) << std::endl;
        std::cout << "Min, Max of projection B: " << std::get<0>(projectionB) << ", " << std::get<1>(projectionB) << std::endl;
    }

    return result;
}

bool SATHelper::checkCollision(sf::Sprite spriteA, sf::Sprite spriteB) {
    bool result = false;

    pointsA = getPoints(spriteA);
    pointsB = getPoints(spriteB);

    axes = getAxes(pointsA, pointsB);
    for (auto& axis: axes) {
        projections.push_back(project(pointsA, axis));
        projections.push_back(project(pointsB, axis));
    }

    for (int i = 0; i < projections.size() - 1; i++) {
        for (int j = i + 1; j < projections.size(); j++) {
            if (areProjectionsOverlapping(projections.at(i), projections.at(j))) {
                result = true;
            }
        }
    }

    return result;
}

bool SATHelper::checkCollision(sf::Sprite spriteA, sf::RectangleShape shapeB) {
    bool result = false;

    pointsA = getPoints(spriteA);
    pointsB = getPoints(shapeB);

    axes = getAxes(pointsA, pointsB);
    for (auto& axis: axes) {
        projections.push_back(project(pointsA, axis));
        projections.push_back(project(pointsB, axis));
    }

    for (int i = 0; i < projections.size() - 1; i++) {
        for (int j = i + 1; j < projections.size(); j++) {
            if (areProjectionsOverlapping(projections.at(i), projections.at(j))) {
                result = true;
            }
        }
    }

    return result;
}
