#include "Settings.h"

Settings* Settings::GetSingleton()
{
	static Settings singleton;
	return std::addressof(singleton);
}

Settings::Settings()
{
	CSimpleIniA ini;
	ini.SetUnicode();

	ini.LoadFile(path);

	ini::get_value(ini, enableCopyPaste, "Settings", "Copy Paste", ";Copy text from clipboard and paste into console");
	ini::get_value(ini, enableCommandCache, "Settings", "Cache Commands", ";Cache commands between game instances");

	ini::get_value(ini, commandHistoryLimit, "Settings", "Command History Limit", ";Number of commands to save\n;Default: 50");

	ini::get_value(ini, primaryKey, "CopyPaste", "Primary Key", ";Keyboard scan codes : https://wiki.nexusmods.com/index.php/DirectX_Scancodes_And_How_To_Use_Them\n;Default: Left Ctrl");
	ini::get_value(ini, secondaryKey, "CopyPaste", "Secondary Key", ";Default: V");
	ini::get_value(ini, pasteType, "CopyPaste", "Paste Type", ";0 - insert text at cursor position | 1 - append text");
	ini::get_value(ini, inputDelay, "CopyPaste", "Input delay", ";Delay between key press and text paste (in milliseconds)");

	(void)ini.SaveFile(path);
}

void Settings::SaveCommands(std::vector<std::string>& a_commands) const
{
	CSimpleIniA ini;
	ini.SetUnicode();

	ini.LoadFile(path);

	ini.Delete("ConsoleCommands", nullptr);
	if (!a_commands.empty()) {
		if (a_commands.size() > commandHistoryLimit) {
			std::ranges::reverse(a_commands);
			a_commands.resize(commandHistoryLimit);
			std::ranges::reverse(a_commands);
		}
		for (std::uint32_t i = 0; i < a_commands.size(); i++) {
			ini.SetValue("ConsoleCommands", fmt::format("Command{}", i).c_str(), a_commands[i].c_str());
		}
	}
	(void)ini.SaveFile(path);
}

std::vector<std::string> Settings::LoadCommands() const
{
	std::vector<std::string> commands{};

	CSimpleIniA ini;
	ini.SetUnicode();

	ini.LoadFile(path);

    if (const auto section = ini.GetSection("ConsoleCommands"); section && !section->empty()) {
		// GetSection sorts by string order, smh SimpleINI
        std::map<int, std::string> commandMap;
	    for (const auto& [key,entry] : *section) {
	        commandMap.emplace(key.nOrder, entry);
		}
		for (const auto& entry : commandMap | std::views::values) {
		    commands.emplace_back(entry);
		}
	}

	if (commands.size() > commandHistoryLimit) {
		std::ranges::reverse(commands);
		commands.resize(commandHistoryLimit);
		std::ranges::reverse(commands);
	}

	return commands;
}

void Settings::ClearCommands() const
{
	CSimpleIniA ini;
	ini.SetUnicode();

	ini.LoadFile(path);

	ini.Delete("ConsoleCommands", nullptr);

	(void)ini.SaveFile(path);
}
