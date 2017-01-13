#pragma once

#include "ofMain.h"
#include "ofxQuickHull.h"

#define pts3D

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

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

    void makeBunchOfRandomPoints();
#ifdef pts3D
    void makeVbo(ofVboMesh & vbo, vector<ofVec3f> & pts, ofPrimitiveMode pm){
        vbo.clear();
        vbo.setUsage(GL_STATIC_DRAW);
        vbo.setMode(pm);
        vbo.addVertices(pts);
    }
#else
    void makeVbo(ofVboMesh & vbo, vector<ofVec2f> & pts, ofPrimitiveMode pm){
        vbo.clear();
        vbo.setUsage(GL_STATIC_DRAW);
        vbo.setMode(pm);
        for(int i=0;i<pts.size();i++){
            vbo.addVertex(ofVec3f(pts[i].x, pts[i].y,0));
        }
    }
#endif
    
    ofEasyCam cam;
    ofVboMesh msh;
    ofVboMesh mshHull;
#ifdef pts3D
    vector<ofVec3f> pts;
    vector<ofVec3f> hullPts;
    ofxQuickHull::Simplex simplex;
#else
    vector<ofVec2f> pts;
    vector<ofVec2f> hullPts;
#endif
};
