# get all .c files
srcFiles=$(wildcard *.c)
# all .c files --> .o files
objFiles=$(patsubst %.c,%.o,$(srcFiles))

all:$(objFiles)
%.o:%.c
	gcc $^ -o $@

.PHONY:print
print:
	@echo $(srcFiles)
	@echo $(objFiles)

# 定义伪目标，防止有歧义
.PHONY:clean
clean:
	-rm -f $(objFiles)
