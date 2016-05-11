all: main
	mv ./core/sst ./

main:
	make -C core all

clean:
	make -C core clean
	rm -rf sst
