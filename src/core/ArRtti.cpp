#include "ArRtti.h"

namespace Aurora
{
	//----------------------------------------------------APropertyList-----------------------------------------------------

	void APropertyList::set(const std::string &name, const std::string &value)
	{
		if (m_properties.find(name) != m_properties.end())
			LOG(ERROR) << "Property \"" << name << "\" was specified multiple times!";
		auto &prop = m_properties[name];
		prop.addValue(value);
	}

	void APropertyList::set(const std::string &name, const std::vector<std::string> &values)
	{
		if (m_properties.find(name) != m_properties.end())
			LOG(ERROR) << "Property \"" << name << "\" was specified multiple times!";
		CHECK_GT(values.size(), 0);
		auto &prop = m_properties[name];
		prop.setValue(values);
	}

	bool APropertyList::getBoolean(const std::string &name) const { return get<bool>(name, 0); }
	bool APropertyList::getBoolean(const std::string &name, const bool &defaultValue) const { return get<bool>(name, 0, defaultValue); }

	Float APropertyList::getFloat(const std::string &name) const { return get<Float>(name, 0); }
	Float APropertyList::getFloat(const std::string &name, const Float &defaultValue) const { return get<Float>(name, 0, defaultValue); }

	int APropertyList::getInteger(const std::string &name) const { return get<Float>(name, 0); }
	int APropertyList::getInteger(const std::string &name, const int &defaultValue) const { return get<Float>(name, 0, defaultValue); }

	std::string APropertyList::getString(const std::string &name) const { return get<std::string>(name, 0); }
	std::string APropertyList::getString(const std::string &name, const std::string &defaultValue) const { return get<std::string>(name, 0, defaultValue); }

	AVector2f APropertyList::getVector2f(const std::string &name) const
	{
		auto it = m_properties.find(name);
		if (it == m_properties.end())
			LOG(ERROR) << "Property \"" << name << "\" is missing!";

		if (it->second.size() < 2)
			LOG(ERROR) << "Property \"" << name << "\" doesn't have 2 components!";

		std::array<Float, 2> values = getVector<2>(it->second);
		return AVector2f(values[0], values[1]);
	}

	AVector2f APropertyList::getVector2f(const std::string &name, const AVector2f &defaultValue) const
	{
		auto it = m_properties.find(name);
		if (it == m_properties.end())
			return defaultValue;

		if (it->second.size() < 2)
			LOG(ERROR) << "Property \"" << name << "\" doesn't have 2 components!";

		std::array<Float, 2> values = getVector<2>(it->second);
		return AVector2f(values[0], values[1]);
	}

	AVector3f APropertyList::getVector3f(const std::string &name) const
	{
		auto it = m_properties.find(name);
		if (it == m_properties.end())
			LOG(ERROR) << "Property \"" << name << "\" is missing!";
		
		if (it->second.size() < 3)
			LOG(ERROR) << "Property \"" << name << "\" doesn't have 3 components!";

		std::array<Float, 3> values = getVector<3>(it->second);
		return AVector3f(values[0], values[1], values[2]);
	}

	AVector3f APropertyList::getVector3f(const std::string &name, const AVector3f &defaultValue) const
	{
		auto it = m_properties.find(name);
		if (it == m_properties.end())
			return defaultValue;

		if (it->second.size() < 3)
			LOG(ERROR) << "Property \"" << name << "\" doesn't have 3 components!";

		std::array<Float, 3> values = getVector<3>(it->second);
		return AVector3f(values[0], values[1], values[2]);
	}

	//----------------------------------------------------APropertyTreeNode-----------------------------------------------------

	std::string APropertyTreeNode::getTypeName() const 
	{ 
		return m_property.getString("Type"); 
	}

	const APropertyList& APropertyTreeNode::getPropertyList() const { return m_property; }

	bool APropertyTreeNode::hasPropertyChild(const std::string &name) const
	{
		for (int i = 0; i < m_children.size(); ++i)
		{
			if (m_children[i].getNodeName() == name)
			{
				return true;
			}
		}
		return false;
	}

	const APropertyTreeNode& APropertyTreeNode::getPropertyChild(const std::string &name) const
	{
		for (int i = 0; i < m_children.size(); ++i)
		{
			if (m_children[i].getNodeName() == name)
			{
				return m_children[i];
			}
		}
		LOG(ERROR) << "No property for " << name;
	}

	void APropertyTreeNode::addProperty(const std::string &name, const std::string &value) 
	{ 
		m_property.set(name, value); 
	}

	void APropertyTreeNode::addProperty(const std::string &name, const std::vector<std::string> &values) 
	{ 
		m_property.set(name, values); 
	}

	void APropertyTreeNode::addChild(const APropertyTreeNode &child) { m_children.push_back(child); }

	//---------------------------------------------------------AObject----------------------------------------------------------

	void AObject::activate() { /* Do nothing */ }

	void AObject::addChild(AObject *child)
	{
		LOG(FATAL) << "NoriObject::addChild() is not implemented for objects of type "
			<< getClassTypeName(getClassType()) << "!";
	}

	void AObject::setParent(AObject *) { /* Do nothing */ }

	//-------------------------------------------AObjectFactory-------------------------------------

	std::map<std::string, AObjectFactory::Constructor> &AObjectFactory::getConstrMap()
	{
		static std::map<std::string, Constructor> constrMap;
		return constrMap;
	}

	void AObjectFactory::registerClass(const std::string &type, const Constructor &constr)
	{
		auto &constrMap = getConstrMap();
		constrMap.insert({ type, constr });
	}

	AObject *AObjectFactory::createInstance(const std::string &type, const APropertyTreeNode &node)
	{
		auto &constrMap = getConstrMap();
		if (constrMap.find(type) == constrMap.end())
			LOG(ERROR) << "A constructor for class \"" << type << "\" could not be found!";
		return constrMap[type](node);
	}

}