#include "ofApp.h"

#include <iostream>
using namespace std;		// this is so we can use cout

#define MAX_AGENTS 100		// how many agents we can have max
agent Agents[MAX_AGENTS];	// storage (memory) space for all the unique details of every agent


// constructor
agent::agent(){
    velDir.set(0, ofRandom(.5, 2));	// init random speed
    velDir.rotate(ofRandom(360));		// init random direction
    
    mouse.set(0,0);
    gravity.set(0,1);
}

// update all variables in an agent
void agent::update(agent *arr){  // influence the main vector
    ofVec2f noVec(-1,-1);
    
    if (!active)
        return;
    
    // Where are agents near you headed? Align with that.
    float alignDelta = calcAlignment(arr, MAX_AGENTS);	// set up aligment
    velDir.rotate(alignDelta*.05);			// apply alignment delta (weighted)
    
    /*
     set mouse gravity here
     weight by .06
     }
     */
    
    mouse += gravity*.06;
    
    
    // For agents nearby, get centroid, head that way
    ofVec2f middle = calcCohesion(arr, MAX_AGENTS);	// get centroid of nearby mass of agents
    if (middle != noVec) {
        float alignMouse = velDir.angle(middle - location);	// delta between centroid & location gives us a VECTOR
        velDir.rotate(alignMouse*.02);			// apply centroid gravity (weighted)
    }
    
    // For too-close agents try to move away from them
    ofVec2f tooClose = calcDispersion(arr, MAX_AGENTS);			// get avg heading of too-close agents
    if (tooClose != noVec) {
        float tooCloseDelta;
        if (velDir.angle((tooClose - location)) < 0) 			// if it is on our left side
            tooCloseDelta = velDir.angle((tooClose - location).rotate(90));	// move opposite to mass
        
        else
            tooCloseDelta = velDir.angle((tooClose - location).rotate(-90));  // move opposite to mass
        velDir.rotate(tooCloseDelta*.03);					// apply centroid gravity (weighted)
        
    }
    
    location += velDir;	// move to current location
    
    if(ofGetMousePressed()){
        location -= velDir;
    }
}

void agent::draw(){
    if (!active)  		// if not active, do nothing
        return;
    
    ofVec2f top(0,-1);		// top of origin used as control
    ofVec2f p1(6.25,  12);	// triangle pointing up w middle at origin
    ofVec2f p2(0, -12);
    ofVec2f p3(-6.25,  12);
    
    ofVec2f top2(0,-1);
    ofVec2f p4(3,  6);	// triangle pointing up w middle at origin
    ofVec2f p5(0, -6);
    ofVec2f p6(-3,  6);
    
    float angle = top.angle(velDir);	// get direction angle
    float angle2 = top2.angle(velDir*0.6);
    
    p1.rotate(angle);	// point in direction of movement
    p2.rotate(angle);
    p3.rotate(angle);
    p4.rotate(angle2);	// point in direction of movement
    p5.rotate(angle2);
    p6.rotate(angle2);
    
    // if it moves off the board, move it to opposite side
    if (location.x > ofGetWidth()) location.x = 0;
    if (location.x < 0) location.x = ofGetWidth();
    if (location.y > ofGetHeight()) location.y = 0;
    if (location.y < 0) location.y = ofGetHeight();
    
    p1 += location;	// move to current location
    p2 += location;
    p3 += location;
    p4 += location;	// move to current location
    p5 += location;
    p6 += location;
    
    if( location.x>0 && location.x<512){
        //ofSetColor(238,233,59);
        ofDrawTriangle(p4, p5, p6);
    } else {
        ofDrawTriangle(p1, p2, p3);
    }
    int alpha = 150;
    ofSetColor(70, 120, 200,alpha);
    ofDrawLine(512,0,512,768);
    ofSetLineWidth(1);
    
    ofSetColor(225,20,0);
    if(!ofGetMousePressed()){
        ofDrawCircle(mouse, 10);
    }
    
}

void agent::start(float x, float y){
    active = true; // turn this one on!
    location.set(x,y);
    
    
}


// Get relative angle from me -180 - 180
float agent::getRelAngle(ofVec2f neighbor){
    float angle = velDir.angle(neighbor);
    return (angle);
}


// get mouse location for gravity
void agent::calcGravityWell(float x,float y){
    mouse.set(x,y);
}

