#include "Properties.h"
#include <format>
#include <imgui.h>
#include <string>

PropertyInfo Properties::UnsupportedProperty(STypeID* p_Type, void* p_Data) {
    return "<" + std::string(p_Type->typeInfo()->m_pTypeName) + ">";
}

PropertyInfo Properties::StringProperty(STypeID* p_Type, void* p_Data) {
    auto* s_RealData = static_cast<ZString*>(p_Data);
    const auto s_StringSize = min(s_RealData->size(), 300);
    return std::string(s_RealData->c_str(), s_StringSize);
}

PropertyInfo Properties::BoolProperty(STypeID* p_Type, void* p_Data) {
    auto s_Value = *static_cast<bool*>(p_Data);
    return PropertyInfo{s_Value ? "true" : "false"};
}

PropertyInfo Properties::Uint8Property(STypeID* p_Type, void* p_Data) {
    auto s_Value = *static_cast<uint8*>(p_Data);
    return std::format("{}", s_Value);
}

PropertyInfo Properties::Uint16Property(STypeID* p_Type, void* p_Data) {
    auto s_Value = *static_cast<uint16*>(p_Data);
    return std::format("{}", s_Value);
}

PropertyInfo Properties::Uint32Property(STypeID* p_Type, void* p_Data) {
    auto s_Value = *static_cast<uint32*>(p_Data);
    return std::format("{}", s_Value);
}

PropertyInfo Properties::Uint64Property(STypeID* p_Type, void* p_Data) {
    auto s_Value = *static_cast<uint64*>(p_Data);
    return std::format("{}", s_Value);
}

PropertyInfo Properties::Int8Property(STypeID* p_Type, void* p_Data) {
    auto s_Value = *static_cast<int8*>(p_Data);
    return std::format("{}", s_Value);
}

PropertyInfo Properties::Int16Property(STypeID* p_Type, void* p_Data) {
    auto s_Value = *static_cast<int16*>(p_Data);
    return std::format("{}", s_Value);
}

PropertyInfo Properties::Int32Property(STypeID* p_Type, void* p_Data) {
    auto s_Value = *static_cast<int32*>(p_Data);
    return std::format("{}", s_Value);
}

PropertyInfo Properties::Int64Property(STypeID* p_Type, void* p_Data) {
    auto s_Value = *static_cast<int64*>(p_Data);
    return std::format("{}", s_Value);
}

PropertyInfo Properties::Float32Property(STypeID* p_Type, void* p_Data) {
    auto s_Value = *static_cast<float32*>(p_Data);
    return std::format("{}", s_Value);
}

PropertyInfo Properties::Float64Property(STypeID* p_Type, void* p_Data) {
    auto s_Value = *static_cast<float64*>(p_Data);
    return std::format("{}", s_Value);
}

PropertyInfo Properties::SColorRGBProperty(STypeID* p_Type, void* p_Data) {
    return *static_cast<SColorRGB*>(p_Data);
}

PropertyInfo Properties::SColorRGBAProperty(STypeID* p_Type, void* p_Data) {
    return *static_cast<SColorRGBA*>(p_Data);
}

PropertyInfo Properties::EnumProperty(STypeID* p_Type, void* p_Data) {
    auto s_Value = *static_cast<int32*>(p_Data);
    auto s_Type = reinterpret_cast<IEnumType*>(p_Type->typeInfo());
    return PropertyInfo_EnumValue{s_Type, s_Value};
}

PropertyInfo Properties::SMatrix43Property(STypeID* p_Type, void* p_Data) {
    return *static_cast<SMatrix43*>(p_Data);
}

PropertyInfo Properties::ResourceProperty(STypeID* p_Type, void* p_Data) {
    auto* s_Resource = static_cast<ZResourcePtr*>(p_Data);
    std::string s_ResourceName = "null";
    if (s_Resource && s_Resource->m_nResourceIndex >= 0)
        s_ResourceName = std::format(" {}", s_Resource->GetResourceInfo().rid.GetID());
    return s_ResourceName;
}

PropertyInfo Properties::SVector2Property(STypeID* p_Type, void* p_Data) {
    return *static_cast<SVector2*>(p_Data);
}

PropertyInfo Properties::SVector3Property(STypeID* p_Type, void* p_Data) {
    return *static_cast<SVector3*>(p_Data);
}

PropertyInfo Properties::SVector4Property(STypeID* p_Type, void* p_Data) {
    return *static_cast<SVector4*>(p_Data);
}

PropertyInfo Properties::ZRepositoryIDProperty(STypeID* p_Type, void* p_Data) {
    return std::string(static_cast<ZRepositoryID*>(p_Data)->ToString());
}