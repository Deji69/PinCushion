#pragma once
#define NOMINMAX
#include "Properties.h"
#include <IPluginInterface.h>
#include <Glacier/Pins.h>
#include <Glacier/SGameUpdateEvent.h>
#include <Glacier/ZEntity.h>
#include <Glacier/ZObject.h>
#include <atomic>
#include <chrono>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <set>
#include <string>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#ifdef MAX
#undef MAX
#endif

#ifdef MIN
#undef MIN
#endif

struct PinCallData {
	std::string entityId;
	std::string entityName;
	std::string entityType;
	std::string entityTree;
	std::string data;
	std::vector<PropertyInfo> props;
};

struct PinData {
	uint32 id = -1;
	uint32 lastCheckedTimesCalled = 0;
	uint64 timesCalled = 1;
	double checkedDelta = 0;
	std::string name;
	std::list<PinCallData> calls;
};

enum class UpdateDataAction {
	None,
	Blacklist,
	Clear,
	ClearBlacklist,
	ToggleFreeze,
	RateLimit,
};

class PinCushion : public IPluginInterface {
public:
	void OnEngineInitialized() override;
	PinCushion();
	~PinCushion() override;
	void OnDrawMenu() override;
	void OnDrawUI(bool p_HasFocus) override;

private:
	void OnFrameUpdate(const SGameUpdateEvent& p_UpdateEvent);
	DECLARE_PLUGIN_DETOUR(PinCushion, void, OnLoadScene, ZEntitySceneContext* th, ZSceneData& p_SceneData);
	DECLARE_PLUGIN_DETOUR(PinCushion, bool, OnPinOutput, ZEntityRef entity, uint32 pinId, const ZObjectRef& data);
	//DECLARE_PLUGIN_DETOUR(PinCushion, bool, OnPinInput, ZEntityRef entity, uint32 pinId, const ZObjectRef& data);

	auto getRecentPinIterator(uint32 pinId) -> std::list<PinData>::iterator {
		for (auto it = pinData.begin(); it != pinData.end(); ++it)
			if (it->id == pinId) return it;
		return pinData.end();
	}

	auto getUpdateDataAction() const -> UpdateDataAction {
		return updateDataAction;
	}

	auto haveUpdateDataAction() const -> bool {
		return updateDataAction != UpdateDataAction::None;
	}

private:
	std::set<ZHMPin> pinBlacklist;
	std::list<PinData> pinData;
	std::vector<PinData> frozenPinData;
	std::vector<PinData> displayPinData;
	//std::shared_mutex pinDataLock;
	std::chrono::system_clock::time_point lastCleanupTime;
	std::chrono::system_clock::time_point lastDisplayUpdateTime;
	std::shared_mutex displayDataLock;
	std::shared_mutex filterInputLock;
	double lastLogTime = 0;
	double lastFreqPruneTime = 0;
	UpdateDataAction updateDataAction = UpdateDataAction::None;
	ZHMPin blacklistPin = static_cast<ZHMPin>(0);
	uint64 rateLimit = 15;
	int uiRateLimit = 15;
	bool enableRateBlock = true;
	bool hooksInstalled = false;
	bool m_ShowMessage = false;
	char filterInput[40] = "";
	std::string_view filterInputSV;
};

DEFINE_ZHM_PLUGIN(PinCushion)
