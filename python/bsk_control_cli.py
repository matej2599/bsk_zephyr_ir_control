from bsk_control import BskControl, BSK_COMMANDS
import sys

args = sys.argv[1:]
for arg in args:
    print(arg)

if len(args) != 2:
    print("example: bsk_control_cli.py ALL BSK_TURN_ON_OFF")
    print("Available commands:")
    for command in BSK_COMMANDS:
        print(f" {command}")
    exit()

bskCtrl = BskControl()
bskCtrl.discover()
bskCtrl.send_command(args[0], args[1])
