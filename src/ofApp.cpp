#include "ofApp.h"
#include "Emitter.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofBackground(0, 0, 0);
//    score = 0;
    
    // Record the time when the game starts
    gameStartTime = ofGetElapsedTimeMillis();
    
    
    
    if (!backgroundImage.load("images/backgroundImag.png")){
        cout << "Can't load image 'backgroundImage.jpeg'" << endl;
        exit();
    }
    
    
    
    if (!playerImage.load("images/airplane.png")){
        cout << "Can't load image 'airplane.png'" << endl;
        exit();
    }
    //player
    player = Player(glm::vec3(-30, 50, 0), glm::vec3(30, 50, 0), glm::vec3(0, -100, 0));
    player.pos = glm::vec3(ofGetWindowWidth()/2.0, ofGetWindowHeight()/2.0, 0);
    player.rotation = 45;
    player.playerImage = playerImage;
    player.width = playerImage.getWidth();
    player.height =playerImage.getHeight();
    
    

    
    gui.setup();
    gui.add(playerSpeed.setup("player Speed", 3, 0, 5));
    gui.add(useImage.setup("Use image", true));
    gui.add(showHeading.setup("Draw Heading", true));
    gui.add(playerScale.setup("Scale", 0.1, 0.05, 0.4));
    gui.add(numEnergy.setup("Energy level", 5, 3, 10));
    gui.add(nAgents.setup("Number of Agents", 2, 1, 10));
    gui.add(lifespan.setup("lifespan", 10000, 6000, 20000));
    gui.add(spriteSpeed.setup("sprite Speed", 2, 0, 5));
    gui.add(rate.setup("rate", 3, 0.5, 10));

    
    bHide = false;
    
    

    // Create and setup emitters
    invaders = new Emitter(new SpriteSystem());
    
    //setup image for invaders
    if (!(spriteImage.load("images/rocket.png"))){
        cout << "Can't load image 'rocket.png'" << endl;
        exit();
    }
    invaders->setChildImage(spriteImage);
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
//====== PLAYER update: =========
        
    //Control from the GUI: scale
    float s = playerScale;
    player.scale = glm::vec3(s, s, s);
    
    //Control from the GUI: use image or not
    player.bShowSprite = useImage;
    player.showHeading = showHeading;
    
    
    //Control from the GUI: speed
    float moveSpeed = playerSpeed;
    
    //Control from the GUI: nEnergy
    if(invaders->started == false){
        int energy = numEnergy;
        player.nEnergy = energy;
        currentLevel = energy;
    }
    //Control from the GUI: nAgents
    int numOfAgents = nAgents;
    invaders->numAgents = numOfAgents;
    
    //Control from the GUI: lifeSpan
    int lifeSpan = lifespan;
    invaders->setLifespan(lifeSpan);
    
    //Control from the GUI: rate
    int r = rate;
    invaders->setRate(r);
    
    if (keymap[OF_KEY_LEFT] ){
         player.rotation -= 1.5;
     }
    if (keymap[OF_KEY_RIGHT] ){
        player.rotation += 1.5;
    }
    if (keymap[OF_KEY_UP] ){
        //move forward to the new heading direction
        player.pos = player.pos + player.heading() * moveSpeed;
    }
    if (keymap[OF_KEY_DOWN] ){
        player.pos = player.pos - player.heading() * moveSpeed;
    }
    if(keymap[' ']){
        // start the game
        gameOver = false;
        invaders->start();
        
    }
    if(keymap['F'] || keymap['f']){
        ofToggleFullscreen();
    }
    
    // update player's position
    if(player.pos.x > ofGetWindowWidth()){
        player.pos.x = player.pos.x - ofGetWindowWidth();
    }else if(player.pos.x < 0){
        player.pos.x = player.pos.x + ofGetWindowWidth();
    }
    if(player.pos.y > ofGetWindowHeight()){
        player.pos.y = player.pos.y - ofGetWindowHeight();
    }else if(player.pos.y < 0){
        player.pos.y = player.pos.y + ofGetWindowHeight();
    }
    
//====== SPRITES update: =========
    
