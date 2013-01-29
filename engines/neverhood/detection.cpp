/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "base/plugins.h"

#include "engines/advancedDetector.h"
#include "common/file.h"

#include "neverhood/neverhood.h"


namespace Neverhood {

struct NeverhoodGameDescription {
	ADGameDescription desc;

	int gameID;
	int gameType;
	uint32 features;
	uint16 version;
};

const char *NeverhoodEngine::getGameId() const {
	return _gameDescription->desc.gameid;
}

uint32 NeverhoodEngine::getFeatures() const {
	return _gameDescription->features;
}

Common::Platform NeverhoodEngine::getPlatform() const {
	return _gameDescription->desc.platform;
}

uint16 NeverhoodEngine::getVersion() const {
	return _gameDescription->version;
}

bool NeverhoodEngine::isDemo() const {
	return _gameDescription->desc.flags & ADGF_DEMO;
}

}

static const PlainGameDescriptor neverhoodGames[] = {
	{"neverhood", "The Neverhood Chronicles"},
	{0, 0}
};

namespace Neverhood {

static const NeverhoodGameDescription gameDescriptions[] = {

	{
		// Neverhood English version
		{
			"neverhood",
			0,
			AD_ENTRY1s("hd.blb", "22958d968458c9ff221aee38577bb2b2", 4279716),
			Common::EN_ANY,
			Common::kPlatformWindows,
			ADGF_NO_FLAGS,
			GUIO1(GUIO_NONE)
		},
		0,
		0,
		0,
		0,
	},

	{
		// Neverhood English demo version
		{
			"neverhood",
			"Demo",
			AD_ENTRY1s("nevdemo.blb", "05b735cfb1086892bec79b54dca5545b", 22564568),
			Common::EN_ANY,
			Common::kPlatformWindows,
			ADGF_DEMO,
			GUIO1(GUIO_NONE)
		},
		0,
		0,
		0,
		0,
	},

	{ AD_TABLE_END_MARKER, 0, 0, 0, 0 }
};

} // End of namespace Neverhood

class NeverhoodMetaEngine : public AdvancedMetaEngine {
public:
	NeverhoodMetaEngine() : AdvancedMetaEngine(Neverhood::gameDescriptions, sizeof(Neverhood::NeverhoodGameDescription), neverhoodGames) {
		_singleid = "neverhood";
	}

	virtual const char *getName() const {
		return "Neverhood Engine";
	}

	virtual const char *getOriginalCopyright() const {
		return "Neverhood Engine (C) Infogrames";
	}

	virtual bool hasFeature(MetaEngineFeature f) const;
	virtual bool createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const;

	SaveStateList listSaves(const char *target) const;
	virtual int getMaximumSaveSlot() const;
	void removeSaveState(const char *target, int slot) const;
	SaveStateDescriptor querySaveMetaInfos(const char *target, int slot) const;

};

bool NeverhoodMetaEngine::hasFeature(MetaEngineFeature f) const {
	return
		(f == kSupportsListSaves) ||
		(f == kSupportsLoadingDuringStartup) ||
		(f == kSupportsDeleteSave) ||
	   	(f == kSavesSupportMetaInfo) ||
		(f == kSavesSupportThumbnail) ||
		(f == kSavesSupportCreationDate) ||
		(f == kSavesSupportPlayTime);
}

bool Neverhood::NeverhoodEngine::hasFeature(EngineFeature f) const {
	return
		(f == kSupportsRTL) ||
		(f == kSupportsLoadingDuringRuntime) ||
		(f == kSupportsSavingDuringRuntime);
}

bool NeverhoodMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	const Neverhood::NeverhoodGameDescription *gd = (const Neverhood::NeverhoodGameDescription *)desc;
	if (gd) {
		*engine = new Neverhood::NeverhoodEngine(syst, gd);
	}
	return gd != 0;
}

