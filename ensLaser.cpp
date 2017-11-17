#include "ensLaser.h"

CensHalfLaser::CensHalfLaser():m_program(nullptr),m_maskTex(nullptr),m_maskTex2(nullptr),m_noiseTex(nullptr),m_noiseTex2(nullptr),
m_highlight(1.0f),m_time(0),m_PorN(1),m_kLightColor(1.0f),m_kGlowColor(1.0f),m_noiseScale(1.0f)
{}

CensHalfLaser::~CensHalfLaser(){
    CC_SAFE_RELEASE_NULL(m_program);
    CC_SAFE_RELEASE_NULL(m_maskTex);
    CC_SAFE_RELEASE_NULL(m_maskTex2);
    CC_SAFE_RELEASE_NULL(m_noiseTex);
    CC_SAFE_RELEASE_NULL(m_noiseTex2);
}

CensHalfLaser* CensHalfLaser::create(const string &Laser,const string &mask, const string &mask2, const string &noise, const string &noise2)
{
    CensHalfLaser *halfLaser = new (std::nothrow) CensHalfLaser();
    if (halfLaser && halfLaser->init(Laser, mask, mask2, noise, noise2))
    {
        return halfLaser;
    }
    CC_SAFE_DELETE(halfLaser);
    return nullptr;
}

bool CensHalfLaser::init(const string &Laser,const string &mask, const string &mask2, const string &noise, const string &noise2)
{
    
    //lightTex
    this->CCSprite::initWithFile(Laser);
    {
        Texture2D::TexParams texParams= {GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT};
        this->getTexture()->setTexParameters(&texParams);
    }
    
    //maskTex
    m_maskTex=CCTextureCache::sharedTextureCache()->addImage(mask);
    m_maskTex->retain();
    {
        Texture2D::TexParams texParams= {GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT};
        m_maskTex->setTexParameters(&texParams);
    }
    //maskTex2
    m_maskTex2=CCTextureCache::sharedTextureCache()->addImage(mask2);
    m_maskTex2->retain();
    {
        Texture2D::TexParams texParams= {GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT};
        m_maskTex2->setTexParameters(&texParams);
    }

    //noiseTex
    m_noiseTex=CCTextureCache::sharedTextureCache()->addImage(noise);
     m_noiseTex->retain();
    {
        Texture2D::TexParams texParams= {GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT};
        m_noiseTex->setTexParameters(&texParams);
    }
    //noiseTex2
    m_noiseTex2=CCTextureCache::sharedTextureCache()->addImage(noise2);
    m_noiseTex2->retain();
    {
        Texture2D::TexParams texParams= {GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT};
        m_noiseTex2->setTexParameters(&texParams);
    }

    //create and set shader program
    {
        GLchar * fragSource = (GLchar*) CCString::createWithContentsOfFile(CCFileUtils::sharedFileUtils()->fullPathForFilename("shaders/laser.fsh").c_str())->getCString();
        ens::CGLProgramWithUnifos* program = new ens::CGLProgramWithUnifos();
        program->autorelease();
        program->initWithVertexShaderByteArray(ccPositionTextureColor_noMVP_vert, fragSource);//use ccPositionTextureColor_noMVP_vert instead of ccPositionTextureColor_vert, see: http://www.cnblogs.com/wantnon/p/4190341.html
        //            --below code is no longer needed, because bindPredefinedVertexAttribs() is called in link() in 3.x
        //            --bind attribute
        //            --program->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
        //            --program->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
        //            --program->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
        //link  (must after bindAttribute)
        program->link();
        //get cocos2d-x build-in uniforms
        program->updateUniforms();
        //get my own uniforms
        program->attachUniform("u_maskTex");
        program->attachUniform("u_maskTex2");
        program->attachUniform("u_noiseTex");
        program->attachUniform("u_noiseTex2");
        program->attachUniform("u_spriteSize");
        program->attachUniform("u_LUPos");
        program->attachUniform("u_noiseSize");
        program->attachUniform("u_highlight");
        program->attachUniform("u_time");
        program->attachUniform("u_PorN");
        program->attachUniform("u_scale");
        program->attachUniform("u_kLightColor");
        program->attachUniform("u_kGlowColor");
        program->attachUniform("u_noiseScale");
        program->attachUniform("u_color_r");
        program->attachUniform("u_color_g");
        program->attachUniform("u_color_b");
        
        //set program
        m_program=program;
        m_program->retain();
        
        //check gl error
        CHECK_GL_ERROR_DEBUG();
    }
    //update
    this->scheduleUpdate();
 

    return true;

}

