#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    
    makeBunchOfRandomPoints();
#ifdef pts3D
    
    simplex.hullMode = ofxQuickHull::HULL_TRIANGLES;
    simplex.init(pts, &hullPts);
#else
    hullPts = ofxQuickHull::Hull2D::getHull(pts);
#endif
    makeVbo(mshHull, hullPts, simplex.hullMode==ofxQuickHull::HULL_PT_CLOUD?OF_PRIMITIVE_POINTS:OF_PRIMITIVE_TRIANGLES);
}

void ofApp::makeBunchOfRandomPoints(){
    const int aBunch = 100;
    const ofVec3f bBox(ofGetWidth()/2, ofGetHeight()/2, ofGetWidth()/2);
#ifdef pts3D
    pts.resize(aBunch);
    for(int i=0;i<aBunch;i++){
        pts[i].set(ofRandom(-bBox.x,bBox.x),ofRandom(-bBox.y,bBox.y),ofRandom(-bBox.z,bBox.z));
    }
#else
    pts.resize(aBunch);
    for(int i=0;i<aBunch;i++){
        pts[i].set(bBox.x+ofRandom(-bBox.x/2,bBox.x/2),bBox.y+ofRandom(-bBox.y/2,bBox.y/2));
    }
#endif
    
    makeVbo(msh, pts, OF_PRIMITIVE_POINTS);
}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
#ifdef pts3D
    cam.begin();
#endif
    ofPushStyle();
    glPointSize(5);
    ofSetColor(255, 255, 255, 100);
    msh.draw();
    glPointSize(20);
    ofSetColor(0, 255, 0, 70);
    ofSetLineWidth(2);
    mshHull.draw();
    ofSetColor(0, 255, 0, 100);
    mshHull.drawWireframe();
    ofPopStyle();
#ifdef pts3D
    ofSetLineWidth(3);
    simplex.draw();
    ofSetLineWidth(1);
    cam.end();
#endif
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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
