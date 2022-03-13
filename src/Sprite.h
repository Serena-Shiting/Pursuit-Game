#pragma once
#include "ofMain.h"
//----------------------------------------------------------------------------------
//
// Basic Sprite Class
//
//
//  Kevin M. Smith - CS 134 SJSU



// This is a base object that all drawable object inherit from
// It is possible this will be replaced by ofNode when we move to 3D
//
typedef enum { autoTurn, Moveing } MoveState;

class Shape {
public:
    Shape() {}
    virtual void draw() {}
    virtual bool inside(glm::vec3 p) {
        return true;
    }

    glm::vec3 pos;
    float rotation = 0.0;    // degrees
    glm::vec3 scale = glm::vec3(1, 1, 1);

    glm::mat4 getMatrix() {
        glm::mat4 trans = glm::translate(glm::mat4(1.0), glm::vec3(pos));
        glm::mat4 rot = glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 0, 1));
        glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(this->scale));
        return (trans * rot * scale);
    }
    
    vector<glm::vec3> verts;
};

class TriangleShape : public Shape {
public:
    TriangleShape() {}
    TriangleShape(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
        verts.push_back(p1);
        verts.push_back(p2);
        verts.push_back(p3);
    }
    bool inside(glm::vec3 p, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
    glm::vec3 heading();
};


class Player : public TriangleShape {
public:
    Player() {}
    Player(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3): TriangleShape(p1, p2, p3){
        nEnergy = 5;
    }
    void draw();
    virtual bool inside(glm::vec3 p);
    bool bShowSprite;
    ofImage playerImage;
    int nEnergy;
    bool useImage;
    bool showHeading;
    float width, height;
};
//+++++++++++++++++++++++++++++++++


//  General Sprite class  (similar to a Particle)
//
class Sprite : public TriangleShape {
public:
	Sprite(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3): TriangleShape(p1, p2, p3){
        speed = 0;
        velocity = ofVec3f(0, 0, 0);
        lifespan = -1;      // lifespan of -1 => immortal
        birthtime = 0;
        haveImage = false;
        moveDir = autoTurn;
    };
    void setPosition(ofVec3f);
	void draw();
	float age();
	void setImage(ofImage);
    void setVelocity(ofVec3f);
	int speed;    //   in pixels/sec
	ofVec3f velocity; // in pixels/sec
    ofVec3f movingOrientation;
	ofImage image;
	float birthtime; // elapsed time in ms
	float lifespan;  //  time in ms
	bool haveImage;
	float width, height;
    MoveState moveDir;
};

//  Manages all Sprites in a system.  You can create multiple systems
//
class SpriteSystem {
public:
	void add(Sprite);
	void remove(int);
	void update();
	int removeNear(Player player);
//    int removeNear(ofVec3f point, float dist);
	void draw();
	vector<Sprite> sprites;
};