// average & return neighbor's VECTORS
float agent::calcAlignment(agent *arr, int max){
    float sum = 0;  // sum of angles
    int sumNum = 0; // num of angles summed
    
    for (int i=0; i < max; i++) {
        float distance = location.distance(arr[i].location);
        if (distance < 150 && distance > 0 && arr[i].active) { 	// if a neighbor is close & active
            //cout  << " Dist: " << distance << "\n";
            sum += getRelAngle(arr[i].velDir);			// add its angle to sum
            sumNum++;							// count how many are summed
        }
    }
    if (sumNum > 0) {   // avoid div by 0
        return(sum/sumNum);
    } else
        return(0);
}

// average and return neighbor's LOCATIONS
ofVec2f agent::calcCohesion(agent *arr, int max){
    ofVec2f centroid, nearArr[MAX_AGENTS];				// centroid & arr of Neighbors near us
    int lenArr = 0;
    
    for (int i=0; i < max; i++) {
        float distance = location.distance(arr[i].location);
        
        if (distance < 70 && distance > 20 && arr[i].active) { 	// if a neighbor is close & active
            nearArr[lenArr] = arr[i].location;			// add its location to array
            lenArr++;// count how many are added
            
        }
    }
    
    if (lenArr > 0) {
        centroid.average(nearArr, lenArr);	// average the location of all neighbor agents
        return (centroid);
    } else {
        ofVec2f noVec(-1,-1);
        return (noVec);  			// no nearby agents
    }
}

// average and return too-close neighbor's LOCATIONS
ofVec2f agent::calcDispersion(agent *arr, int max){
    ofVec2f centroid, nearArr[MAX_AGENTS];		// centroid & arr of Neighbors near us
    int lenArr = 0;
    
    for (int i=0; i < max; i++) {
        float distance = location.distance(arr[i].location);
        
        if (distance < 20 && arr[i].active) { 	// if a neighbor is close & active
            nearArr[lenArr] = arr[i].location;	// add its location to array
            lenArr++;					// count how many are added
            
        }
    }
    
    if (lenArr > 0) {
        centroid.average(nearArr, lenArr);		// average the location of all too-close neighbor agents
        return (centroid);
    } else {
        ofVec2f noVec(-1,-1);
        return (noVec);				// no nearby agents
    }
}




int curAgent = 0;			// what agent we are dealing with

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowShape(1024,768);
    ofSetFrameRate(60); // Limit the speed of our program to 60 frames per second
    ofBackground(0);
    
    for (curAgent; curAgent < MAX_AGENTS; curAgent++) {  // only enable if we disable mouse gravity
        Agents[curAgent].start(ofRandom(ofGetWidth()),ofRandom(ofGetHeight()));
        
    }
    

}

//--------------------------------------------------------------
void ofApp::update(){
    for (int i=0; i < MAX_AGENTS; i++) {
        Agents[i].update(Agents);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    int alpha = 150;
    
    ofColor aqua(0, 252, 255, alpha);
    ofColor purple(198, 0, 205, alpha);
    ofColor inbetween = aqua.getLerped(purple, ofRandom(1.0));  // linear interpolation between colors color
    
    ofColor yellow(255, 252, 5, alpha);
    ofColor green(113, 213, 76, alpha);
    ofColor inbetween2 = yellow.getLerped(green, ofRandom(1.0));
    
    
    for (int i=0; i < MAX_AGENTS; i++) {
        if(!ofGetKeyPressed()){
            
            ofSetColor(inbetween);
            Agents[i].draw();
            
        } else if(ofGetKeyPressed()){
            ofSetColor(inbetween2);
            Agents[i].draw();
        }
        
        if(ofGetMousePressed()){
            ofSetColor(221,126,152);
            Agents[i].draw();
        }
        
    }
    
    if(ofGetMousePressed()){
        ofDrawCircle(ballx, bally, 10);
    }
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    for (int i=0; i < MAX_AGENTS; i++) {
        Agents[i].calcGravityWell(x,y);
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    Agents[curAgent].start(x, y);
    curAgent++;
    
    if (curAgent >= MAX_AGENTS) {  // make sure it does not go over
        curAgent = 0;
    }
    ballx=ofRandom(0,ofGetWindowWidth());
    bally=ofRandom(0,ofGetWindowHeight());
    
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
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
