Import("env")

#: The custom-option that when defined, modifies the program-version.
OTA_CDN_URL = "custom_cdn_url"
#: The custom-option that when defined, modifies the program-version.
RELEASE_FOLDER = "custom_release_folder"
#: The custom-option that when defined, modifies the program-version.
OTA_CONFIGURATION_FILE = "custom_ota_config"

cdn_url, cdn_url_src = (env.GetProjectOption(OTA_CDN_URL, None), "custom_option")
release_folder, release_folder_src = (env.GetProjectOption(RELEASE_FOLDER, None), "custom_option")
ota_config_filename, ota_config_filename_src = (env.GetProjectOption(OTA_CONFIGURATION_FILE, None), "custom_option")

env.Append(CPPDEFINES=[
    # ("MYSTRING", env.StringifyMacro('Text is "Quoted"')),
    ("CDN_URL", env.StringifyMacro(f"{cdn_url}{ota_config_filename}")),
])
