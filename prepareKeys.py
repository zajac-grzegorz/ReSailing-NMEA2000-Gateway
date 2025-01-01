Import("env")

import json
import shutil

device_type = env.GetProjectOption("custom_device_type")

# set additional build flag for "sender" mode
if (device_type == "sender"):
    env.Append(CCFLAGS=["-D RE_BUILD_AS_SENDER"])

gateway_file_path = "config/reInitData_gateway.json"
sender_file_path = "config/reInitData_sender.json"
keys_header_file_path = "include/ReStorageKeys.h"
keys_json_file_path = "config/reInitData.json"

print(f"----------------------------------------------------------------------------------");
print(f"Building '{device_type}' version of the software")

if (device_type == "gateway") :
    shutil.copyfile(gateway_file_path, keys_json_file_path)
    print(f"Copied file '{gateway_file_path}' version of the software ")
elif (device_type == "sender") :
    shutil.copyfile(sender_file_path, keys_json_file_path)
    print(f"Copied file '{sender_file_path}' version of the software ")
else :
    print(f"Using default config file")

print(f"----------------------------------------------------------------------------------");

# Open the file in write mode
with open(keys_header_file_path, 'w') as keys_file:
    # Write content to the file
    keys_file.write('#pragma once\n\n')

    # Opening JSON file
    with open(keys_json_file_path) as json_file:
        data = json.load(json_file)

        # for each key add entry to the .h file
        for key in data.keys():
            #print(f'inline const char* key_{key} = "{key}";')
            keys_file.write(f'inline const char* key_{key} = "{key}";\n')
    
    print(f"File with storage keys'{keys_header_file_path}' created successfully")
    print(f"----------------------------------------------------------------------------------");
