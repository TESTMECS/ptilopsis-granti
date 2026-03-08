default:
	@just --list

ray_lib_path := "/nix/store/lp7anw24jqsgyha19byxmb45833wxg15-raylib-5.5"
ray_gui_path := "/nix/store/jm9g8vxqxck5r85hz364i6zn2pf63vff-raygui"

alias b := build
alias c := commit

build:
	gcc main.c \
		-L {{ray_lib_path}}/lib \
		-lraylib -lm -lpthread -ldl -lrt \
		-o main.out

commit msg:
	git add . && git commit -m "{{msg}}"
	
