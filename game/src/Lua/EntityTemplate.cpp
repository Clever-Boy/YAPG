#include "Lua/EntityTemplate.hpp"

#include "Components/Component.hpp"
#include "Components/CustomDataComponent.hpp"
#include "Components/TemplateComponent.hpp"
#include "Lua/EntityHandle.hpp"
#include "Lua/EntityParametersHelper.hpp"
#include "Lua/LuaState.hpp"
#include "Settings/tinyxml2.h"

namespace lua
{

EntityTemplate::EntityTemplate(const sol::table& templateTable) :
    m_name(),
    m_friendlyName(),
    m_inheritedTemplate(),
    m_abstract(false),
    m_parameters(),
    m_componentsTable()
{
    m_name = templateTable.get<std::string>("name");
    m_friendlyName = templateTable.get<std::string>("friendlyname");

    if(templateTable.get<sol::object>("inherits").is<std::string>())
        m_inheritedTemplate = templateTable.get<std::string>("inherits");

    m_componentsTable = templateTable.get<sol::table>("components");

    if(templateTable.get<sol::object>("abstract").is<bool>())
        m_abstract = templateTable.get<bool>("abstract");

    //Load the parameters
    if(templateTable.get<sol::object>("parameters").is<sol::table>())
    {
        templateTable.get<sol::table>("parameters").for_each([&](const sol::object &key, const sol::object &value)
        {
            sol::table valueTable = value.as<sol::table>();

            if(valueTable.get<sol::object>("custom_data_field").valid())
            {
                //It's a custom data parameter (a parameter that affects a value to a data stored in CustomDataComponent : can be any type of data)
                m_parameters.emplace(
                    key.as<std::string>(),
                    Parameter{
                        key.as<std::string>(),
                        valueTable.get<std::string>("name"),
                        Parameter::CustomDataFieldParameter,
                        "",
                        "",
                        valueTable.get<std::string>("custom_data_field")
                    }
                );
            }
            else
            {
                //It's a parameter to a component's attribute
                m_parameters.emplace(
                    key.as<std::string>(),
                    Parameter{
                        key.as<std::string>(),
                        valueTable.get<std::string>("name"),
                        Parameter::ComponentAttributeParameter,
                        valueTable.get<std::string>("component"),
                        valueTable.get<std::string>("attribute"),
                        ""
                    }
                );
            }
        });
    }
}

void EntityTemplate::applyInheritance(LuaState& luaState)
{
    if(!m_inheritedTemplate.empty())
    {
        //Merge the parameters
        m_parameters.insert(
            luaState.getTemplate(m_inheritedTemplate).m_parameters.begin(),
            luaState.getTemplate(m_inheritedTemplate).m_parameters.end()
        );

        //Merge the component lua table
        m_componentsTable = luaState.mergeTables(luaState.getTemplate(m_inheritedTemplate).m_componentsTable, m_componentsTable);
    }
}

void EntityTemplate::initializeEntity(entityx::Entity entity, const level::SerializedEntityGetter& entityGetter, bool templateComponent) const
{
    if(isAbstract())
        throw std::runtime_error("[Template/Error] Trying to instanciate \"" + getName() + "\", which is an abstract template!");

    //Add each components to the entity
    m_componentsTable.for_each([&](const sol::object& key, const sol::object& value) {
        std::string componentType = key.as<std::string>();

        components::Component::assignComponent(entity, componentType, value, entityGetter);
    });

    //Add the CustomData component (special case!)
    entity.assign<components::CustomDataComponent>(entity);
    if(m_componentsTable.get<sol::object>("custom_data").is<sol::table>())
    {
        //If the template has CustomDataComponent defined, it means that he needs to predefine some values
        entity.component<components::CustomDataComponent>()->loadFromLua(
            m_componentsTable.get<sol::object>("custom_data"),
            entityGetter
        );
    }

    if(templateComponent)
    {
        //Add the template component, containing infos about the template
        entity.assign<components::TemplateComponent>(
            entity,
            entityGetter.getSerializationIdFromEntity(entity),
            getName(),
            lua::EntityParametersHelper(this, entity)
        );
    }
}

void EntityTemplate::initializeEntity(entityx::Entity entity, const level::SerializedEntityGetter& entityGetter, const tinyxml2::XMLElement* parametersElement, bool templateComponent) const
{
    initializeEntity(entity, entityGetter, templateComponent);
    for(auto it = m_parameters.cbegin(); it != m_parameters.cend(); ++it)
    {
        try
        {
            const tinyxml2::XMLElement* parameterElement = parametersElement->FirstChildElement(it->first.c_str());
            if(!parameterElement)
            {
                throw std::runtime_error("Template \"" + m_name + "\" needs the parameter \"" + it->first + "\" but not given by the instanciated object !");
            }

            //Load the corresponding parameter (from parameter infos) using the XML element
            if(it->second.parameterType == Parameter::ComponentAttributeParameter)
            {
                EntityHandle(entity).loadAttributeFromXml(it->second.component, it->second.attribute, parameterElement, entityGetter);
            }
            else
            {
                if(EntityHandle(entity).getCustomData()->hasValue(it->second.field)) //Note: the template must have already defined the custom data value.
                {
                    boost::any value = EntityHandle(entity).getCustomData()->getValue(it->second.field);
                    meta::MetadataStore::getMetadata(value.type()).loadFromXml(boost::unsafe_any_cast<void*>(&value), parameterElement, entityGetter);
                    EntityHandle(entity).getCustomData()->setValue(it->second.field, value);
                }
            }
        }
        catch(std::exception& e)
        {
            std::cout << "[Lua/Warning] " << e.what() << std::endl;
        }
    }
}

void EntityTemplate::saveEntity(entityx::Entity entity, const level::SerializedEntityGetter& entityGetter, tinyxml2::XMLElement* parametersElement) const
{
    tinyxml2::XMLDocument* doc = parametersElement->GetDocument();

    for(auto it = m_parameters.cbegin(); it != m_parameters.cend(); ++it)
    {
        tinyxml2::XMLElement* parameterElement = doc->NewElement(it->first.c_str());
        parametersElement->LinkEndChild(parameterElement);

        if(it->second.parameterType == Parameter::ComponentAttributeParameter)
        {
            EntityHandle(entity).saveAttributeToXml(it->second.component, it->second.attribute, parameterElement, entityGetter);
        }
        else
        {
            if(EntityHandle(entity).getCustomData()->hasValue(it->second.field))
            {
                boost::any value = EntityHandle(entity).getCustomData()->getValue(it->second.field);
                meta::MetadataStore::getMetadata(value.type()).saveToXml(boost::unsafe_any_cast<void*>(&value), parameterElement, entityGetter);
            }
        }
    }
}

bool EntityTemplate::isPlayer() const
{
    return getComponentsTable().get<sol::object>("player").is<sol::table>();
}

}
