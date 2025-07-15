#pragma once
#include <Glacier/IEnumType.h>
#include <Glacier/SColorRGB.h>
#include <Glacier/SColorRGBA.h>
#include <Glacier/ZEntity.h>
#include <Glacier/ZMath.h>
#include <Glacier/ZObject.h>
#include <Glacier/ZResource.h>
#include <string>
#include <optional>

struct PropertyInfo_EnumValue {
    int32 value;
    IEnumType* type;

    PropertyInfo_EnumValue(IEnumType* type, int32 value) : type(type), value(value)
    { }
};

class PropertyInfo {
public:
    std::string name;
    std::string typeName;
    std::string inputId;
    std::optional<std::string> primitiveValue;
    std::optional<SColorRGB> rgb;
    std::optional<SColorRGBA> rgba;
    std::optional<PropertyInfo_EnumValue> enumValue;
    std::optional<SMatrix43> matrixValue;
    std::optional<SVector2> vec2Value;
    std::optional<SVector3> vec3Value;
    std::optional<SVector4> vec4Value;
    std::optional<std::string> str;
    bool hasNoDirectName = false;

    PropertyInfo() = default;

    PropertyInfo(const SColorRGB& value) : rgb(value)
    { }

    PropertyInfo(const SColorRGBA& value) : rgba(value)
    { }

    PropertyInfo(std::string&& value) : primitiveValue(value)
    { }

    PropertyInfo(const PropertyInfo_EnumValue& value) : enumValue(value)
    { }

    PropertyInfo(const SMatrix43& value) : matrixValue(value)
    { }

    PropertyInfo(const SVector2& value) : vec2Value(value)
    { }

    PropertyInfo(const SVector3& value) : vec3Value(value)
    { }

    PropertyInfo(const SVector4& value) : vec4Value(value)
    { }

    const std::string& ToString() {
        if (!str.has_value()) {
            str = "<error>";
            if (this->primitiveValue)
                str = this->primitiveValue;
            else if (this->vec2Value)
                str = std::format("{}, {}", this->vec2Value->x, this->vec2Value->y);
            else if (this->vec3Value)
                str = std::format("{}, {}, {}", this->vec3Value->x, this->vec3Value->y, this->vec3Value->z);
            else if (this->vec4Value)
                str = std::format("{}, {}, {}, {}", this->vec4Value->x, this->vec4Value->y, this->vec4Value->z, this->vec4Value->w);
            else if (this->rgb)
                str = std::format("RGB: {}, {}, {}", this->rgb->r, this->rgb->g, this->rgb->b);
            else if (this->rgba)
                str = std::format("RGBA: {}, {}, {}", this->rgba->r, this->rgba->g, this->rgba->b, this->rgba->a);
            else if (this->enumValue) {
                auto& enumVal = *this->enumValue;
                str = "";

                for (auto& s_EnumValue : enumVal.type->m_entries) {
                    if (s_EnumValue.m_nValue != enumVal.value) continue;
                    str = s_EnumValue.m_pName;
                }

                if (str->empty()) str = std::to_string(enumVal.value);
            }
            else if (this->matrixValue) {
                str = std::format("x: {}, y: {}, z: {}, t: {}", this->matrixValue->XAxis.x, this->matrixValue->YAxis.x, this->matrixValue->ZAxis.x, this->matrixValue->Trans.x);
            }
        }
        return *str;
    }
};

class Properties {
public:
    // Properties
    static PropertyInfo UnsupportedProperty(STypeID* p_Property, void* p_Data);

    // Primitive properties.
    static PropertyInfo StringProperty(STypeID* p_Property, void* p_Data);
    static PropertyInfo BoolProperty(STypeID* p_Property, void* p_Data);
    static PropertyInfo Uint8Property(STypeID* p_Property, void* p_Data);
    static PropertyInfo Uint16Property(STypeID* p_Property, void* p_Data);
    static PropertyInfo Uint32Property(STypeID* p_Property, void* p_Data);
    static PropertyInfo Uint64Property(STypeID* p_Property, void* p_Data);
    static PropertyInfo Int8Property(STypeID* p_Property, void* p_Data);
    static PropertyInfo Int16Property(STypeID* p_Property, void* p_Data);
    static PropertyInfo Int32Property(STypeID* p_Property, void* p_Data);
    static PropertyInfo Int64Property(STypeID* p_Property, void* p_Data);
    static PropertyInfo Float32Property(STypeID* p_Property, void* p_Data);
    static PropertyInfo Float64Property(STypeID* p_Property, void* p_Data);
    static PropertyInfo EnumProperty(STypeID* p_Property, void* p_Data);

    // Vector properties.
    static PropertyInfo SVector2Property(STypeID* p_Property, void* p_Data);
    static PropertyInfo SVector3Property(STypeID* p_Property, void* p_Data);
    static PropertyInfo SVector4Property(STypeID* p_Property, void* p_Data);

    static PropertyInfo SMatrix43Property(STypeID* p_Property, void* p_Data);

    static PropertyInfo SColorRGBProperty(STypeID* p_Property, void* p_Data);
    static PropertyInfo SColorRGBAProperty(STypeID* p_Property, void* p_Data);

	static PropertyInfo ZRepositoryIDProperty(STypeID* p_Property, void* p_Data);

    static PropertyInfo ResourceProperty(STypeID* p_Property, void* p_Data);
};