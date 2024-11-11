# Set FLAG based on the input argument
FLAG=""
[[ "$1" = "-d" ]] && FLAG="-s -S -d int"

# Set common qemu options
QEMU_CMD="qemu-system-x86_64 $FLAG -hda Image/x64BareBonesImage.qcow2 -m 512"

if [[ "$(uname)" == "Linux" ]]; then
    # Linux specific execution
    $QEMU_CMD
else
    # macOS specific execution
    export AUDIO_DRIVER="coreaudio"
    $QEMU_CMD -audiodev $AUDIO_DRIVER,id=audio0 -machine pcspk-audiodev=audio0
fi
