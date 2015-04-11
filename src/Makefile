.c.o:
	$(CC) $(CFLG) -c $^
CC=gcc
CFLG=-Wall -g -I/usr/include/freetype2
LFLG=-lGL -lglfw -lOpenCL -pthread -lfreetype
SYS=m_sysmanager.o v_visualsys.o p_physicsys.o a_audiosys.o l_logicsys.o
MAIN=main.o m_main.o m_memory.o m_args.o m_hashlib.o
VIS=v_buffer.o v_entity.o v_loader.o v_memory.o v_model.o v_program.o v_texture.o
PHY=p_buffer.o p_dispatcher.o p_entity.o p_loader.o p_memory.o p_program.o
LOG=l_entity.o l_memory.o
AUD=a_entity.o
OBJ=$(MAIN) $(SYS) $(VIS) $(PHY) $(LOG) $(AUD)
PRG=YatM

all:$(PRG)

YatM:$(OBJ)
	$(CC) $(CFLG) $(LFLG) -o $@ $^

clean:
	/bin/rm -f $(PRG) *.o a.out
	
