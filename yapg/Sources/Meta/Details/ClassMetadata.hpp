#ifndef YAPG_GAME_META_DETAILS_CLASSMETADATA_H
#define YAPG_GAME_META_DETAILS_CLASSMETADATA_H

#include <iostream>

#include "Lua/sol.hpp"
#include "Meta/Details/AttributeMetadataBase.hpp"
#include "Meta/Details/Metadata.hpp"
#include "Meta/Details/MetadataStore.hpp"

namespace yapg
{

namespace meta
{

template<class C, typename T>
class AttributeMetadata;

template<class C, typename T>
class VectorAttributeMetadata;

template<class C, typename T, typename U>
class MapAttributeMetadata;

template<class C>
class ClassMetadata : public Metadata
{
public:
    ClassMetadata() :
        Metadata(),
        m_attributes(),
        m_extraFunction()
    {

    }

    virtual std::type_index getType() const override
    {
        return typeid(C);
    }

    virtual void load(void* object, const sol::object& luaObject) const
    {
        loadImpl(reinterpret_cast<C*>(object), luaObject);
    }

    virtual void loadFromXml(void* value, const tinyxml2::XMLElement* xmlElement, const SerializedEntityGetter& entityGetter) const
    {
        //TODO: Support this for classes
        std::cout << "Compound classes do not support being loaded from XML yet ! (TODO!)" << std::endl;
    }

    virtual void saveToXml(const void* object, tinyxml2::XMLElement* xmlElement, const SerializedEntityGetter& entityGetter) const
    {
        //TODO: Support for this feature for classes
        std::cout << "Compound classes do not support being saved to XML yet ! (TODO!)" << std::endl;
    }

    template<typename T>
    ClassMetadata<C>& declareAttribute(const std::string& name, T C::*member, bool loadableFromLua = true, bool gettableFromLua = true, bool settableFromLua = true)
    {
        m_attributes.emplace(name, std::unique_ptr<AttributeMetadataBase>(new AttributeMetadata<C, T>(member, nullptr, nullptr, loadableFromLua, gettableFromLua, settableFromLua)));
        return *this;
    }

    template<typename T>
    ClassMetadata<C>& declareAttribute(const std::string& name, T(C::*getter)() const = nullptr, void(C::*setter)(T) = nullptr, bool gettableFromLua = true, bool settableFromLua = true)
    {
        m_attributes.emplace(name, std::unique_ptr<AttributeMetadataBase>(new AttributeMetadata<C, T>(nullptr, getter, setter, false, gettableFromLua, settableFromLua)));
        return *this;
    }

    template<typename T>
    ClassMetadata<C>& declareAttribute(const std::string& name, T C::*member, T(C::*getter)() const, void(C::*setter)(T) = nullptr, bool loadableFromLua = true, bool gettableFromLua = true, bool settableFromLua = true)
    {
        m_attributes.emplace(name, std::unique_ptr<AttributeMetadataBase>(new AttributeMetadata<C, T>(member, getter, setter, loadableFromLua, gettableFromLua, settableFromLua)));
        return *this;
    }

    ClassMetadata<C>& setExtraLoadFunction(std::function<void(C*, const sol::object&)> extraFunction)
    {
        m_extraFunction = extraFunction;
        return *this;
    }

    virtual bool hasAttributes() const { return true; }

    virtual const AttributeMetadataBase* getAttribute(const std::string& name) const
    {
        return m_attributes.at(name).get();
    }

    AttributeMetadataBase* getAttribute(const std::string& name)
    {
        return m_attributes.at(name).get();
    }

private:
    void loadImpl(C* object, const sol::object& luaObject) const
    {
        if(luaObject.is<sol::table>())
        {
            const sol::table& luaTable = luaObject.as<sol::table>();
            for(auto it = m_attributes.cbegin(); it != m_attributes.cend(); ++it)
            {
                it->second->load(object, luaTable.get<sol::object>(it->first));
            }
        }

        if(m_extraFunction)
        {
            m_extraFunction(object, luaObject);
        }
    }

    std::map<std::string, std::unique_ptr<AttributeMetadataBase>> m_attributes;
    std::function<void(C*, const sol::object&)> m_extraFunction;
};

}

}

#endif