void CensHalfLaser::updateStartAndEnd(){
    
    CCPoint mid=ccpMult(m_start+m_end, 0.5) ;
    CCPoint dir=ccpNormalize(m_end-m_start);
    float A;
    if(dir.y<=0){
        A=acosf(dir.x)*180/M_PI;
    }else{
        A=(2*M_PI-acosf(dir.x))*180/M_PI;
    }
    //this->setPosition(mid);
    this->setRotation(A);
    
    float targetLen=ccpDistance(m_start, m_end);
    float contentLen=this->getContentSize().width;
    float scaleX=targetLen/contentLen;
    this->setScaleX(scaleX);
    
    
    
}
void CensHalfLaser::setStart(const CCPoint&start){
    m_start=start;
    updateStartAndEnd();
}
void CensHalfLaser::setEnd(const CCPoint&end){
    m_end=end;
    updateStartAndEnd();
}

void CensHalfLaser::setBlendFunc(GLenum src,GLenum dst){
    //
    ccBlendFunc blendFunc={src,dst};
    this->CCSprite::setBlendFunc(blendFunc);
    
}
void CensHalfLaser::update(float dt){
    m_time+=dt;
    if(m_time>=10000)m_time=0;
    
}

void CensHalfLaser::draw(Renderer* renderer, const Mat4 &transform, uint32_t flags){
    //change shader
    this->setShaderProgram(m_program);
    
    
    //passUnifo and bindTex
    _passUnifoAndBindTexCommand.init(_globalZOrder);
    _passUnifoAndBindTexCommand.func = CC_CALLBACK_0(CensHalfLaser::onPassUnifoAndBindTex, this,transform,flags);
    Director::getInstance()->getRenderer()->addCommand(&_passUnifoAndBindTexCommand);
    //draw quad
    {
        assert(getGLProgramState()->getGLProgram()==m_program);
        //if we use _quadCommand to draw we must use noMV shader.
        //because (if i am not seeing wrong) in _quadCommand modelview is applied to vertex via cpu and at the same time modelview is passed in to shader.
        //shit!
        //_quadCommand.init(_globalZOrder, _texture->getName(), getGLProgramState(), _blendFunc, &_quad, 1, transform);
        //renderer->addCommand(&_quadCommand);	
		_trianglesCommand.init(_globalZOrder, _texture->getName(), getGLProgramState(), _blendFunc, _polyInfo.triangles, transform, flags);
		renderer->addCommand(&_trianglesCommand);
    }
    

    
}
void CensHalfLaser::onPassUnifoAndBindTex(const Mat4 &transform, uint32_t flags){
    assert(getGLProgram()==m_program);
    float spriteSize_tmp[2]={this->getContentSize().width*this->getScaleX(),
        this->getContentSize().height*this->getScaleY()};//because this sprite may rotate, so we use contentSize*scale instead of boundingBox().size
    float LUPos_tmp[2]={0,0};//if we use {boundingBox().getMinX(),boundingBox().getMaxY()} instead of {0,0}, when this sprite scale, the noiseTex will move, cause uncontinuous effect. to avoid this, so we should use {0,0}
    float noiseSize_tmp[2]={m_noiseTex->getContentSize().width,m_noiseTex->getContentSize().height};
    float scale_tmp[2]={this->getScaleX(),this->getScaleY()};
    //pass values for cocos2d-x build-in uniforms
    ens::CGLProgramWithUnifos*program=(ens::CGLProgramWithUnifos*)this->getShaderProgram();
    program->use();
    program->setUniformsForBuiltins();
    //pass values for my own uniforms
    program->passUnifoValue1i("u_maskTex", 1);
    program->passUnifoValue1i("u_noiseTex", 2);
    program->passUnifoValue1i("u_noiseTex2", 4);
    program->passUnifoValue1i("u_maskTex2", 3);
    program->passUnifoValueNfv("u_spriteSize", spriteSize_tmp, 2);
    program->passUnifoValueNfv("u_LUPos", LUPos_tmp, 2);
    program->passUnifoValueNfv("u_noiseSize", noiseSize_tmp, 2);
    program->passUnifoValue1f("u_highlight", m_highlight);
    program->passUnifoValue1f("u_time", m_time);
    program->passUnifoValue1f("u_PorN", m_PorN);
    program->passUnifoValueNfv("u_scale", scale_tmp, 2);
    program->passUnifoValue1f("u_kLightColor", m_kLightColor);
    program->passUnifoValue1f("u_kGlowColor", m_kGlowColor);
    program->passUnifoValue1f("u_noiseScale", m_noiseScale);
    
    Node *parent = getParent();
    Color3B color3b = getColor();
    if (parent)
        color3b = parent->getColor();
    program->passUnifoValue1f("u_color_r", color3b.r/255.0);
    program->passUnifoValue1f("u_color_g", color3b.g/255.0);
    program->passUnifoValue1f("u_color_b", color3b.b/255.0);
    setColor(color3b);
    //attach texture to texture attach point
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_maskTex->getName());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_noiseTex->getName());
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_maskTex2->getName());
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_noiseTex2->getName());
    glActiveTexture(GL_TEXTURE0);//back to GL_TEXTURE0
    
    //note: i am not investigated in detail yet,but it seems that if we do not use any of CC_Texture0,CC_Texture1,..., we can use our own sampler names, otherwise, we should only use CC_Texture0, CC_Texture1,.... .
}

