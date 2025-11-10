#pragma once

#include "Components/Component.h"
#include "Effects/EffectIdentifier.h"

#include <queue>
#include <unordered_map>
#include "../include/Sync/Lock.h"

using DWORD = unsigned long;

class EffectShortcuts : public Component
{
	std::unordered_map<int, std::vector<EffectIdentifier>> m_AvailableShortcuts;

        std::queue<EffectIdentifier> m_EffectQueue;
        // fiber-safe: removed STL mutex
        SrwLock m_EffectQueueLock;

  public:
	EffectShortcuts();

	virtual void OnRun() override;
	virtual void OnKeyInput(DWORD key, bool repeated, bool isUpNow, bool isCtrlPressed, bool isShiftPressed,
	                        bool isAltPressed) override;
};