//
//  ofxQuickHull.h
//
//  magicLab 2017
//  http://www.magiclab.nyc
//  Created by enrico<nausea>viola on 1/10/17.
//
//

#pragma once
#include "ofMain.h"
#include "ofxCodeJutsu.h"

namespace ofxQuickHull{
    enum HullMode{
        HULL_PT_CLOUD,
        HULL_TRIANGLES
    };
    
    template<typename ofVecType>
    void appendPoints(vector<ofVecType> & src, vector<ofVecType> & dst){
        for(int i=0;i<src.size();i++){
            dst.push_back(src[i]);
        }
    }
    
    class AssignablePoint{
    public:
        AssignablePoint(){flag=false;}
        AssignablePoint(ofVec3f _pt){pt=_pt;flag=false;}
        ofVec3f pt;
        bool flag;
    };
    
    class Plane{
    public:
        ofVec3f normal;
        float a;
        float b;
        float c;
        float d;
    };
    
    class Triangle{
    public:
        Triangle(){clear();colPt.set(ofRandomf()*255,ofRandomf()*255,ofRandomf()*255);}
        void clear(){
            vertices = NULL;
            assignedPts.clear();
        }
        void draw();
        void set(vector<ofVec3f> * vtx, int id0, int id1, int id2);
        float getDistance(ofVec3f & pt);
        void assignPts(vector<ofxQuickHull::AssignablePoint> & tba);
        void emitTriangle(vector<ofVec3f> * hullSet);
        vector<ofVec3f> assignedPts;
        vector<ofVec3f> * vertices;
        int indices[3];
        Plane plane;
        ofColor colPt;
    };
    
    class SubTriangle{
    public:
        /*void setFromTriangle(ofxQuickHull::Triangle & t, vector<ofVec3f> * _hull, HullMode _hullMode){
            hullMode=_hullMode;
            hullSet=_hull;
            plane = t.plane;
            for(int i=0;i<3;i++){
                vertices.push_back(t.vertices->at(i));
            }
            for(int i=0;i<t.assignedPts.size();i++){
                assignedPts.push_back(t.assignedPts[i]);
            }
            
        }*/
        void emitTriangle();
        void setFromTriangle(ofxQuickHull::Triangle & t, vector<ofVec3f> * _hull, HullMode _hullMode);
        void setFromPoints(ofVec3f p0, ofVec3f p1, ofVec3f p2, vector<ofVec3f> * _hull, HullMode _hullMode);
        
        float getDistance(ofVec3f & pt);
        ofVec3f getFurthest();
        void iterate();
        void makeSubFaces(ofVec3f nPt);
        void setPoints(ofVec3f p0, ofVec3f p1, ofVec3f p2);
        void assignPoints(vector<ofxQuickHull::AssignablePoint> & tba);
        
        Plane plane;
        vector<ofVec3f> vertices;
        vector<ofVec3f> assignedPts;
        vector<ofVec3f> * hullSet;
        ofVec3f projectedPt;
        HullMode hullMode;
    };
    
    class Simplex{
    public:
        Simplex(){hullMode = HULL_PT_CLOUD; clear();}
        Simplex(vector<ofVec3f> & pts, vector<ofVec3f> * _hull){hullMode = HULL_PT_CLOUD;init(pts, _hull);}
        void clear();
        void init(vector<ofVec3f> & pts, vector<ofVec3f> * _hull);
        bool isSimplexVertex(ofVec3f p);
        void assignPtsToFaces(vector<ofxQuickHull::AssignablePoint> & tba);
        void makeSubSplines();
        void draw();
        vector<ofVec3f> vertices;
        vector<ofVec3f> * hullSet;
        Triangle faces[4];
        HullMode hullMode;
    };
    
    template <class ofVecType>
    class AxisExtremes{
    public:
        void set(ofVecType val, int _id){
            min = val;
            max = val;
            idMin=_id;
            idMax=_id;
        }
        void appendPoints(vector<ofVecType> & pts){
            pts.push_back(min);
            pts.push_back(max);
        }
        void appendIndices(vector<int> & idx){
            idx.push_back(idMin);
            idx.push_back(idMax);
        }
        ofVecType min;
        ofVecType max;
        int idMin;
        int idMax;
    };
    
    typedef AxisExtremes<ofVec2f> AxisExtremes2D;
    typedef AxisExtremes<ofVec3f> AxisExtremes3D;
    
    template <class ofVecType>
    class DistancePair{
    public:
        ofVecType first;
        ofVecType second;
        int idFirst;
        int idSecond;
        float distance;
    };
    
    template <class ofVecType>
    class ExtremePoints{
    public:
        void set(vector<ofVecType> & pts, int idx){
            set(pts[idx], idx);
        }
        
        void set(ofVecType val, int _id){
            for(int i=0;i<ae.size();i++){
                ae[i].set(val, _id);
            }
        }
        
        void test(int axis, ofVecType val, int idx){
            if(ae[axis].min[axis] < val[axis]){
                ae[axis].min = val;
                ae[axis].idMin = idx;
            }else if(ae[axis].max[axis] > val[axis]){
                ae[axis].max = val;
                ae[axis].idMax = idx;
            }
        }
        void test(ofVecType val, int idx){
            for(int i=0;i<ae.size();i++){
                test(i, val, idx);
            }
        }
        void test(vector<ofVecType> & pts, int idx){
            test(pts[idx], idx);
        }
        void appendPoints(vector<ofVecType> & pts){
            for(int i=0;i<ae.size();i++){
                ae[i].appendPoints(pts);
            }
        }
        void appendIndices(vector<int> & idx){
            for(int i=0;i<ae.size();i++){
                ae[i].appendIndices(idx);
            }
        }
    protected:
        vector<AxisExtremes<ofVecType>> ae;
    };
    
    template<typename ofVecType>
    static int getMostDistant(ofVecType & me, vector<ofVecType> & set, float & dist){
        int idx = -1;
        if(set.size()>0){
            idx = 0;
            dist = me.distance(set[0]);
            if(set.size()>1){
                for(int i=1;i<set.size();i++){
                    float curDist = me.distance(set[i]);
                    if(curDist>dist){
                        dist = curDist;
                        idx=i;
                    }
                }
            }
        }
        return idx;
    }
    
    static float pointSegmentDistance(ofVec3f p, ofVec3f segStart, ofVec3f segEnd);
    
    template<typename ofVecType>
    static DistancePair<ofVecType> getFurthestPoints(vector<ofVecType> & set);
    
    class Hull2D{
    public:
        class ExtremePoints2D : public ExtremePoints<ofVec2f>{
        public:
            ExtremePoints2D(){
                ae.resize(2);
            }
            AxisExtremes2D & x(){return ae[0];}
            AxisExtremes2D & y(){return ae[1];}
        };
        
        static vector<ofVec2f> getHull(vector<ofVec2f> & pts);
        static ExtremePoints2D getExtremePoints(vector<ofVec2f> & pts);
    };
    
    class Hull3D{
    public:
        class ExtremePoints3D : public ExtremePoints<ofVec3f>{
        public:
            ExtremePoints3D(){
                ae.resize(3);
            }
            AxisExtremes3D & x(){return ae[0];}
            AxisExtremes3D & y(){return ae[1];}
            AxisExtremes3D & z(){return ae[2];}
        };
        static vector<ofVec3f> getHull(vector<ofVec3f> & pts);
        static ExtremePoints3D getExtremePoints(vector<ofVec3f> & pts);
        static vector<ofVec3f> getBaseTriangle(vector<ofVec3f> & pts);
        static vector<int> getBaseTriangleIdx(vector<ofVec3f> & pts);
    };
    
}