//----------------------------------------------------------------------------------------------------
CensLaser::CensLaser():m_halfLaser1(nullptr),m_halfLaser2(nullptr),m_slimHalfLaser1(nullptr),m_slimHalfLaser2(nullptr)
{}

CensLaser* CensLaser::create(const string &Laser,const string &mask, const string &mask2, const string &noise, const string &noise2)
{
    CensLaser *laser = new (std::nothrow) CensLaser();
    if (laser && laser->init(Laser, mask, mask2, noise, noise2))
    {
        return laser;
    }
    CC_SAFE_DELETE(laser);
    return nullptr;
}

bool CensLaser::init(const string &Laser,const string &mask, const string &mask2, const string &noise, const string &noise2){
    //halfLaser1
    m_halfLaser1=CensHalfLaser::create(Laser, mask, mask2, noise, noise2);
    m_halfLaser1->setPorN(1);
    m_halfLaser1->setBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    m_halfLaser1->setScaleY(1.3);//(1.0);
    m_halfLaser1->m_kGlowColor=1.6;//1.0;
    m_halfLaser1->m_kLightColor=0.0;
    m_halfLaser1->m_noiseScale=2.2;//0.5;
    addChild(m_halfLaser1);
    //halfLaser2
    m_halfLaser2=CensHalfLaser::create(Laser, mask, mask2, noise, noise2);
    m_halfLaser2->setPorN(-1);
    m_halfLaser2->setBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    m_halfLaser2->setScaleY(1.3);//(1.0);
    m_halfLaser2->m_kGlowColor=1.6;//1.0;
    m_halfLaser2->m_kLightColor=0.0;
    m_halfLaser2->m_noiseScale=2.2;//0.5;
    addChild(m_halfLaser2);
    //slimHalfLaser1
    m_slimHalfLaser1=CensHalfLaser::create(Laser, mask, mask2, noise, noise2);;
    m_slimHalfLaser1->setPorN(1);
    m_slimHalfLaser1->setBlendFunc(GL_ONE, GL_ONE);
    m_slimHalfLaser1->setScaleY(0.4);//(0.6);//(0.1);
    m_slimHalfLaser1->m_kGlowColor=2.5;//1.5;
    m_slimHalfLaser1->m_kLightColor=0.5;//0.4;//0.3;
    m_slimHalfLaser1->m_noiseScale=1.5;
    addChild(m_slimHalfLaser1);
    //slimHalfLaser2
    m_slimHalfLaser2=CensHalfLaser::create(Laser, mask, mask2, noise, noise2);
    m_slimHalfLaser2->setPorN(-1);
    m_slimHalfLaser2->setBlendFunc(GL_ONE, GL_ONE);
    m_slimHalfLaser2->setScaleY(0.4);//(0.6);//(0.1);
    m_slimHalfLaser2->m_kGlowColor=2.5;//1.5;
    m_slimHalfLaser2->m_kLightColor=0.5;//0.4;//0.3;
    m_slimHalfLaser2->m_noiseScale=1.5;
    addChild(m_slimHalfLaser2);
    
    setContentSize(m_halfLaser1->getContentSize());
    m_halfLaser1:setPosition(m_halfLaser1->getContentSize().width*0.5, m_halfLaser1->getContentSize().height*0.5);
    m_halfLaser2:setPosition(m_halfLaser1->getContentSize().width*0.5, m_halfLaser1->getContentSize().height*0.5);
    m_slimHalfLaser1:setPosition(m_halfLaser1->getContentSize().width*0.5, m_halfLaser1->getContentSize().height*0.5);
    m_slimHalfLaser2:setPosition(m_halfLaser1->getContentSize().width*0.5, m_halfLaser1->getContentSize().height*0.5);
    
    
    return true;
}

void CensLaser::setStart(const CCPoint&start){
    m_start=start;
    m_halfLaser1->setStart(m_start);
    m_halfLaser2->setStart(m_start);
    m_slimHalfLaser1->setStart(m_start);
    m_slimHalfLaser2->setStart(m_start);
    
    CCPoint mid=ccpMult(m_start+m_end, 0.5) ;
    CCPoint dir=ccpNormalize(m_end-m_start);
    float A;
    if(dir.y<=0){
        A=acosf(dir.x)*180/M_PI;
    }else{
        A=(2*M_PI-acosf(dir.x))*180/M_PI;
    }
    setPosition(mid);
}
void CensLaser::setEnd(const CCPoint&end){
    m_end=end;
    m_halfLaser1->setEnd(m_end);
    m_halfLaser2->setEnd(m_end);
    m_slimHalfLaser1->setEnd(m_end);
    m_slimHalfLaser2->setEnd(m_end);
    
    CCPoint mid=ccpMult(m_start+m_end, 0.5) ;
    CCPoint dir=ccpNormalize(m_end-m_start);
    float A;
    if(dir.y<=0){
        A=acosf(dir.x)*180/M_PI;
    }else{
        A=(2*M_PI-acosf(dir.x))*180/M_PI;
    }
    setPosition(mid);
}