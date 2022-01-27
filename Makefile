OUTPUT=./bin

.PHONY: compile clean
compile: clean
	mkdir -p $(OUTPUT)
	$(MAKE) -C src

.PHONY: clean
clean:
	rm -rf $(OUTPUT)
