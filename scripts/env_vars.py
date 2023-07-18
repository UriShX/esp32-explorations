Import("env")

import re

#: The custom-option that when defined, modifies the program-version.
OTA_CDN_URL = "custom_cdn_url"
#: The custom-option that when defined, modifies the program-version.
RELEASE_FOLDER = "custom_release_folder"
#: The custom-option that when defined, modifies the program-version.
OTA_CONFIGURATION_FILE = "custom_ota_config"
#: The custom-option that when defined, modifies the program-version.
PROG_VERSION_OPTION = "custom_prog_version"
#: The custom-option that when defined, modifies the program-version.
SEMANTIC_VERSIONING_OPTION = "custom_semantic_version"

cdn_url, cdn_url_src = (env.GetProjectOption(OTA_CDN_URL, None), "custom_option")
release_folder, release_folder_src = (env.GetProjectOption(RELEASE_FOLDER, None), "custom_option")
ota_config_filename, ota_config_filename_src = (env.GetProjectOption(OTA_CONFIGURATION_FILE, None), "custom_option")

fw_version, fw_version_src = (env.GetProjectOption(PROG_VERSION_OPTION, None), "custom_option")
semantic_fw_version, semantic_fw_version_src = (env.GetProjectOption(SEMANTIC_VERSIONING_OPTION, None), "custom_option")

_set_semantic_versioning = False

def test_version_match():
    try:
        match = re.search('\d{1,3}\.\d{1,3}\.\d{1,3}[a-zA-Z]?', fw_version)
        if match is not None:
            return True
        else:
            return False
    except AttributeError:
        print("Firmware version not found in platformio.ini")
        return True
        
_defines = [
        # ("MYSTRING", env.StringifyMacro('Text is "Quoted"')),
    ("CDN_URL", env.StringifyMacro(f"{cdn_url}{ota_config_filename}")),
]

if semantic_fw_version is not None or fw_version is not None:
    if test_version_match():
        _defines.append(("EOTA_SEMANTIC_VERSIONING", 1))
        
env.Append(CPPDEFINES=_defines)