//      for sprite in invaders->sys->sprites{
//      update each sprites' direction and velocity
    for(int i=0; i < invaders->sys->sprites.size();i++){
        
        ofVec3f direction = glm::vec3(player.pos.x, player.pos.y, 0) - glm::vec3(invaders->sys->sprites[i].pos.x, invaders->sys->sprites[i].pos.y, 0);
        
        ofVec3f orientation = glm::normalize(glm::vec3(direction));
        invaders->sys->sprites[i].movingOrientation = orientation;

        ofVec3f sHeading = invaders->sys->sprites[i].heading();
        
        float dot_product = sHeading.x * orientation.x + sHeading.y * orientation.y;
        float cross_product = sHeading.x * orientation.y - sHeading.y * orientation.x;
        float rotationAmount = atan2(cross_product, dot_product) * (180.0/3.141592653589793238463);
        
        invaders->sys->sprites[i].rotation += rotationAmount/ofGetFrameRate();
        
        //---autoTurn state---
        if(invaders->sys->sprites[i].moveDir == autoTurn){
            
            if(rotationAmount < 1){
                invaders->sys->sprites[i].moveDir = Moveing;
                invaders->sys->sprites[i].speed = spriteSpeed;
            }
        }else{
        //---Moveing state---
            ofVec3f velocity = direction * invaders->sys->sprites[i].speed;
            invaders->sys->sprites[i].setVelocity(velocity);
        }
    }
    
    invaders->update();
    

    // check for collisions between player and invaders
    checkCollisions();

    
    //if player lose all the energy, game over
    if(currentLevel <= 0){
        gameOver = true;
        invaders->stop();
        setup();
    }
    
    
    //handle game time:
    float t = ofGetElapsedTimeMillis();
    //Only when game started, total game time starts to accumulate
    if(invaders->started == true){
        totalGameTime = t - gameStartTime;
    }else{
        gameStartTime = t;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    backgroundImage.draw(0,0,ofGetWindowWidth(), ofGetWindowHeight());
    
    player.draw();
    
    if(!bHide){
        gui.draw();
    }
    
    // ---------Display Text----------
    //
    string scoreText;
    int realTime = totalGameTime/1000;
    scoreText += "Press space to start the game!\n";
    ofDrawBitmapString(scoreText, ofPoint(250, 20));
    
    if (gameOver) {
        ofDrawBitmapString("GAME OVER", ofPoint(ofGetWindowWidth() / 2, ofGetWindowHeight() / 2));
    }
    else {
        invaders->draw();
    }
    
    string gameInfo;
    if(gameOver){
        gameInfo += "Current player energy: 0\n";
    }else{
        gameInfo += "Current player energy: " + std::to_string(currentLevel) + "/" + std::to_string(player.nEnergy) + "\n";
    }
    gameInfo += "Total elapsed time: " + std::to_string(realTime) + " s\n";
    gameInfo += "Frame rate: " + std::to_string(ofGetFrameRate());
    ofDrawBitmapString(gameInfo, ofPoint(ofGetWindowWidth()-250, 20));
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    keymap[key] = true;
    if(key == 'h'){
        bHide = !bHide;
    }
    
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    keymap[key] = false;
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    //when mouse draging inside the triangle, the trianle should
    //change its pos to where the mouse position is
    if(mouseDrag){
        player.pos = glm::vec3(ofGetMouseX(), ofGetMouseY(), 1.0);
        player.pos -= distanceToCenter;
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    mousePos = glm::vec3(ofGetMouseX(), ofGetMouseY(), 1.0);

    mouseDrag = player.inside(mousePos);
    if(mouseDrag){
        distanceToCenter = mousePos - player.pos;
    }
    if (!invaders->started) {
        gameStartTime = ofGetElapsedTimeMillis();
        gameOver = false;
        invaders->start();
        totalGameTime = 0;
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    mouseDrag = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

void ofApp::checkCollisions() {

    // find the distance at which the two sprites (missles and invaders) will collide
    // detect a collision when we are within that distance.
    //
    float collisionDist = 10;

    // the removeNear() function returns the number of missiles removed.
    //
    for (int i = 0; i < invaders->sys->sprites.size(); i++){
        int removeNum;
        removeNum = invaders->sys->removeNear(player);
        if(removeNum){
//            player.nEnergy -= 1;
            currentLevel -= 1;
        }
    }
}
//void ofApp::checkCollisions() {
//
//    // find the distance at which the two sprites (missles and invaders) will collide
//    // detect a collision when we are within that distance.
//    //
//    float collisionDist = player.width / 2 + invaders->width / 2;
//
//    // the removeNear() function returns the number of missiles removed.
//    //
//    for (int i = 0; i < invaders->sys->sprites.size(); i++){
//        int removeNum;
//        removeNum = invaders->sys->removeNear(player.pos, collisionDist);
//        if(removeNum){
////            player.nEnergy -= 1;
//            currentLevel -= 1;
//        }
//    }
//}
