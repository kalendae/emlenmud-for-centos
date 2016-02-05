

CC      = gcc 
NOCRYPT =
1BUILD  = 
C_FLAGS = $(1BUILD) -O2 -m486 -g -DLinux -DNEED_EXTERN $(PROF) $(NOCRYPT)
L_FLAGS = $(PROF) 

O_FILES = aedit.o alien.o ansi_out.o ar.o arena.o  armor.o autoeq.o \
aw.o auction.o bank.o buysell.o chan.o channels.o char.o clan.o  \
climb.o cmd.o config.o  consid.o comm.o create.o \
crsh.o dam.o dam2.o db2.o death.o def.o draw.o edit.o fgt.o \
give.o grnd.o guild.o help.o hnd.o inetd.o info.o int.o look.o  \
medit.o mgc.o misc.o move.o note.o object.o oc.o oedit.o pb.o  pc_wiz.o \
pedit.o pets.o pinfo.o power.o prompt.o prse.o ptrack.o  quit.o redit.o \
reloc.o remort.o rfd.o room.o score.o script3.o  sedit.o shell.o spec.o \
spl.o skill.o sv.o syslag.o toptr.o upd.o view.o warmth.o yell.o \

O_FILES_T = utils.o

O_FILES_TH = install.o

LoC: $(O_FILES) 
	rm -f LoC
	$(CC) $(L_FLAGS) -o LoC $(O_FILES)

util: $(O_FILES_T) 
	rm -f util 
	$(CC) $(L_FLAGS) -o util $(O_FILES_T)
	cp util ../newworld

install: $(O_FILES_TH)
	rm -f inst
	$(CC) $(L_FLAGS) -o inst $(O_FILES_TH)

.c.o: emlen.h
	$(CC) -c $(C_FLAGS) $<

clean:
	rm *.o