SaveStateList NeverhoodMetaEngine::listSaves(const char *target) const {
	Common::SaveFileManager *saveFileMan = g_system->getSavefileManager();
	Neverhood::NeverhoodEngine::SaveHeader header;
	Common::String pattern = target;
	pattern += ".???";

	Common::StringArray filenames;
	filenames = saveFileMan->listSavefiles(pattern.c_str());
	Common::sort(filenames.begin(), filenames.end());	// Sort (hopefully ensuring we are sorted numerically..)

	SaveStateList saveList;
	for (Common::StringArray::const_iterator file = filenames.begin(); file != filenames.end(); file++) {
		// Obtain the last 3 digits of the filename, since they correspond to the save slot
		int slotNum = atoi(file->c_str() + file->size() - 3);

		if (slotNum >= 0 && slotNum <= 999) {
			Common::InSaveFile *in = saveFileMan->openForLoading(file->c_str());
			if (in) {
				if (Neverhood::NeverhoodEngine::readSaveHeader(in, false, header) == Neverhood::NeverhoodEngine::kRSHENoError) {
					saveList.push_back(SaveStateDescriptor(slotNum, header.description));
				}
				delete in;
			}
		}
	}

	return saveList;
}

int NeverhoodMetaEngine::getMaximumSaveSlot() const {
	return 999;
}

void NeverhoodMetaEngine::removeSaveState(const char *target, int slot) const {

	Common::SaveFileManager *saveFileMan = g_system->getSavefileManager();
	Common::String filename = Neverhood::NeverhoodEngine::getSavegameFilename(target, slot);

	saveFileMan->removeSavefile(filename.c_str());

	Common::StringArray filenames;
	Common::String pattern = target;
	pattern += ".???";
	filenames = saveFileMan->listSavefiles(pattern.c_str());
	Common::sort(filenames.begin(), filenames.end());	// Sort (hopefully ensuring we are sorted numerically..)

	for (Common::StringArray::const_iterator file = filenames.begin(); file != filenames.end(); ++file) {
		// Obtain the last 3 digits of the filename, since they correspond to the save slot
		int slotNum = atoi(file->c_str() + file->size() - 3);

		// Rename every slot greater than the deleted slot,
		// Also do not rename quicksaves.
		if (slotNum > slot && slotNum < 990) {
			saveFileMan->renameSavefile(file->c_str(), filename.c_str());

			filename = Neverhood::NeverhoodEngine::getSavegameFilename(target, ++slot);
		}
	}

}

SaveStateDescriptor NeverhoodMetaEngine::querySaveMetaInfos(const char *target, int slot) const {
	Common::String filename = Neverhood::NeverhoodEngine::getSavegameFilename(target, slot);
	Common::InSaveFile *in = g_system->getSavefileManager()->openForLoading(filename.c_str());

	if (in) {
		Neverhood::NeverhoodEngine::SaveHeader header;
		Neverhood::NeverhoodEngine::kReadSaveHeaderError error;

		error = Neverhood::NeverhoodEngine::readSaveHeader(in, true, header);
		delete in;

		if (error == Neverhood::NeverhoodEngine::kRSHENoError) {
			SaveStateDescriptor desc(slot, header.description);

			desc.setDeletableFlag(false);
			desc.setWriteProtectedFlag(false);
			desc.setThumbnail(header.thumbnail);
			int day = (header.saveDate >> 24) & 0xFF;
			int month = (header.saveDate >> 16) & 0xFF;
			int year = header.saveDate & 0xFFFF;
			desc.setSaveDate(year, month, day);
			int hour = (header.saveTime >> 16) & 0xFF;
			int minutes = (header.saveTime >> 8) & 0xFF;
			desc.setSaveTime(hour, minutes);
			desc.setPlayTime(header.playTime * 1000);
			return desc;
		}
	}

	return SaveStateDescriptor();
}

#if PLUGIN_ENABLED_DYNAMIC(NEVERHOOD)
	REGISTER_PLUGIN_DYNAMIC(NEVERHOOD, PLUGIN_TYPE_ENGINE, NeverhoodMetaEngine);
#else
	REGISTER_PLUGIN_STATIC(NEVERHOOD, PLUGIN_TYPE_ENGINE, NeverhoodMetaEngine);
#endif
