#include "PinCushion.h"
#include "Properties.h"
#include <Logging.h>
#include <IconsMaterialDesign.h>
#include <ResourceLib_HM3.h>
#include <Globals.h>
#include <Glacier/Enums.h>
#include <Glacier/Pins.h>
#include <Glacier/ZGameLoopManager.h>
#include <Glacier/ZGeomEntity.h>
#include <Glacier/ZModule.h>
#include <Glacier/ZScene.h>
#include <algorithm>
#include <chrono>
#include <format>
#include <set>
#include <map>
#include <string>

using namespace std::string_literals;

std::set<uint32> permaBlacklist = {
	uint32(ZHMPin::OnDeactivate),
	uint32(ZHMPin::OnTerminate),
	uint32(ZHMPin::OnRelease),
	uint32(ZHMPin::OnWake),
	uint32(ZHMPin::OnValue),
	uint32(ZHMPin::OnTrue),
	uint32(ZHMPin::OnFalse),
	uint32(ZHMPin::OnTriggeredEvent01),
	uint32(ZHMPin::OnTriggeredEvent02),
	uint32(ZHMPin::OnTriggeredEvent03),
	uint32(ZHMPin::OnTriggeredEvent04),
	uint32(ZHMPin::OnTriggeredEvent05),
	uint32(ZHMPin::OnTriggeredEvent06),
	uint32(ZHMPin::OnTriggeredEvent07),
	uint32(ZHMPin::OnTriggeredEvent08),
	uint32(ZHMPin::OnActTimeout),
	uint32(ZHMPin::Out00),
	uint32(ZHMPin::Out01),
	uint32(ZHMPin::Out02),
	uint32(ZHMPin::Out03),
	uint32(ZHMPin::Out04),
	uint32(ZHMPin::Out05),
	uint32(ZHMPin::Out06),
	uint32(ZHMPin::Out07),
	uint32(ZHMPin::Out08),
	uint32(ZHMPin::Out1),
	uint32(ZHMPin::Out2),
	uint32(ZHMPin::Out3),
	uint32(ZHMPin::Out4),
	uint32(ZHMPin::Out5),
	uint32(ZHMPin::Out6),
	uint32(ZHMPin::Out7),
	uint32(ZHMPin::Out8),
	uint32(ZHMPin::Output),
	uint32(ZHMPin::Output1),
	uint32(ZHMPin::Output2),
	uint32(ZHMPin::Output3),
	uint32(ZHMPin::Output4),
	uint32(ZHMPin::Output5),
	uint32(ZHMPin::OutRGB),
	uint32(ZHMPin::OutRGBA),
	uint32(ZHMPin::Vector2),
	uint32(ZHMPin::Vector3),
	uint32(ZHMPin::On),
	uint32(ZHMPin::Off),
	uint32(ZHMPin::PositionOutput),
	uint32(ZHMPin::RotationOutput),
	uint32(ZHMPin::TimeOut),
	uint32(ZHMPin::Done),
	uint32(ZHMPin::Result),
	uint32(ZHMPin::Invert),
	uint32(ZHMPin::Same),
	uint32(ZHMPin::GetTrue),
	uint32(ZHMPin::GetFalse),
	uint32(ZHMPin::GetValue),
	uint32(ZHMPin::PollValue),
	uint32(ZHMPin::SendValue),
	uint32(ZHMPin::Value),
	uint32(ZHMPin::MinValue),
	uint32(ZHMPin::MaxValue),
	uint32(ZHMPin::MinIndex),
	uint32(ZHMPin::MaxIndex),
	uint32(ZHMPin::Distance),
	uint32(ZHMPin::Duration),
	uint32(ZHMPin::Negate),
	uint32(ZHMPin::Color),
	uint32(ZHMPin::Lerp),
	uint32(ZHMPin::Abs),
	uint32(ZHMPin::Power),
	uint32(ZHMPin::Clamped),
	uint32(ZHMPin::Unclamped),
	uint32(ZHMPin::HighClamped),
	uint32(ZHMPin::LowClamped),
	uint32(ZHMPin::DiffusePower),
	uint32(ZHMPin::DistanceChanged),
	uint32(ZHMPin::MainEventEnded),
	uint32(ZHMPin::WentAboveMax),
	uint32(ZHMPin::WentBelowMin),
	//uint32(ZHMPin::WentIntoRange),
	uint32(ZHMPin::PrincipalTargetAngleVert),
	uint32(ZHMPin::PrincipalTargetAngleHoriz),
	uint32(ZHMPin::Outside),
	uint32(ZHMPin::Glow),
	uint32(ZHMPin::FilterOut),
	uint32(ZHMPin::AimLookAt),
	uint32(ZHMPin::Trigger),
	uint32(ZHMPin::TriggerBeforeRaycast),
	uint32(ZHMPin::SecurityCameraAttentionMaxPan),
	uint32(ZHMPin::CrowdAmbientNearestActor),
	uint32(ZHMPin::CrowdAmbientNearestActor_Left),
	uint32(ZHMPin::CrowdAmbientNearestActor_Right),
	uint32(ZHMPin::CrowdAmbientNearestActor_Back),
	0x0007a726, // sends destroyed entities? causes crash
	2417308959,
	2175216505,
	470685528,
	4060967557,
	2488885864,
	3492492454,
};

