//
//  ofxQuickHull.cpp
//
//  magicLab 2017
//  http://www.magiclab.nyc
//  Created by enrico<nausea>viola on 1/10/17.
//

#include "ofxQuickHull.h"

vector<ofVec2f> ofxQuickHull::Hull2D::getHull(vector<ofVec2f> & pts){
    vector<ofVec2f> hull;
    ofxQuickHull::Hull2D::ExtremePoints2D ep = ofxQuickHull::Hull2D::getExtremePoints(pts);
    ep.appendPoints(hull);
    
    return hull;
}

ofxQuickHull::Hull2D::ExtremePoints2D ofxQuickHull::Hull2D::getExtremePoints(vector<ofVec2f> & pts){
    ofxQuickHull::Hull2D::ExtremePoints2D ep;
    if(pts.size()>0){
        ep.set(pts, 0);
        if(pts.size()>1){
            for(int i=1;i<pts.size();i++){
                ep.test(pts, i);
            }
        }
    }
    return ep;
}

vector<ofVec3f> ofxQuickHull::Hull3D::getHull(vector<ofVec3f> &pts){
    vector<ofVec3f> hull;
    ofxQuickHull::Simplex simplex(pts, &hull);
    
    return hull;
}

ofxQuickHull::Hull3D::ExtremePoints3D ofxQuickHull::Hull3D::getExtremePoints(vector<ofVec3f> &pts){
    ofxQuickHull::Hull3D::ExtremePoints3D ep;
    if(pts.size()>0){
        ep.set(pts, 0);
        if(pts.size()>1){
            for(int i=1;i<pts.size();i++){
                ep.test(pts, i);
            }
        }
    }
    return ep;
}

vector<int> ofxQuickHull::Hull3D::getBaseTriangleIdx(vector<ofVec3f> &pts){
    ofxQuickHull::Hull3D::ExtremePoints3D ep = ofxQuickHull::Hull3D::getExtremePoints(pts);
    vector<int> tmp;
    ep.appendIndices(tmp);
    
}

vector<ofVec3f> ofxQuickHull::Hull3D::getBaseTriangle(vector<ofVec3f> &pts){
    ofxQuickHull::Hull3D::ExtremePoints3D ep = ofxQuickHull::Hull3D::getExtremePoints(pts);
    vector<ofVec3f> tmp;
    ep.appendPoints(tmp);
    ofxQuickHull::DistancePair<ofVec3f> dp = getFurthestPoints(tmp);
    float dist = 0;
    int pId = 0;
    for(int i=0;i<tmp.size();i++){
        float curDist = ofxQuickHull::pointSegmentDistance(tmp[i], dp.first, dp.second);
        if(curDist>dist){
            dist = curDist;
            pId = i;
        }
    }
    vector<ofVec3f> tri;
    tri.resize(3);
    tri[0]=dp.first;
    tri[1]=dp.second;
    tri[2]=tmp[pId];
    return tri;
}


void ofxQuickHull::Triangle::set(vector<ofVec3f> * vtx, int id0, int id1, int id2){
    vertices = vtx;
    indices[0]=id0;
    indices[1]=id1;
    indices[2]=id2;
    plane.normal = ofxCodeJutsu::Geometry::getTriangleNormal(vertices->at(id0),vertices->at(id1),vertices->at(id2));
    ofxCodeJutsu::Geometry::planeFromPtAndNormal(vertices->at(id0), plane.normal, plane.a, plane.b, plane.c, plane.d);
}

float ofxQuickHull::Triangle::getDistance(ofVec3f &pt){
    return ofxCodeJutsu::Geometry::getUnsignedPlaneDistance(pt, plane.a, plane.b, plane.c, plane.d);
}

float ofxQuickHull::SubTriangle::getDistance(ofVec3f & pt){
    return ofxCodeJutsu::Geometry::getUnsignedPlaneDistance(pt, plane.a, plane.b, plane.c, plane.d);
}

void ofxQuickHull::Triangle::draw(){
    ofPushStyle();
    ofSetColor(ofColor::yellow);
    ofNoFill();
    ofDrawTriangle(vertices->at(indices[0]), vertices->at(indices[1]), vertices->at(indices[2]));
    
    ofVec3f nStart = ofxCodeJutsu::Geometry::pointInTriangle(vertices->at(indices[0]), vertices->at(indices[1]), vertices->at(indices[2]));
    ofVec3f nEnd = nStart + (plane.normal * 50);
    ofSetColor(ofColor::cyan);
    ofDrawLine(nStart, nEnd);
    //ofSetColor(colPt);
    //for(int i=0;i<assignedPts.size();i++){
    //    ofDrawCircle(assignedPts[i], 10);
    //}
    ofPopStyle();
}

void ofxQuickHull::Triangle::emitTriangle(vector<ofVec3f> * hullSet){
    for(int i=0;i<3;i++){
        hullSet->push_back(vertices->at(indices[i]));
    }
}

