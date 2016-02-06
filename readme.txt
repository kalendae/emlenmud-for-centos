Welcome to the free version of LOC 2005-2006.

This uses the map used previous to the map used in 2006. If you want the newer version of the loc map please download EmlenMudLOC2006.zip.


How to setup a mud.

1: Find yourself a secure host. Examples Wolfpaw.net, kyndig.com, or check out TMC for other sites that host a mud. http://www.mudconnect.com/resources/Mud_Resources:Mud_Hosting.html

2: Unzip the EmlenMudLOC2006.zip and save it on your hard drive.

3: Download PuTTY and FTP Commander. They will be your best friends.

4: Upload the mud using FTP Commander to the site you have chosen to run the mud.
Make sure you have four (4) folders on your site. You will need plr, world, src, and log.

When you log into your server, using PuTTY, type ls. You should see your folders on the site. ls is like the dir command in DOS. ps ux is another command you will want to get to know. It shows the processes you are running and say your mud goes into a loop and you need to stop it you will need to type ps ux to get this type of output:

USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
locstaff 11188  0.0  0.1   4592  2092 ?        S    Apr13   0:15 /bin/csh -f ./startup
locstaff 17118  0.0  0.1   4620  2104 ?        S    Apr21   0:00 /bin/csh -f ./startup
locstaff 23713  0.0  0.1   8676  3536 ?        S    Apr22   0:01 sshd: locstaff@pts/15
locstaff 23714  0.0  0.1   4792  2812 pts/15   Ss   Apr22   0:00 -bash
locstaff 18303  0.2  0.4  12748  9464 ?        S    04:18   1:02 ../src/LoC 3222
locstaff  4667  0.1  0.4  12600  9032 ?        S    09:17   0:15 ../src/LoC 3235
locstaff 19196  0.0  0.0   4060  1640 pts/15   R+   11:33   0:00 ps ux


If you need to stop a process you need to kill it. So find the PID and then type kill -9 (PID NUMBER)
And the process will be killed.

If you want to learn more about linux go out and buy a book called Linux for Dummies. It is a great book for beginner linux runners.

src is another folder (IMPORTANT FOLDER) This folder should have all the mud code files. They are called .c files. There should be a makefile and a startup file. There are a few other files but you might never use them.

Makefile: What the make file is used for is to compile the mud to make a exe file you run. Makefile will take all the .c files and create .o files which will make up LoC file. You mud run makefile (type makefile) before you can run your mud. You need to have the LoC file to have the mud run when you type 
./startup & from the next section of the startup file.


Startup file: The startup file helps you with having control of your port number and helps the mud find the world files. Do not change any information in this file if you do not have to.  The only think that is safe to change in this file is your port number.  

#! /bin/csh -f
set port = 3333    ? You can change this number to the allowed ports of your server.
if ( "$1" != "" ) set port="$1"

# Change to area/world directory.
cd ../world

# Set limits.
nohup
limit stack 1024k
if ( -e shutdown.txt) rm -f shutdown.txt

while ( 1 )
    set index = 1000
    while (1)
	set logfile = ../log/$index.log
	if ( ! -e $logfile ) break
	@ index++
    end

    date > $logfile

    ../src/LoC $port >&! $logfile

end

To use the startup file and get your mud running type ./startup &

Ok now the mud should be running log into the mud with the ip address/domain name/ or whatever just like you would always do and then create a character. Once you have made yourself log off and go into your plr file and you will see all player files example Kilith and player backup files Kilith.bak.

Ok depending on your linux experience linux has two (2) editors. VIM and pico. I use pico because it is simple to use.

Ok log into your shell (if you are not already) and type cd plr to go into your plr folder. Then type pico your Username (it is case sensitive) and then hit ctrl-w and at the bottom a search bar will come up and type level.

Your level should look like .1 you will want to change that to .156 which equals level 110. once you are the right level and everything you are ready to do all your building and whatever else you want.

If at any time you have a problem you can contact me at 
AIM kilith mpv
MSN jaker_1976@hotmail.com
Yahoo kilith_mpv@yahoo.com
Email jaker_1976@hotmail.com

I will give you ALL the assistance you need in getting yourself going.

taonotes:

- remove pkdata.dat and ratings and topten etc because those users don't exist
  and will cause a server crash

