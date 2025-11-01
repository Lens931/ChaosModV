#include <stdafx.h>

#include "EffectShortcuts.h"

#include "Components/EffectDispatcher.h"
#include "Effects/EnabledEffects.h"

EffectShortcuts::EffectShortcuts() : Component()
{
	for (const auto &effectData : GetFilteredEnabledEffects())
		if (effectData->ShortcutKeycode > 0 && !effectData->IsHidden())
			m_AvailableShortcuts[effectData->ShortcutKeycode].push_back(effectData->Id);
}

void EffectShortcuts::OnRun()
{
	if (!m_EffectQueue.empty())
	{
            AutoLock lock(m_EffectQueueLock); // fiber-safe: removed STL mutex
		while (!m_EffectQueue.empty())
		{
			auto &id = m_EffectQueue.front();

			if (ComponentExists<EffectDispatcher>())
				GetComponent<EffectDispatcher>()->DispatchEffect(id);

			m_EffectQueue.pop();
		}
	}
}

void EffectShortcuts::OnKeyInput(DWORD key, bool repeated, bool isUpNow, bool isCtrlPressed, bool isShiftPressed,
                                 bool isAltPressed)
{
	if (repeated)
		return;

	key += (isCtrlPressed << 10) + (isShiftPressed << 9) + (isAltPressed << 8);

	if (m_AvailableShortcuts.contains(key))
	{
            AutoLock lock(m_EffectQueueLock); // fiber-safe: removed STL mutex
		for (auto &effectId : m_AvailableShortcuts.at(key))
			m_EffectQueue.push(effectId);
	}
}
