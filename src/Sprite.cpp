#include "Sprite.h"
#include "ofApp.h"


//--------------------------------------------------------------
//TriangleShape
//--------------------------------------------------------------

bool TriangleShape::inside(glm::vec3 p, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    glm::vec3 v1 = glm::normalize(p1 - p);
    glm::vec3 v2 = glm::normalize(p2 - p);
    glm::vec3 v3 = glm::normalize(p3 - p);
    float a1 = glm::orientedAngle(v1, v2, glm::vec3(0, 0, 1));
    float a2 = glm::orientedAngle(v2, v3, glm::vec3(0, 0, 1));
    float a3 = glm::orientedAngle(v3, v1, glm::vec3(0, 0, 1));
    if (a1 < 0 && a2 < 0 && a3 < 0) return true;
    else return false;
}

glm::vec3 TriangleShape::heading(){
    float angle = glm::radians(rotation);
    glm::vec3 heading = glm::normalize(glm::vec3(glm::sin(angle), -glm::cos(angle), 0));
    return heading;
}

//--------------------------------------------------------------
//Player
//--------------------------------------------------------------

void Player::draw(){
    ofFill();
    ofPushMatrix();
    ofMultMatrix(getMatrix());
    ofDrawTriangle(verts[0], verts[1], verts[2]);
    ofPopMatrix();
    
    if(showHeading){
        ofSetColor(ofColor::whiteSmoke);
        ofDrawLine(pos, pos+ heading() * 75);
    }

    //draw image
    if(bShowSprite){
        ofPushMatrix();
        ofMultMatrix(getMatrix());
        playerImage.draw(-width/2, -height/2);
        ofPopMatrix();
    }
}

//
// inside - check if point is inside player (can be an image or TriangleShape if no image)
bool Player::inside(const glm::vec3 p) {
    
    glm::vec3 s = glm::inverse(getMatrix()) * glm::vec4(p, 1);
    
    // if there is no sprite image attached, then just use triangle case.
    //
    if (!bShowSprite) {
        return TriangleShape::inside(s, verts[0], verts[1], verts[2]);
    }
    
    // if sprite image attached, then first check if point is inside bounds of image
    // in object space.  If point is inside bounds, then make sure the point is in
    // opaque part of image.
    int w = width;
    int h = height;
    if (s.x > -w / 2 && s.x < w / 2 && s.y > -h / 2 && s.y < h / 2) {
        int x = s.x + w / 2;
        int y = s.y + h / 2;
        ofColor color = playerImage.getColor(x, y);
        return (color.a != 0);   // check if color is opaque (not the transparent background)
    }else return false;
}


//--------------------------------------------------------------
//Sprite
//--------------------------------------------------------------

void Sprite::setPosition(ofVec3f p) {
	pos = p;
}
void Sprite::setVelocity(ofVec3f v) {
    velocity = v;
}


// Return a sprite's age in milliseconds
//
float Sprite::age() {
	return (ofGetElapsedTimeMillis() - birthtime);
}

//  Set an image for the sprite. If you don't set one, a rectangle
//  gets drawn.
//
void Sprite::setImage(ofImage img) {
	image = img;
	haveImage = true;
	width = image.getWidth();
	height = image.getHeight();
}


//  Render the sprite
//
void Sprite::draw() {

	ofSetColor(255, 255, 255, 255);

	// draw image centered and add in translation amount
	//
	if (haveImage) {
        ofPushMatrix();
        ofMultMatrix(getMatrix());
		image.draw(-width / 2.0 + pos.x, -height / 2.0 + pos.y);
        ofPopMatrix();
	}
	else {
		// in case no image is supplied, draw something.
		// 
		ofNoFill();
		ofSetColor(255, 0, 0);

        ofPushMatrix();
        ofMultMatrix(getMatrix());
        ofDrawTriangle(verts[0], verts[1], verts[2]);
        ofPopMatrix();
	}
}

//  Add a Sprite to the Sprite System
//
void SpriteSystem::add(Sprite s) {
	sprites.push_back(s);
}

// Remove a sprite from the sprite system. Note that this function is not currently
// used. The typical case is that sprites automatically get removed when the reach
// their lifespan.
//
void SpriteSystem::remove(int i) {
	sprites.erase(sprites.begin() + i);
}


//  Update the SpriteSystem by checking which sprites have exceeded their
//  lifespan (and deleting).  Also the sprite is moved to it's next
//  location based on velocity and direction.
//
void SpriteSystem::update() {

	if (sprites.size() == 0) return;
	vector<Sprite>::iterator s = sprites.begin();
	vector<Sprite>::iterator tmp;

	// check which sprites have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, use an iterator.
	//
	while (s != sprites.end()) {
		if (s->lifespan != -1 && s->age() > s->lifespan) {
			tmp = sprites.erase(s);
			s = tmp;
		}
		else s++;
	}

	//  Move enemy
	//
	for (int i = 0; i < sprites.size(); i++) {
        if(sprites[i].moveDir == Moveing){
            sprites[i].pos += sprites[i].movingOrientation *  sprites[i].speed;
        }else{
            sprites[i].pos += sprites[i].velocity / ofGetFrameRate();
        }
	}
}

//  Render all the sprites
//
void SpriteSystem::draw() {
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].draw();
	}
}

// remove all sprites within a given dist of point, return number removed
//
int SpriteSystem::removeNear(Player player) {
    vector<Sprite>::iterator s = sprites.begin();
    vector<Sprite>::iterator tmp;
    int count = 0;

    while (s != sprites.end()) {
//        ofVec3f v = s->pos - point;

        if (player.inside(glm::vec3(s->pos.x, s->pos.y, 1))) {
            tmp = sprites.erase(s);
            count++;
            s = tmp;
        }
        else s++;
    }
    return count;
}

//int SpriteSystem::removeNear(ofVec3f point, float dist) {
//    vector<Sprite>::iterator s = sprites.begin();
//    vector<Sprite>::iterator tmp;
//    int count = 0;
//
//    while (s != sprites.end()) {
//        ofVec3f v = s->pos - point;
//        if (v.length() < dist) {
//            tmp = sprites.erase(s);
//            count++;
//            s = tmp;
//        }
//        else s++;
//    }
//    return count;
//}
