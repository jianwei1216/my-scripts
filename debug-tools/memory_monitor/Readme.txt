Before you use this tool,you should read this instruction.

This tool is used to monitor the process virtual memory changed.It can record data of process virtual memory, and drawing it into a picture.If you use it,you need to know the following points:

1.This tool can monitor the following processes digioceand; digioceanfs; nfs; quotad; rebalance; self-heal and every bricks.So the process need in this context that what you want to monitor.

2.You should check whether there is a DIR named "monitor_data" in /mnt,if there is,you need renamed or delect it,because this tool will "mkdir monitor_data" in /mnt (you must be check all node).

3.You must install "gnuplot" on at least one node(you can install gnuplot_4.6.0),because this software can use data and drawing it into a picture.But if you needn't picture,you needn't install it.When you install it completed,you need test whether it is in use,you can follow these steps to test:
    (1)Input "gnuplot" in terminal.enter
    (2)Input "plot sin(x)" in terminal.enter
    (3)If it can drawing a picture of sin(x),it mean gnuplot can be used normally

4.Create a service,then test its related processes,you can follow these steps to monitor:
    (1)You can running "memory_monitor.sh" in one or all node,it depends how many node you want monitor,and if you running it you will see the help.
    (2) m: If you want monitor the process virtual memory,you can input "m" and enter,then the program will enter an infinite loop,if you want to end the monitor,you can press "Ctrl+C"(But if you want to carry out other operations,you need running "memory_monitor.sh" again),and the monitor data will saved in /mnt/monitor_data/orignal_data,you can view it.
    (3) c: If you want to collect data from the other nodes to the current node,first you should input "c", then you need to determine the choice of which copies, if you want copy
	all other node data to the current node, you should input "t"; if you want copy portion node data, you should input "p", then input the hostname or ip which node you want copy
	, the middle separated by spaces. (But you must establishment of SSH keys links first.),then other nodes data will saved in current node /mnt/monitor_data/orignal_data,you can view it.
    (4) a: If you want to lookup which PID of process changed,you can input "a" on current node,then this tool will analyse orignal data,and save analysis data in /mnt/monitor_data/analy_data,
    (5) g: If you want drawing the data into picture,you can input "g" in which node install gnuplot,But you should analyse orignal data frist.
    (6) r: If you want save the test data,you can input "r",but you better running this command after running m;c;a;g command,to ensure that the backup data is up to date. 
    (7) h: If you want some help,you can input "h".
    (8) q: When you want quit, you can input "q".But if you input "m" frist,then input "q",this will have no effect.
 
Tips:
    The Monitoring cycle in the code is 10s,if you think the time unreasonable,you can change it,the code in "memory_monitor.sh" 208;209 line.
    sleep 10
    let timer=$timer+10
    Change "10" to what you want.	
