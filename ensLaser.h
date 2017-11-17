#ifndef __ensLaser_H__
#define __ensLaser_H__

#include "cocos2d.h"
using namespace cocos2d;
#include "cocos-ext.h"
using namespace cocos2d::extension;
#include "ensCommon.h"
class CensHalfLaser:public CCSprite{
public:
    CensHalfLaser();
    virtual~CensHalfLaser();
    static CensHalfLaser* create(const string &Laser,const string &mask, const string &mask2, const string &noise, const string &noise2);
    bool init(const string &Laser,const string &mask, const string &mask2, const string &noise, const string &noise2);
    
    void setBlendFunc(GLenum src,GLenum dst);
    void updateStartAndEnd();
    void setStart(const CCPoint&start);
    void setEnd(const CCPoint&end);
    
    void draw(Renderer* renderer, const Mat4 &transform, uint32_t flags);
    void update(float dt);
    void setPorN(float value){m_PorN=value;}
    void onPassUnifoAndBindTex(const Mat4 &transform, uint32_t flags);
    
    ens::CGLProgramWithUnifos*m_program;
    CCTexture2D*m_maskTex;
    CCTexture2D*m_maskTex2;
    CCTexture2D*m_noiseTex;
    CCTexture2D*m_noiseTex2;
    float m_highlight;
    float m_time;
    float m_PorN;
    CCPoint m_start;
    CCPoint m_end;
    float m_kLightColor;
    float m_kGlowColor;
    float m_noiseScale;

    CustomCommand _customCommand;
    CustomCommand _passUnifoAndBindTexCommand;
};

class CensLaser:public Node
{
public:
    CensLaser();
    static CensLaser* create(const string &Laser,const string &mask, const string &mask2, const string &noise, const string &noise2);
    bool init(const string &Laser,const string &mask, const string &mask2, const string &noise, const string &noise2);
    void setStart(const CCPoint&start);
    void setEnd(const CCPoint&end);

private:
    CensHalfLaser*m_halfLaser1;
    CensHalfLaser*m_halfLaser2;
    CensHalfLaser*m_slimHalfLaser1;
    CensHalfLaser*m_slimHalfLaser2;
    CCPoint m_start;
    CCPoint m_end;
};

#endif // __ensLaser_H__
