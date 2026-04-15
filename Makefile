# Run driver integration test
test_driver_compile:
	clang -framework IOKit -framework CoreFoundation \
        -I./accel \
        -o test_driver \
        accel/tests/test_driver.c accel/accel_driver.c accel/accel_data.c -lm

test_driver_run:
	./test_driver

# Run test parsing data
test_parse_compile:
	clang -I./accel -o test_parse accel/tests/test_parse.c accel/accel_data.c -lm 

test_parse_run:
	./test_parse

.PHONY: clean
clean:
	rm -f $(TARGET) *.o
