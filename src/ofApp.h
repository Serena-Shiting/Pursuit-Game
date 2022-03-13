#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Sprite.h"


class Emitter;
class Player;


class ofApp : public ofBaseApp{

    public:
        void setup();
        void update();
        void draw();
    void checkCollisions();

        void keyPressed(int key);
        void keyReleased(int key);
        void mouseMoved(int x, int y );
        void mouseDragged(int x, int y, int button);
        void mousePressed(int x, int y, int button);
        void mouseReleased(int x, int y, int button);
        void mouseEntered(int x, int y);
        void mouseExited(int x, int y);
        void windowResized(int w, int h);
        void dragEvent(ofDragInfo dragInfo);
        void gotMessage(ofMessage msg);

        Player player;
    
        bool mouseDrag = false;
        
        glm::vec3 mousePos;
        glm::vec3 distanceToCenter;
        map<int, bool> keymap;
    
        
    ofxPanel gui;
    ofxFloatSlider playerSpeed;
    ofxToggle useImage;
    ofxToggle showHeading;
    ofxFloatSlider playerScale;
    ofxIntSlider numEnergy;
    ofxIntSlider nAgents;
    ofxIntSlider lifespan;
    ofxIntSlider spriteSpeed;
    ofxFloatSlider rate;
    
    bool bHide;
    
    ofImage playerImage;
    ofImage spriteImage;
    ofImage backgroundImage;
    int imageWidth;
    int imageHeight;
    
    Emitter *invaders;
    int score;
    float gameStartTime;
    bool gameOver = false;
    ofVec3f mouseLast;
    float totalGameTime;
    int currentLevel;
};
