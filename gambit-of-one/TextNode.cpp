#include "Headers/TextNode.h"

TextNode::TextNode(const FontHolder& fonts, const std::string& text)
{
	mText.setFont(fonts.get(Fonts::Main));
	mText.setCharacterSize(20);
	setString(text);
}

void TextNode::setString(const std::string& text)
{
	mText.setString(text);
	centerOrigin(mText);
}

void TextNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states)
{
	target.draw(mText, states);
}