std::map<ZHMPin, uint32> pinFrequency;

class ZObjectRefAccessible : public ZObjectRef {
public:
	void* GetData() const { return this->m_pData; }
};

static auto ZObjectRefToString(const ZObjectRef& obj, std::string& out) {
	auto type = obj.GetTypeID();
	auto typeInfo = type ? type->typeInfo() : nullptr;

	if (typeInfo) {
		const std::string s_TypeName = typeInfo->m_pTypeName;
		
		PropertyInfo prop;
		bool unk = false;

		if (s_TypeName == "ZString")
			prop = Properties::StringProperty(type, obj.As<ZString>());
		else if (s_TypeName == "bool")
			prop = Properties::BoolProperty(type, obj.As<bool>());
		else if (s_TypeName == "uint8")
			prop = Properties::Uint8Property(type, obj.As<uint8>());
		else if (s_TypeName == "int8")
			prop = Properties::Int8Property(type, obj.As<int8>());
		else if (s_TypeName == "uint16")
			prop = Properties::Uint16Property(type, obj.As<uint16>());
		else if (s_TypeName == "int16")
			prop = Properties::Int16Property(type, obj.As<int16>());
		else if (s_TypeName == "uint32")
			prop = Properties::Uint32Property(type, obj.As<uint32>());
		else if (s_TypeName == "int32")
			prop = Properties::Int32Property(type, obj.As<int32>());
		else if (s_TypeName == "uint64")
			prop = Properties::Uint64Property(type, obj.As<uint64>());
		else if (s_TypeName == "int64")
			prop = Properties::Int64Property(type, obj.As<int64>());
		else if (s_TypeName == "float32")
			prop = Properties::Float32Property(type, obj.As<float32>());
		else if (s_TypeName == "float64")
			prop = Properties::Float64Property(type, obj.As<float64>());
		else if (s_TypeName == "SVector2")
			prop = Properties::SVector2Property(type, obj.As<SVector2>());
		else if (s_TypeName == "SVector3")
			prop = Properties::SVector3Property(type, obj.As<SVector3>());
		else if (s_TypeName == "SVector4")
			prop = Properties::SVector4Property(type, obj.As<SVector4>());
		else if (s_TypeName == "SMatrix43")
			prop = Properties::SMatrix43Property(type, obj.As<SMatrix43>());
		else if (s_TypeName == "SColorRGB")
			prop = Properties::SColorRGBProperty(type, obj.As<SColorRGB>());
		else if (s_TypeName == "SColorRGBA")
			prop = Properties::SColorRGBAProperty(type, obj.As<SColorRGBA>());
		else if (type->typeInfo()->isEnum())
			prop = Properties::EnumProperty(type, reinterpret_cast<const ZObjectRefAccessible&>(obj).GetData());
		else if (type->typeInfo()->isResource())
			prop = Properties::ResourceProperty(type, obj.As<ZResourcePtr>());
		else if (s_TypeName == "ZRepositoryID")
			prop = Properties::ZRepositoryIDProperty(type, obj.As<ZRepositoryID>());
		else if (s_TypeName == "ZDynamicObject")
			prop = Properties::ZDynamicObjectProperty(type, obj.As<ZDynamicObject>());
		//else if (s_TypeName.starts_with("TEntityRef<"))
		//	Properties::TEntityRefProperty(s_InputId, s_Entity, s_Property, s_Data);
		else {
			unk = true;
			prop = Properties::UnsupportedProperty(type, nullptr);
		}

		out = unk ? prop.ToString() : std::format("{}  ({})", prop.ToString(), s_TypeName);
		return;
	}

	out = "";
}

