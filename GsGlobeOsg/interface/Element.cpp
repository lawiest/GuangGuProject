#include "Element.h"

GLOBE_NS

Element::Element()
{
}

Element::~Element()
{
}
ElementType Element::Type()
{
	return m_ElementType;
}
std::string Element::Name()
{
	return m_strName;
}
void Element::setName(const std::string & name)
{
	m_strName = name;
}
std::string Element::UID()
{
	return m_UID;
}
void Element::setUID(const std::string & uid)
{
	m_UID = uid;
}
void Element::setAtrribute(const std::string & name, const std::string & value)
{
	m_mapAttribute[name] = value;
}
std::string Element::getAttribute(const std::string & name)
{
	std::map<std::string, std::string>::iterator  iter = m_mapAttribute.find(name);
	if (iter != m_mapAttribute.end())
		return iter->second;
	return std::string();
}
std::map<std::string, std::string>& Element::getAttributes()
{
	// TODO: 在此处插入 return 语句
	return m_mapAttribute;
}
double Element::Longitude()
{
	return m_dLongitude;
}
double Element::Latitude()
{
	return m_dLatitude;
}
double Element::Height()
{
	return m_dHeigh;
}
GLOBE_ENDNS