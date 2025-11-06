import os
import shutil

Import("env")

def copy_firmware_to_root(source, target, env):
    """Copy firmware bin file to project root."""
    project_dir = env.get("PROJECT_DIR")
    firmware_name = os.path.basename(target[0].get_path())
    firmware_path = os.path.join(project_dir, firmware_name)

    print(f"Copying {target[0].path} to {firmware_path}")
    shutil.copy(target[0].path, firmware_path)
    print("Firmware copied to project root.")

env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", copy_firmware_to_root)