static auto getEntityLeafName(ZEntityRef entity) -> std::string {
	auto s_Factory = reinterpret_cast<ZTemplateEntityBlueprintFactory*>(entity.GetBlueprintFactory());
	auto type = entity->GetType();
	if (!type) return "???";
	const auto& s_Interfaces = *type->m_pInterfaces;
	return std::string(s_Interfaces[0].m_pTypeId->typeInfo()->m_pTypeName);

}

static auto getEntityTree(ZEntityRef entity) -> std::string {
	if (!entity) return "";
	if (entity.GetLogicalParent())
		return getEntityTree(entity.GetLogicalParent()) + " > " + getEntityLeafName(entity);
	return getEntityLeafName(entity);
}

static auto displayProperties(PinCallData& call) -> void {
	for (auto& prop : call.props) {
		ImGui::PushFont(SDK()->GetImGuiBoldFont());

		ImGui::TextUnformatted(prop.name.c_str());

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("%s", prop.typeName.c_str());

		ImGui::PopFont();
		ImGui::SameLine();

		ImGui::PushItemWidth(-1);

		if (prop.rgb)
			ImGui::ColorEdit3(prop.inputId.c_str(), &prop.rgb->r, ImGuiColorEditFlags_NoInputs);
		else if (prop.rgba)
			ImGui::ColorEdit4(prop.inputId.c_str(), &prop.rgba->r, ImGuiColorEditFlags_NoInputs);
		else if (prop.enumValue) {
			auto& enumVal = *prop.enumValue;
			std::string s_CurrentValue;

			for (auto& s_EnumValue : enumVal.type->m_entries) {
				if (s_EnumValue.m_nValue != enumVal.value) continue;
				s_CurrentValue = s_EnumValue.m_pName;
			}

			if (ImGui::BeginCombo(prop.inputId.c_str(), s_CurrentValue.c_str())) {
				for (auto& s_EnumValue : enumVal.type->m_entries)
					ImGui::Selectable(s_EnumValue.m_pName, s_EnumValue.m_nValue == enumVal.value);
				ImGui::EndCombo();
			}
		}
		else {
			auto& str = prop.ToString();
			if (str.empty())
				ImGui::LabelText((prop.inputId + "t").c_str(), "%s", "<error>");
			else
				ImGui::LabelText(prop.inputId.c_str(), "%s", str.c_str());
		}
		ImGui::Separator();
	}
}

PinCushion::PinCushion() {
}

PinCushion::~PinCushion() {
	// Unregister our frame update function when the mod unloads.
	const ZMemberDelegate<PinCushion, void(const SGameUpdateEvent&)> s_Delegate(this, &PinCushion::OnFrameUpdate);
	Globals::GameLoopManager->UnregisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdateAlways);
	Hooks::ZEntitySceneContext_LoadScene->RemoveDetour(&PinCushion::OnLoadScene);
}

void PinCushion::OnEngineInitialized() {
	Logger::Info("PinCushion has been initialized!");

	// Register a function to be called on every game frame while the game is in play mode.
	const ZMemberDelegate<PinCushion, void(const SGameUpdateEvent&)> s_Delegate(this, &PinCushion::OnFrameUpdate);
	Globals::GameLoopManager->RegisterFrameUpdate(s_Delegate, 1, EUpdateMode::eUpdateAlways);

	// Install a hook to print the name of the scene every time the game loads a new one.
	Hooks::ZEntitySceneContext_LoadScene->AddDetour(this, &PinCushion::OnLoadScene);

	Hooks::SignalOutputPin->AddDetour(this, &PinCushion::OnPinOutput);
	//Hooks::SignalInputPin->AddDetour(this, &PinCushion::OnPinInput);
}

