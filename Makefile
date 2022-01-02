start:
	cd build && \
	ln -svf ../config.json . && \
	cmake .. -DCMAKE_BUILD_TYPE=Debug && \
	make -j7 && \
	compdb -p ./ list > ../compile_commands.json && \
	./RinPhone

release:
	cd build && \
	ln -svf ../config.json . && \
	cmake .. -DCMAKE_BUILD_TYPE=Release && \
	make -j7 && \
	compdb -p ./ list > ../compile_commands.json && \
	./RinPhone

tc:
	./build/test/RinPhone_test

watch:
	make start
	bash scripts/watch.sh