void ofxQuickHull::Simplex::clear(){
    for(int i=0;i<4;i++){
        faces[i].clear();
    }
    vertices.clear();
    hullSet = NULL;
}

bool ofxQuickHull::Simplex::isSimplexVertex(ofVec3f p){
    for(int i=0;i<vertices.size();i++){
        if(p==vertices[i]){
            return true;
        }
    }
    return false;
}

void ofxQuickHull::Simplex::init(vector<ofVec3f> &pts, vector<ofVec3f> * _hull){
    clear();
    hullSet =_hull;
    vector<ofVec3f> baseTri = ofxQuickHull::Hull3D::getBaseTriangle(pts);
    
    ofxQuickHull::appendPoints(baseTri, vertices);
    faces[0].set(&vertices, 0, 1, 2);
    
    float dist = 0;
    int pId = 0;
    for(int i=0;i<pts.size();i++){
        float curDist = faces[0].getDistance(pts[i]);
        if(curDist>dist){
            dist = curDist;
            pId = i;
        }
    }
    
    vertices.push_back(pts[pId]);
    
    vector<ofVec3f> order;
    order.resize(3);
    if(ofxCodeJutsu::Geometry::isPointInFrontOfPlane(vertices[3], faces[0].plane.a, faces[0].plane.b, faces[0].plane.c, faces[0].plane.d)){
        faces[0].set(&vertices, 2,1,0);
        order[0].set(1,3,0);
        order[1].set(2,3,1);
        order[2].set(0,3,2);
    }else{
        order[0].set(0,3,1);
        order[1].set(1,3,2);
        order[2].set(2,3,0);
    }
    
    for(int i=0;i<3;i++){
        faces[i+1].set(&vertices, order[i][0], order[i][1], order[i][2]);
    }
    
    if(hullMode==HULL_PT_CLOUD){
        for(int i=0;i<vertices.size(); i++){
            hullSet->push_back(vertices[i]);
        }
    }
    
    vector<ofxQuickHull::AssignablePoint> tba;
    for(int i=0;i<pts.size();i++){
        if(!isSimplexVertex(pts[i])){
            tba.push_back(ofxQuickHull::AssignablePoint(pts[i]));
        }
    }
    assignPtsToFaces(tba);
    
    makeSubSplines();
}

void ofxQuickHull::Simplex::makeSubSplines(){
    for(int i=0;i<4;i++){
        if(faces[i].assignedPts.size()>0){
            ofxQuickHull::SubTriangle st;
            st.setFromTriangle(faces[i], hullSet, hullMode);
            st.iterate();
            
            /*hullSet->push_back(st.vertices[0]);
            hullSet->push_back(st.vertices[1]);
            hullSet->push_back(st.projectedPt);
            
            hullSet->push_back(st.vertices[1]);
            hullSet->push_back(st.vertices[2]);
            hullSet->push_back(st.projectedPt);
            
            hullSet->push_back(st.vertices[2]);
            hullSet->push_back(st.vertices[0]);
            hullSet->push_back(st.projectedPt);*/
        }else{
            if(hullMode==HULL_TRIANGLES){
                faces[i].emitTriangle(hullSet);
            }
        }
    }
}


void ofxQuickHull::SubTriangle::emitTriangle(){
    hullSet->push_back(vertices[0]);
    hullSet->push_back(vertices[1]);
    hullSet->push_back(vertices[2]);
}

void ofxQuickHull::SubTriangle::setFromTriangle(ofxQuickHull::Triangle &t, vector<ofVec3f> *_hull, ofxQuickHull::HullMode _hullMode){
    hullMode = _hullMode;
    hullSet = _hull;
    vertices.clear();
    vertices.push_back(t.vertices->at(t.indices[0]));
    vertices.push_back(t.vertices->at(t.indices[1]));
    vertices.push_back(t.vertices->at(t.indices[2]));
    plane = t.plane;
    for(int i=0;i<t.assignedPts.size();i++){
        assignedPts.push_back(t.assignedPts[i]);
    }
}

void ofxQuickHull::SubTriangle::setFromPoints(ofVec3f p0, ofVec3f p1, ofVec3f p2, vector<ofVec3f> *_hull, HullMode _hullMode){
    hullMode = _hullMode;
    hullSet=_hull;
    setPoints(p0, p1, p2);
}

void ofxQuickHull::SubTriangle::setPoints(ofVec3f p0, ofVec3f p1, ofVec3f p2){
    vertices.clear();
    vertices.push_back(p0);
    vertices.push_back(p1);
    vertices.push_back(p2);
    plane.normal = ofxCodeJutsu::Geometry::getTriangleNormal(p0, p1, p2);
    ofxCodeJutsu::Geometry::planeFromPtAndNormal(p0, plane.normal, plane.a, plane.b, plane.c, plane.d);
}

void ofxQuickHull::SubTriangle::iterate(){
    if(assignedPts.size()<=0)return;
    ofVec3f nPt = getFurthest();
    makeSubFaces(nPt);
}