void PinCushion::OnDrawMenu() {
	// Toggle our message when the user presses our button.
	if (ImGui::Button(ICON_MD_PUSH_PIN " PINS")) {
		m_ShowMessage = !m_ShowMessage;
	}
}

void PinCushion::OnDrawUI(bool p_HasFocus) {
	if (!m_ShowMessage)
		return;

	if (ImGui::Begin("PIN CUSHION", &m_ShowMessage)) {
		static size_t selected = 0;
		static std::string titleBuff;

		auto lock = std::unique_lock(displayDataLock);

		ImGui::Checkbox("Rate Blocking", &this->enableRateBlock);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(120);
		if (ImGui::InputInt("Rate Limit", &uiRateLimit, 1, 120))
			this->updateDataAction = UpdateDataAction::RateLimit;
		if (ImGui::BeginItemTooltip()) {
			ImGui::TextUnformatted("The number of times per-second a pin is allowed to be fired without being blocked. This is checked every 3 seconds, so one-off 'rapid-fire' pins may be spared if this number is not low.");
			ImGui::EndTooltip();
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset Blacklist") && !this->haveUpdateDataAction())
			this->updateDataAction = UpdateDataAction::ClearBlacklist;

		auto frozen = !frozenPinData.empty();
		auto& activeList = frozen ? frozenPinData : displayPinData;

		ImGui::SameLine();
		if (ImGui::Button(frozen ? "Unfreeze" : "Freeze") && !this->haveUpdateDataAction())
			this->updateDataAction = UpdateDataAction::ToggleFreeze;
		ImGui::SameLine();
		if (ImGui::Button("Clear") && !this->haveUpdateDataAction())
			this->updateDataAction = UpdateDataAction::Clear;

		ImGui::BeginChild("left pane", ImVec2(300, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

		if (ImGui::InputText("Filter", filterInput, sizeof(filterInput))) {
			auto lock = std::unique_lock(filterInputLock);
			filterInputSV = filterInput;
		}
			
		size_t current = 0;

		if (activeList.empty()) {
			ImGui::TextUnformatted("No Data");
		}
		else {
			for (auto it = activeList.begin(); it != activeList.end(); ++it, ++current) {
				auto& data = *it;
				auto title = data.name.c_str();

				if (data.calls.size() > 1) {
					titleBuff = title;
					titleBuff += " (" + std::to_string(data.timesCalled) + ")";
					title = titleBuff.c_str();
				}
				if (ImGui::Selectable(title, selected == current))
					selected = current;
			}
		}

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginGroup();
		ImGui::BeginChild("pin view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

		if (!activeList.empty()) {
			auto it = activeList.begin();
			std::advance(it, selected);

			if (selected > 0 && selected >= activeList.size())
				selected = activeList.size() - 1;

			ImGui::SameLine();

			if (it != activeList.end() && ImGui::Button("Blacklist") && !this->haveUpdateDataAction()) {
				this->updateDataAction = UpdateDataAction::Blacklist;
				this->blacklistPin = static_cast<ZHMPin>(it->id);
			}

			auto pinIt = activeList.begin();
			std::advance(pinIt, selected < (activeList.size() - 1) ? selected : activeList.size() - 1);
			auto& pin = *pinIt;
			current = 0;

			ImGui::TextUnformatted("Pin Name: ");
			ImGui::SameLine();
			ImGui::TextUnformatted(pin.name.c_str());

			ImGui::NewLine();

			for (auto it = pin.calls.begin(); current < std::min<size_t>(pin.calls.size(), 5) && it != pin.calls.end(); ++it, ++current) {
				auto& call = *it;

				ImGui::Indent(20);

				ImGui::TextUnformatted("Data: ");
				ImGui::SameLine();
				ImGui::TextUnformatted(call.data.c_str());

				ImGui::TextUnformatted(call.entityId.c_str());
				ImGui::Text("Entity Name: %s", call.entityName.c_str());
				ImGui::Text("Entity Type: %s", call.entityType.empty() ? "(none)" : call.entityType.c_str());

				ImGui::Text("Entity Tree: %s", call.entityTree.c_str());

				ImGui::TextUnformatted("Entity Props");

				displayProperties(call);

				ImGui::Separator();
			}
		}

		ImGui::EndChild();
		ImGui::EndGroup();
	}
	ImGui::End();
}

void PinCushion::OnFrameUpdate(const SGameUpdateEvent &p_UpdateEvent) {
	if (this->haveUpdateDataAction()) {
		auto lock = std::unique_lock(displayDataLock);
		if (uiRateLimit != rateLimit)
			rateLimit = uiRateLimit;
		switch (this->getUpdateDataAction()) {
		case UpdateDataAction::Clear:
			pinData.clear();
			break;
		case UpdateDataAction::Blacklist: {
			pinBlacklist.insert(blacklistPin);
			auto it = std::find_if(pinData.begin(), pinData.end(), [this](const PinData& v) { return static_cast<ZHMPin>(v.id) == this->blacklistPin; });
			if (it != pinData.end())
				pinData.erase(it);
			auto displayIt = std::find_if(displayPinData.begin(), displayPinData.end(), [this](const PinData& v) { return static_cast<ZHMPin>(v.id) == this->blacklistPin; });
			if (displayIt != displayPinData.end())
				displayPinData.erase(displayIt);
			auto frozenIt = std::find_if(frozenPinData.begin(), frozenPinData.end(), [this](const PinData& v) { return static_cast<ZHMPin>(v.id) == this->blacklistPin; });
			if (frozenIt != frozenPinData.end())
				frozenPinData.erase(frozenIt);
			break;
		}
		case UpdateDataAction::ClearBlacklist:
			pinBlacklist.clear();
			break;
		case UpdateDataAction::ToggleFreeze:
			if (!frozenPinData.empty())
				frozenPinData.clear();
			else
				std::copy(displayPinData.begin(), displayPinData.end(), std::back_inserter(frozenPinData));
			break;
		}

		this->updateDataAction = UpdateDataAction::None;
	}

	auto now = std::chrono::system_clock::now();
	if (this->enableRateBlock) {
		auto secs = std::chrono::duration<double>(now - this->lastCleanupTime).count();

		if (secs >= 3) {
			for (auto freqIt = pinFrequency.begin(); freqIt != pinFrequency.end(); ++freqIt) {
				if (freqIt->second < static_cast<uint64>(secs / 3) * rateLimit) continue;
				auto it = std::find_if(this->pinData.begin(), this->pinData.end(), [freqIt](const PinData& v) { return static_cast<ZHMPin>(v.id) == freqIt->first; });
				if (it != this->pinData.end())
					this->pinData.erase(it);
				this->pinBlacklist.insert(freqIt->first);
				freqIt = pinFrequency.erase(freqIt);
				if (freqIt == pinFrequency.end()) break;
			}

			this->lastCleanupTime = now;
			this->lastFreqPruneTime = secs;
		}
	}

	auto secsSinceUpdate = std::chrono::duration<double>(now - this->lastDisplayUpdateTime).count();
	if (secsSinceUpdate > .15) {
		auto lock = std::unique_lock(displayDataLock);
		auto filterLock = std::shared_lock(filterInputLock);
		this->displayPinData.clear();
		for (auto& data : this->pinData) {
			auto filterThisPin = !filterInputSV.empty() && !data.name.starts_with(filterInputSV);
			if (filterThisPin) continue;
			this->displayPinData.push_back(data);
		}
		this->lastDisplayUpdateTime = now;
	}
}

DEFINE_PLUGIN_DETOUR(PinCushion, void, OnLoadScene, ZEntitySceneContext* th, ZSceneData& p_SceneData) {
	Logger::Debug("Loading scene: {}", p_SceneData.m_sceneName);
	//pinData.clear();
	return HookResult<void>(HookAction::Continue());
}

DEFINE_PLUGIN_DETOUR(PinCushion, bool, OnPinOutput, ZEntityRef entity, uint32 pinId, const ZObjectRef& data) {
	static ZString zPinName;

	if (!m_ShowMessage || pinBlacklist.contains(static_cast<ZHMPin>(pinId)) || permaBlacklist.contains(pinId))
		return HookAction::Continue();

	PinCallData callData;

	ZObjectRefToString(data, callData.data);

	const auto s_SceneCtx = Globals::Hitman5Module->m_pEntitySceneContext;
	if (!s_SceneCtx || !s_SceneCtx->m_pScene) return HookAction::Continue();

	auto s_EntityType = entity->GetType();

	// The way to get the factory here is probably wrong.
	auto s_Factory = reinterpret_cast<ZTemplateEntityBlueprintFactory*>(entity.GetBlueprintFactory());

	if (entity.GetOwningEntity())
		s_Factory = reinterpret_cast<ZTemplateEntityBlueprintFactory*>(entity.GetOwningEntity().GetBlueprintFactory());

	if (s_Factory) {
		// This is also probably wrong.
		auto s_Index = s_Factory->GetSubEntityIndex(entity->GetType()->m_nEntityId);

		if (s_Index != -1 && s_Factory->m_pTemplateEntityBlueprint)
			callData.entityName = s_Factory->m_pTemplateEntityBlueprint->subEntities[s_Index].entityName;
	}

	const auto& s_Interfaces = *entity->GetType()->m_pInterfaces;
	callData.entityId = fmt::format("Entity ID: {:016x}", entity->GetType()->m_nEntityId);
	callData.entityType = s_Interfaces[0].m_pTypeId->typeInfo()->m_pTypeName;
	callData.entityTree = getEntityTree(entity);

	if (s_EntityType && s_EntityType->m_pProperties01) {
		for (uint32_t i = 0; i < s_EntityType->m_pProperties01->size(); ++i) {
			ZEntityProperty* s_Property = &s_EntityType->m_pProperties01->operator[](i);
			if (!s_Property->m_pType)
				continue;

			const auto* s_PropertyInfo = s_Property->m_pType->getPropertyInfo();

			if (!s_PropertyInfo || !s_PropertyInfo->m_pType)
				continue;

			const auto s_PropertyAddress = reinterpret_cast<uintptr_t>(entity.m_pEntity) + s_Property->m_nOffset;
			const auto s_PropertyType = s_PropertyInfo->m_pType;
			const auto s_TypeInfo = s_PropertyType->typeInfo();
			const uint16_t s_TypeSize = s_TypeInfo->m_nTypeSize;
			const uint16_t s_TypeAlignment = s_TypeInfo->m_nTypeAlignment;

			auto* s_Data = (*Globals::MemoryManager)->m_pNormalAllocator->AllocateAligned(s_TypeSize, s_TypeAlignment);

			if (s_PropertyInfo->m_nFlags & EPropertyInfoFlags::E_HAS_GETTER_SETTER)
				s_PropertyInfo->get(reinterpret_cast<void*>(s_PropertyAddress), s_Data, s_PropertyInfo->m_nOffset);
			else
				s_TypeInfo->m_pTypeFunctions->copyConstruct(s_Data, reinterpret_cast<void*>(s_PropertyAddress));

			const std::string s_TypeName = s_TypeInfo->m_pTypeName;
			const std::string s_InputId = std::format("##Property{}", i);

			PropertyInfo prop;

			if (s_TypeName == "ZString")
				prop = Properties::StringProperty(s_PropertyType, s_Data);
			else if (s_TypeName == "bool")
				prop = Properties::BoolProperty(s_PropertyType, s_Data);
			else if (s_TypeName == "uint8")
				prop = Properties::Uint8Property(s_PropertyType, s_Data);
			else if (s_TypeName == "int8")
				prop = Properties::Int8Property(s_PropertyType, s_Data);
			else if (s_TypeName == "uint16")
				prop = Properties::Uint16Property(s_PropertyType, s_Data);
			else if (s_TypeName == "int16")
				prop = Properties::Int16Property(s_PropertyType, s_Data);
			else if (s_TypeName == "uint32")
				prop = Properties::Uint32Property(s_PropertyType, s_Data);
			else if (s_TypeName == "int32")
				prop = Properties::Int32Property(s_PropertyType, s_Data);
			else if (s_TypeName == "uint64")
				prop = Properties::Uint64Property(s_PropertyType, s_Data);
			else if (s_TypeName == "int64")
				prop = Properties::Int64Property(s_PropertyType, s_Data);
			else if (s_TypeName == "float32")
				prop = Properties::Float32Property(s_PropertyType, s_Data);
			else if (s_TypeName == "float64")
				prop = Properties::Float64Property(s_PropertyType, s_Data);
			else if (s_TypeName == "SVector2")
				prop = Properties::SVector2Property(s_PropertyType, s_Data);
			else if (s_TypeName == "SVector3")
				prop = Properties::SVector3Property(s_PropertyType, s_Data);
			else if (s_TypeName == "SVector4")
				prop = Properties::SVector4Property(s_PropertyType, s_Data);
			else if (s_TypeName == "SMatrix43")
				prop = Properties::SMatrix43Property(s_PropertyType, s_Data);
			else if (s_TypeName == "SColorRGB")
				prop = Properties::SColorRGBProperty(s_PropertyType, s_Data);
			else if (s_TypeName == "SColorRGBA")
				prop = Properties::SColorRGBAProperty(s_PropertyType, s_Data);
			else if (s_TypeName == "ZRepositoryID")
				prop = Properties::ZRepositoryIDProperty(s_PropertyType, static_cast<ZRepositoryID*>(s_Data));
			else if (s_TypeName == "ZDynamicObject")
				prop = Properties::ZDynamicObjectProperty(s_PropertyType, static_cast<ZDynamicObject*>(s_Data));
			else if (s_PropertyInfo->m_pType->typeInfo()->isEnum())
				prop = Properties::EnumProperty(s_PropertyType, s_Data);
			else if (s_PropertyInfo->m_pType->typeInfo()->isResource())
				prop = Properties::ResourceProperty(s_PropertyType, s_Data);
			//else if (s_TypeName.starts_with("TEntityRef<"))
			//	Properties::TEntityRefProperty(s_InputId, s_Entity, s_Property, s_Data);
			else
				prop = Properties::UnsupportedProperty(s_PropertyType, s_Data);

			prop.typeName = s_TypeName;
			prop.inputId = s_InputId;

			(*Globals::MemoryManager)->m_pNormalAllocator->Free(s_Data);

			prop.hasNoDirectName = s_PropertyInfo->m_pType->typeInfo()->isResource() || s_PropertyInfo->m_nPropertyID != s_Property->m_nPropertyId;
			if (prop.hasNoDirectName) {
				const auto s_PropertyName = HM3_GetPropertyName(s_Property->m_nPropertyId);

				prop.name = s_PropertyName.Size > 0
					? std::string(s_PropertyName.Data, s_PropertyName.Size)
					: std::format("~{:#08x}", s_Property->m_nPropertyId);
			}
			else prop.name = s_PropertyInfo->m_pName;

			callData.props.push_back(std::move(prop));
		}
	}

	if (this->enableRateBlock) {
		auto freqIt = pinFrequency.find(static_cast<ZHMPin>(pinId));
		if (freqIt == pinFrequency.end()) freqIt = pinFrequency.emplace(static_cast<ZHMPin>(pinId), 1).first;
		else ++freqIt->second;
	}

	auto lastPin = getRecentPinIterator(pinId);

	if (lastPin != pinData.end()) {
		++lastPin->timesCalled;

		lastPin->calls.push_front(std::move(callData));
		if (lastPin->calls.size() > 10)
			lastPin->calls.resize(10);

		if (lastPin != pinData.begin()) {
			pinData.push_front(std::move(*lastPin));
			pinData.erase(lastPin);
		}

		return HookAction::Continue();
	}

	zPinName = "";
	auto name = SDK()->GetPinName(pinId, zPinName) ? std::string(zPinName) : std::to_string(pinId);
	auto filterThisPin = false;
	{
		auto lock = std::shared_lock(filterInputLock);
		filterThisPin = !filterInputSV.empty() && !name.starts_with(filterInputSV);
	}

	if (!filterThisPin) {
		PinData pin;
		pin.id = pinId;
		pin.name = name;
		pin.calls.push_front(std::move(callData));
		pinData.push_front(std::move(pin));
		if (pinData.size() > 200)
			pinData.resize(200);
	}
	return HookAction::Continue();
}

DECLARE_ZHM_PLUGIN(PinCushion);
