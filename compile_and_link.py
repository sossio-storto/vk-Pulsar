import os
import subprocess
import glob

cwd = r"d:\vk-Pulsar-main\vk-Pulsar-main"
cc = r"C:\Program Files (x86)\Freescale\CW for MPC55xx and MPC56xx 2.10\PowerPC_EABI_Tools\Command_Line_Tools\mwcceppc.exe"
kamek_linker = os.path.join(cwd, r"KamekLinker\Kamek.exe")

cflags = [
    "-I-",
    "-i", os.path.join(cwd, "KamekInclude"),
    "-i", os.path.join(cwd, "GameSource"),
    "-i", os.path.join(cwd, "PulsarEngine"),
    "-opt", "all",
    "-inline", "auto",
    "-enum", "int",
    "-fp", "hard",
    "-sdata", "0",
    "-sdata2", "0",
    "-maxerrors", "1",
    "-func_align", "4"
]

# Ensure build dir exists
build_dir = os.path.join(cwd, "build")
if not os.path.exists(build_dir):
    os.makedirs(build_dir)

# Clean old objects
print("Cleaning old objects...")
for f in glob.glob(os.path.join(build_dir, "*.o")):
    try:
        os.remove(f)
    except Exception as e:
        print(f"Failed to remove {f}: {e}")

# Compile kamek.cpp
kamek_cpp = os.path.join(cwd, r"KamekInclude\kamek.cpp")
kamek_o = os.path.join(build_dir, "kamek.o")
print("Compiling kamek.cpp...")
cmd = [cc] + cflags + ["-c", "-o", kamek_o, kamek_cpp]
res = subprocess.run(cmd, capture_output=True, text=True, cwd=cwd)
if res.returncode != 0:
    print("Failed to compile kamek.cpp!")
    print(res.stdout)
    print(res.stderr)
    exit(1)

# Find all cpp files in PulsarEngine
cpp_files = []
for root, dirs, files in os.walk(os.path.join(cwd, "PulsarEngine")):
    for f in files:
        if f.endswith(".cpp"):
            cpp_files.append(os.path.join(root, f))

# Compile each cpp file
objects = []
print(f"Compiling {len(cpp_files)} cpp files...")
for cpp in cpp_files:
    basename = os.path.basename(cpp)
    obj_name = os.path.splitext(basename)[0] + ".o"
    obj_path = os.path.join(build_dir, obj_name)
    
    cmd = [cc] + cflags + ["-c", "-o", obj_path, cpp]
    res = subprocess.run(cmd, capture_output=True, text=True, cwd=cwd)
    if res.returncode != 0:
        print(f"Failed to compile {cpp}!")
        print(res.stdout)
        print(res.stderr)
        exit(1)
    objects.append(obj_path)

# Link
print("Linking...")
symbols = os.path.join(cwd, r"GameSource\symbols.txt")
versions = os.path.join(cwd, r"GameSource\versions.txt")
code_pul = os.path.join(build_dir, "Code.pul")
map_file = os.path.join(build_dir, "Code.$KV$.map")

link_cmd = [
    kamek_linker,
    kamek_o
] + objects + [
    "-dynamic",
    f"-externals={symbols}",
    f"-versions={versions}",
    f"-output-combined={code_pul}",
    f"-output-map={map_file}"
]

res = subprocess.run(link_cmd, capture_output=True, text=True, cwd=cwd)
print("Linker return code:", res.returncode)
if res.returncode != 0:
    print("Link failed!")
    print(res.stdout)
    print(res.stderr)
    exit(1)

print("Build succeeded! Code.pul created at:", code_pul)
print(f"Code.pul size: {os.path.getsize(code_pul)} bytes")