ofVec3f ofxQuickHull::SubTriangle::getFurthest(){
    float dist = 0;
    int pId = 0;
    for(int i=0;i<assignedPts.size();i++){
        float curDist = getDistance(assignedPts[i]);
        if(curDist>dist){
            dist=curDist;
            pId=i;
        }
    }
    projectedPt = assignedPts[pId];
    if(hullMode==HULL_PT_CLOUD){
        hullSet->push_back(assignedPts[pId]);
    }
    assignedPts.erase(assignedPts.begin()+pId);
    return projectedPt;
}

void ofxQuickHull::SubTriangle::makeSubFaces(ofVec3f nPt){
    ofxQuickHull::SubTriangle faces[3];
    faces[0].setFromPoints(vertices[0], nPt, vertices[1], hullSet, hullMode);
    if(ofxCodeJutsu::Geometry::isPointInFrontOfPlane(vertices[2], faces[0].plane.a, faces[0].plane.b, faces[0].plane.c, faces[0].plane.d)){
        faces[0].setPoints(vertices[0], vertices[1], nPt);
        faces[1].setFromPoints(vertices[1], vertices[2], nPt, hullSet, hullMode);
        faces[2].setFromPoints(vertices[2], vertices[0], nPt, hullSet, hullMode);
    }else{
        faces[1].setFromPoints(vertices[1], nPt, vertices[2], hullSet, hullMode);
        faces[2].setFromPoints(vertices[2], nPt, vertices[0], hullSet, hullMode);
    }
    
    vector<ofxQuickHull::AssignablePoint> tba;
    for(int i=0;i<assignedPts.size();i++){
        tba.push_back(ofxQuickHull::AssignablePoint(assignedPts[i]));
    }
    for(int i=0;i<3;i++){
        faces[i].assignPoints(tba);
    }
    
    for(int i=0;i<3;i++){
        if(faces[i].assignedPts.size()>0){
            faces[i].iterate();
        }else{
            if(hullMode==HULL_TRIANGLES){
                faces[i].emitTriangle();
                //hullSet->push_back(faces[i].vertices[0]);
                //hullSet->push_back(faces[i].vertices[1]);
                //hullSet->push_back(faces[i].vertices[2]);
            }
        }
    }
}

void ofxQuickHull::SubTriangle::assignPoints(vector<ofxQuickHull::AssignablePoint> &tba){
    for(int i=0;i<tba.size();i++){
        if(!tba[i].flag){
            if(ofxCodeJutsu::Geometry::isPointInFrontOfPlane(tba[i].pt, plane.a, plane.b, plane.c, plane.d)){
                tba[i].flag = true;
                assignedPts.push_back(tba[i].pt);
            }
        }
    }
}

void ofxQuickHull::Triangle::assignPts(vector<ofxQuickHull::AssignablePoint> &tba){
    for(int i=0;i<tba.size();i++){
        if(!tba[i].flag){
            if(ofxCodeJutsu::Geometry::isPointInFrontOfPlane(tba[i].pt, plane.a, plane.b, plane.c, plane.d)){
                tba[i].flag = true;
                assignedPts.push_back(tba[i].pt);
            }
        }
    }
}

void ofxQuickHull::Simplex::assignPtsToFaces(vector<ofxQuickHull::AssignablePoint> &tba){
    for(int i=0;i<4;i++){
        faces[i].assignPts(tba);
    }
}

void ofxQuickHull::Simplex::draw(){
    for(int i=0;i<4;i++){
        faces[i].draw();
    }
}

template <typename ofVecType>
ofxQuickHull::DistancePair<ofVecType> ofxQuickHull::getFurthestPoints(vector<ofVecType> &set){
    vector<ofxQuickHull::DistancePair<ofVecType>> pairs;
    pairs.resize(set.size());
    int pId = 0;
    float curDist = 0;
    for(int i=0;i<set.size();i++){
        pId = ofxQuickHull::getMostDistant(set[i], set, curDist);
        pairs[i].first = set[i];
        pairs[i].second = set[pId];
        pairs[i].distance = curDist;
    }
    
    pId = 0;
    curDist = 0;
    for(int i=0;i<pairs.size();i++){
        if(pairs[i].distance>curDist){
            curDist = pairs[i].distance;
            pId = i;
        }
    }
    
    return pairs[pId];
}

float ofxQuickHull::pointSegmentDistance(ofVec3f p, ofVec3f segStart, ofVec3f segEnd){
    float segL = segStart.distance(segEnd);
    float u = (((p.x-segStart.x)*(segEnd.x-segStart.x))+
              ((p.y-segStart.y)*(segEnd.y-segStart.y))+
              ((p.z-segStart.z)*(segEnd.x-segStart.z)))/
              (segL*segL);
    //u<0 || u>1 closest point is on segment projection
    ofVec3f intersection;
    intersection.x = segStart.x+u*(segEnd.x-segStart.x);
    intersection.y = segStart.y+u*(segEnd.y-segStart.y);
    intersection.z = segStart.z+u*(segEnd.z-segStart.z);
    return p.distance(intersection);
}

