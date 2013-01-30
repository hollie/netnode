bestsave on
rule pcb (width 0.200000)
rule pcb (clearance 0.200000)
rule pcb (clearance 0.450000 (type via_area))
rule pcb (clearance 0.450000 (type wire_area))
cost via  high
bus diagonal
fanout 5
route 20
clean 2
route 25 16
clean 2
filter 5
recorner diagonal
unit inch
recorner diagonal 1 1 1
recorner diagonal .5 .5 .5
recorner diagonal .25 .25 .25
recorner diagonal .2 .2 .2
recorner diagonal .1 .1 .1
recorner diagonal .05 .05 .05
recorner diagonal .025 .025 .025
write script /Users/lieven/projects/likatronix/netnode/hardware/ioboard v1/eagle/ioboard v1.scr 
status_file
