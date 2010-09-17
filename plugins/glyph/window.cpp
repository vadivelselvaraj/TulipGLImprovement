#ifndef WINDOWGLYPH_H
#define WINDOWGLYPH_H

#include <tulip/Glyph.h>
#include <tulip/GlRect.h>
#include <tulip/GlLabel.h>
#include <tulip/GlPolyQuad.h>

using namespace tlp;
using namespace std;



const string texture(TulipBitmapDir + "cylinderTexture.png");
const string gradient(TulipBitmapDir + "titlebarGradient.png");

class Window : public Glyph
{
public:
    Window(GlyphContext* context);
    virtual void getIncludeBoundingBox(BoundingBox &boundingBox);
    virtual void draw(node n, float lod);
    virtual Coord getAnchor(const Coord &vector) const;
    bool renderLabel() {
      return true;
    }
protected:
private:
    Color _rectColor;
    Color _textColor;
    const float _titleBarSize;
    const float _borderWidth;
    GlPolyQuad _border;
    GlPolyQuad _center;
    GlPolyQuad _titleRec;
    BoundingBox _bb;
    GlLabel *_title;
};

/*
 * 
*----------- -* -0.5, 0.5      0.5, 0.5
|\          / |  
| *--------*  |-0.5 + _borderWith   0.5, 0.5-bo
| |        |  |
| *--------*  |
|/          \ |
*------------*  -0.5,-0.5        0.5, -0.5
*/

GLYPHPLUGIN(Window, "2D - Window", "David Auber", "28/05/2010", "Window with a title bar", "1.0", 8);

Window::Window(GlyphContext* context): 
	Glyph(context), 
	_rectColor(205, 205, 205, 255), 
	_textColor(205, 205, 205, 255), 
	_titleBarSize(0.1), 
	_borderWidth(0.02), 
	_border(texture),
	_center(texture),
	_titleRec(gradient)
	{
	
	const float textheight = 0.05;
	Coord textCenter(-0.5 + _borderWidth, 0.5 - textheight- _borderWidth , 0.0);
	_title = new GlLabel(textCenter, Coord(1. - (2.f * _borderWidth), _titleBarSize, 0.0), _textColor, true);
	
	Coord v[10];
	v[0].set(-0.5, 0.5, 0);
	v[1].set( 0.5, 0.5, 0);
	v[2].set( 0.5,-0.5, 0);
	v[3].set(-0.5,-0.5, 0);
	v[4].set(-0.5 + _borderWidth, 0.5 - _borderWidth, 0);
	v[5].set( 0.5 - _borderWidth, 0.5 - _borderWidth, 0);
	v[6].set( 0.5 - _borderWidth,-0.5 + _borderWidth, 0);
	v[7].set(-0.5 + _borderWidth,-0.5 + _borderWidth, 0);
	v[8].set( 0.5 - _borderWidth, 0.5 - _borderWidth - textheight *2.f, 0);
	v[9].set(-0.5 + _borderWidth, 0.5 - _borderWidth - textheight *2.f, 0);
	Coord tmp(0.5,0.5,0);
	_bb.expand(v[8] + tmp);
	_bb.expand(v[9] + tmp);
	_bb.expand(v[6] + tmp);
	_bb.expand(v[7] + tmp);
	
	 _border.addQuadEdge(v[0], v[4], _textColor);
	 _border.addQuadEdge(v[1], v[5], _textColor);
	 _border.addQuadEdge(v[2], v[6], _textColor);
	 _border.addQuadEdge(v[3], v[7], _textColor);
	 _border.addQuadEdge(v[0], v[4], _textColor);
	 
	 _titleRec.addQuadEdge(v[4], v[9], _rectColor);
	 _titleRec.addQuadEdge(v[5], v[8], _rectColor);

	 _center.addQuadEdge(v[4], v[7], _rectColor);
	 _center.addQuadEdge(v[5], v[6], _rectColor);
 
	}
 void Window::getIncludeBoundingBox(BoundingBox &boundingBox) {
  boundingBox = _bb;
}

void Window::draw(node n, float lod) {
	
        StringProperty* prop = glGraphInputData->elementLabel;
        ColorProperty* color = glGraphInputData->elementColor;
	ColorProperty* colorBorder = glGraphInputData->elementBorderColor;

	_rectColor = color->getNodeValue(n);
	_border.setColor(colorBorder->getNodeValue(n));	
	_center.setColor(_rectColor);	
	_titleRec.setColor(_rectColor);	
	_title->setText(prop->getNodeValue(n));
        _title->setColor(glGraphInputData->elementLabelColor->getNodeValue(n));

	_border.draw(lod, NULL);
	_center.draw(lod, NULL);
	_titleRec.draw(lod, NULL);
        if (lod > 20) {
            _title->draw(lod, NULL);
	}

  
}

//=====================================================
Coord Window::getAnchor(const Coord &vector) const {
	Coord v(vector);
	float x, y, z, fmax;
	v.get(x, y, z);
	v.setZ(0.0f);
	fmax = std::max(fabsf(x), fabsf(y));
	if (fmax > 0.0f)
		return v * (0.5f / fmax);
	else
		return v;
}

#endif // WINDOWGLYPH_H
