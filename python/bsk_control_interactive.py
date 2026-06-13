import bsk_control

bskCtrl = bskControl
bskCtrl.discover()

counter = 0
for command in COMMANDS:
    print(f"{counter} - {command}")
    counter +=1

choice = input("Enter command number: ")

devices = bskCtrl.devices.keys()
devices.append("ALL")

counter = 0
for device in devices:
    print(f"{counter} - {device}")
    counter +=1

chosen = input()"Enter device number: ")

bskCtrl.send_command(devices[chosen], COMMANDS[int(choice)])
